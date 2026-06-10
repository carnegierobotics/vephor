#!/usr/bin/env python3
"""
Vephor Web Client Gateway
Copyright 2026 Carnegie Robotics, LLC / Steve Landers
This gateway hosts the web client, translates Vephor's custom JSONB binary TCP protocol
into WebSocket payloads, and supports bi-directional interaction.
"""

import os
import sys
import json
import asyncio
import struct
import base64
import argparse
import threading
from http.server import SimpleHTTPRequestHandler, ThreadingHTTPServer

# Find workspace root
PROJECT_ROOT = os.path.abspath(os.path.join(os.path.dirname(__file__), '..'))

# Active connections track
# keys: conn_id (int)
# values: dict with { "reader": StreamReader, "writer": StreamWriter, "peer": str, "direction": "inbound"/"outbound" }
active_connections = {}
connection_metadata = {}
connection_objects = {}
connection_payloads = {}
conn_counter = 1
ws_clients = set()

# Lock for state management
conn_lock = threading.Lock()

class VephorHTTPHandler(SimpleHTTPRequestHandler):
    """
    HTTP handler that serves web client files and proxies
    asset requests directly to core/assets/ avoiding copy duplication.
    """
    def translate_path(self, path):
        import urllib.parse
        path_clean = path.split('?')[0]
        path_decoded = urllib.parse.unquote(path_clean)
        
        # If it's an absolute path that exists and is within our project root, serve it directly
        if os.path.isabs(path_decoded) and os.path.exists(path_decoded):
            if path_decoded.startswith(PROJECT_ROOT):
                return path_decoded
                
        # If the path contains 'core/assets/' or 'assets/' anywhere, extract relative path
        for marker in ['/core/assets/', '/assets/']:
            if marker in path_decoded:
                idx = path_decoded.find(marker)
                relative = path_decoded[idx + len(marker):]
                target_path = os.path.join(PROJECT_ROOT, 'core', 'assets', relative)
                if os.path.exists(target_path):
                    return target_path

        # Maps /assets/... to core/assets/...
        if path_clean.startswith('/assets/'):
            relative = path_clean[len('/assets/'):]
            return os.path.join(PROJECT_ROOT, 'core', 'assets', relative)
            
        # Maps /core/assets/... to core/assets/...
        if path_clean.startswith('/core/assets/'):
            relative = path_clean[len('/core/assets/'):]
            return os.path.join(PROJECT_ROOT, 'core', 'assets', relative)
            
        # Maps skybox paths (often loaded relative to Yokohama2 or Nalovardo)
        if path_clean.startswith('/Yokohama2/') or path_clean.startswith('/Nalovardo/'):
            return os.path.join(PROJECT_ROOT, 'core', 'assets', path_clean.lstrip('/'))
            
        # Default maps to web_client/static/...
        relative = path_clean.lstrip('/')
        if not relative:
            relative = 'index.html'
        return os.path.join(PROJECT_ROOT, 'web_client', 'static', relative)

    def log_message(self, format, *args):
        # Suppress noisy HTTP requests logs
        pass

def run_http_server(port):
    """Runs the HTTP static file and asset server."""
    try:
        server = ThreadingHTTPServer(('0.0.0.0', port), VephorHTTPHandler)
        print(f"[HTTP] Web client interface available at http://localhost:{port}/")
        server.serve_forever()
    except Exception as e:
        print(f"[HTTP] Error starting server on port {port}: {e}")

async def read_exact(reader, n):
    """Helper to read exactly n bytes from StreamReader."""
    data = bytearray()
    while len(data) < n:
        packet = await reader.read(n - len(data))
        if not packet:
            return None
        data.extend(packet)
    return bytes(data)

async def handle_tcp_incoming_stream(reader, writer, conn_id, peer, direction):
    """Parses incoming JSONB TCP packets and broadcasts them to WebSockets."""
    global ws_clients, active_connections
    print(f"[TCP] Connection {conn_id} established ({direction}) with {peer}")
    
    # Broadcast current connection list to WebSockets
    await broadcast_connection_list()
    
    try:
        while True:
            # 1. Read total_length (8 bytes, uint64)
            total_length_buf = await read_exact(reader, 8)
            if not total_length_buf:
                break
            
            # C++ client casts 8-byte buffer to signed int in some deserializers.
            # Unpacking as Q (uint64) is robust and matches the sender representation.
            total_length = struct.unpack('<Q', total_length_buf)[0]
            
            # 2. Read payload_count (8 bytes, uint64)
            payload_count_buf = await read_exact(reader, 8)
            if not payload_count_buf:
                break
            payload_count = struct.unpack('<Q', payload_count_buf)[0]
            
            # 3. Read header size (8 bytes, uint64)
            header_size_buf = await read_exact(reader, 8)
            if not header_size_buf:
                break
            header_size = struct.unpack('<Q', header_size_buf)[0]
            
            # 4. Read header JSON string
            header_json_buf = await read_exact(reader, header_size)
            if not header_json_buf:
                break
            header_str = header_json_buf.decode('utf-8', errors='ignore')
            
            # Attempt to parse as JSON
            try:
                header = json.loads(header_str)
            except Exception as je:
                print(f"[TCP] Connection {conn_id} JSON parsing error on header: {je}")
                continue
                
            # Parse and cache metadata to support browser client late-joining
            data = header.get("data") if isinstance(header, dict) else None
            
            # Extract window_id (Window 1 has ID 0 which is omitted, so default to 0)
            window_id = 0
            if isinstance(header, dict) and "window_id" in header:
                window_id = header["window_id"]
            elif isinstance(data, dict) and "window_id" in data:
                window_id = data["window_id"]
            
            def cache_metadata(source_dict):
                if not isinstance(source_dict, dict):
                    return
                for key in ["window", "camera", "flags"]:
                    if key in source_dict and source_dict[key]:
                        connection_metadata.setdefault(conn_id, {}).setdefault(window_id, {})[key] = source_dict[key]
            
            cache_metadata(header)
            if data:
                cache_metadata(data)
                
            # Parse and cache active visual objects per window_id (handles C++'s incremental frames)
            deletes_to_clean = []
            if data and "objects" in data and isinstance(data["objects"], list):
                if conn_id not in connection_objects:
                    connection_objects[conn_id] = {}
                if window_id not in connection_objects[conn_id]:
                    connection_objects[conn_id][window_id] = {}
                for obj in data["objects"]:
                    obj_id = obj.get("id")
                    if obj_id is not None:
                        if obj.get("destroy"):
                            connection_objects[conn_id][window_id][obj_id] = obj
                            deletes_to_clean.append(obj_id)
                        else:
                            if obj_id in connection_objects[conn_id][window_id]:
                                connection_objects[conn_id][window_id][obj_id].update(obj)
                            else:
                                connection_objects[conn_id][window_id][obj_id] = obj
                
            # Merge cached metadata and ALL currently active cached objects for this window_id into this frame's payload
            metadata = connection_metadata.get(conn_id, {}).get(window_id, {})
            if metadata:
                if isinstance(data, dict):
                    for key, val in metadata.items():
                        if key not in data:
                            data[key] = val
                else:
                    for key, val in metadata.items():
                        if key not in header:
                            header[key] = val
                            
            # Always broadcast the full state list of active objects for this window to the browser
            active_objs = connection_objects.get(conn_id, {}).get(window_id, {})
            if active_objs:
                if isinstance(data, dict):
                    data["objects"] = list(active_objs.values())
                else:
                    header["objects"] = list(active_objs.values())
                
            # 5. Read payloads
            payloads_base64 = []
            for _ in range(payload_count):
                pay_size_buf = await read_exact(reader, 8)
                if not pay_size_buf:
                    break
                pay_size = struct.unpack('<Q', pay_size_buf)[0]
                
                pay_data = await read_exact(reader, pay_size)
                if pay_data is None:
                    break
                    
                # Encode as Base64 for the Web client
                payloads_base64.append(base64.b64encode(pay_data).decode('utf-8'))
                
            if len(payloads_base64) < payload_count:
                # Truncated or closed stream
                break
                
            # If C++ sent new binary payloads, cache them per window_id
            if payloads_base64:
                connection_payloads.setdefault(conn_id, {})[window_id] = payloads_base64
            else:
                # If C++ sent no payloads, merge the cached ones so the browser has access to the vertex buffers
                payloads_base64 = connection_payloads.get(conn_id, {}).get(window_id, [])
                
            # Broadcast to WebSockets
            ws_msg = json.dumps({
                "type": "scene",
                "conn_id": conn_id,
                "header": header,
                "payloads": payloads_base64
            })
            
            # Post-serialization cleanup: purge explicitly deleted objects from the cache
            for obj_id in deletes_to_clean:
                connection_objects[conn_id].get(window_id, {}).pop(obj_id, None)
            
            # Fast broadcast
            if ws_clients:
                active_ws = list(ws_clients)
                await asyncio.gather(*[client.send(ws_msg) for client in active_ws], return_exceptions=True)
                
    except asyncio.CancelledError:
        pass
    except Exception as e:
        print(f"[TCP] Connection {conn_id} error: {e}")
    finally:
        print(f"[TCP] Connection {conn_id} ({peer}) disconnected.")
        with conn_lock:
            if conn_id in active_connections:
                del active_connections[conn_id]
            if conn_id in connection_metadata:
                del connection_metadata[conn_id]
            if conn_id in connection_objects:
                del connection_objects[conn_id]
            if conn_id in connection_payloads:
                del connection_payloads[conn_id]
        try:
            writer.close()
            await writer.wait_closed()
        except:
            pass
        await broadcast_connection_list()

def serialize_jsonb_message(header, payloads_bytes):
    """Serializes header dict and payload list into the exact JSONB format."""
    header_str = json.dumps(header)
    header_bytes = header_str.encode('utf-8')
    header_len = len(header_bytes)
    
    # total_length = header_str.size() + sizeof(uint64_t) * (1 + payloads.size()) + sum(payload.size())
    total_length = header_len + 8 * (1 + len(payloads_bytes))
    for p in payloads_bytes:
        total_length += len(p)
        
    out = bytearray()
    out.extend(struct.pack('<Q', total_length))
    out.extend(struct.pack('<Q', len(payloads_bytes)))
    out.extend(struct.pack('<Q', header_len))
    out.extend(header_bytes)
    
    for p in payloads_bytes:
        out.extend(struct.pack('<Q', len(p)))
        out.extend(p)
        
    return bytes(out)

async def write_to_tcp_socket(conn_id, header, payloads_base64):
    """Writes an interactive message back to the active TCP visualizer socket."""
    global active_connections
    with conn_lock:
        conn = active_connections.get(conn_id)
        
    if not conn:
        return
        
    try:
        # Re-convert payloads from base64 strings to bytes
        payloads_bytes = [base64.b64decode(p) for p in payloads_base64]
        
        # Serialize message
        packet = serialize_jsonb_message(header, payloads_bytes)
        
        # Write to TCP StreamWriter
        conn["writer"].write(packet)
        await conn["writer"].drain()
    except Exception as e:
        print(f"[TCP] Error writing to connection {conn_id}: {e}")

async def handle_websocket_message(ws, msg_str):
    """Processes messages received from WebSocket clients (browser UI)."""
    global active_connections, conn_counter
    try:
        msg = json.loads(msg_str)
        msg_type = msg.get("type")
        
        if msg_type == "get_connections":
            # Client requested current connection list
            await ws.send(json.dumps({
                "type": "connection_list",
                "connections": get_serialized_connection_list()
            }))
            
        elif msg_type == "connect_target":
            # Browser requested connecting as client to a remote Vephor server
            host = msg.get("host", "localhost")
            port = int(msg.get("port", 5533))
            
            print(f"[TCP] Connecting to Vephor server at {host}:{port}...")
            try:
                reader, writer = await asyncio.open_connection(host, port)
                
                with conn_lock:
                    new_id = conn_counter
                    conn_counter += 1
                    active_connections[new_id] = {
                        "reader": reader,
                        "writer": writer,
                        "peer": f"{host}:{port}",
                        "direction": "outbound"
                    }
                
                # Start reading from it
                asyncio.create_task(handle_tcp_incoming_stream(reader, writer, new_id, f"{host}:{port}", "outbound"))
            except Exception as e:
                print(f"[TCP] Connection failed to {host}:{port}: {e}")
                await ws.send(json.dumps({
                    "type": "notification",
                    "level": "error",
                    "message": f"Connection failed to {host}:{port}: {e}"
                }))
                
        elif msg_type == "disconnect_target":
            # Browser requested disconnecting a specific TCP peer
            cid = int(msg.get("conn_id"))
            with conn_lock:
                conn = active_connections.get(cid)
            if conn:
                try:
                    conn["writer"].close()
                except:
                    pass
                    
        elif msg_type == "event":
            # Forward user interaction event to TCP visualizer
            cid = int(msg.get("conn_id"))
            header = msg.get("header")
            payloads = msg.get("payloads", [])
            await write_to_tcp_socket(cid, header, payloads)
            
    except Exception as e:
        print(f"[WS] Error processing message: {e}")

async def ws_handler(ws):
    """Manages raw WebSocket connection lifecycle."""
    global ws_clients
    ws_clients.add(ws)
    print(f"[WS] Browser client connected (active: {len(ws_clients)})")
    
    # Send current connection status list on entry
    try:
        await ws.send(json.dumps({
            "type": "connection_list",
            "connections": get_serialized_connection_list()
        }))
        
        async for message in ws:
            await handle_websocket_message(ws, message)
            
    except Exception as e:
        pass
    finally:
        ws_clients.remove(ws)
        print(f"[WS] Browser client disconnected (active: {len(ws_clients)})")

def get_serialized_connection_list():
    """Returns a list of connected TCP peers ready for JSON serialization."""
    global active_connections
    with conn_lock:
        return [
            {
                "id": cid,
                "peer": conn["peer"],
                "direction": conn["direction"]
            }
            for cid, conn in active_connections.items()
        ]

async def broadcast_connection_list():
    """Broadcasts the current active connection list to all connected browsers."""
    global ws_clients
    if not ws_clients:
        return
    msg = json.dumps({
        "type": "connection_list",
        "connections": get_serialized_connection_list()
    })
    active_ws = list(ws_clients)
    await asyncio.gather(*[client.send(msg) for client in active_ws], return_exceptions=True)

async def start_tcp_listener(port):
    """Listens for inbound TCP connections from Vephor clients/visualizers."""
    global conn_counter, active_connections
    
    async def client_connected_cb(reader, writer):
        global conn_counter, active_connections
        peer_addr = writer.get_extra_info('peername')
        peer = f"{peer_addr[0]}:{peer_addr[1]}" if peer_addr else "Unknown"
        
        with conn_lock:
            cid = conn_counter
            conn_counter += 1
            active_connections[cid] = {
                "reader": reader,
                "writer": writer,
                "peer": peer,
                "direction": "inbound"
            }
            
        await handle_tcp_incoming_stream(reader, writer, cid, peer, "inbound")
        
    try:
        server = await asyncio.start_server(client_connected_cb, '0.0.0.0', port)
        print(f"[TCP] Server listening on port {port} for Vephor clients...")
        async with server:
            await server.serve_forever()
    except Exception as e:
        print(f"[TCP] Error starting TCP listener on port {port}: {e}")

async def main_async(ws_port, tcp_port):
    """Orchestrates async servers."""
    import websockets
    
    # Start WebSocket server
    try:
        ws_server = await websockets.serve(ws_handler, '0.0.0.0', ws_port)
        print(f"[WS] WebSocket server listening on port {ws_port}...")
    except Exception as e:
        print(f"[WS] Error starting WebSocket server on port {ws_port}: {e}")
        return

    # Start TCP server
    tcp_task = asyncio.create_task(start_tcp_listener(tcp_port))
    
    # Wait both
    await asyncio.gather(
        ws_server.wait_closed(),
        tcp_task
    )

def main():
    parser = argparse.ArgumentParser(description="Vephor Web Gateway")
    parser.add_argument('--http-port', type=int, default=8080, help="Web UI HTTP server port")
    parser.add_argument('--ws-port', type=int, default=5534, help="Internal WebSocket server port")
    parser.add_argument('--tcp-port', type=int, default=5533, help="Vephor TCP server listening port")
    args = parser.parse_args()
    
    # 1. Start HTTP Server in a background thread
    http_thread = threading.Thread(target=run_http_server, args=(args.http_port,), daemon=True)
    http_thread.start()
    
    # 2. Start Async Loop for WebSockets and TCP Server
    try:
        asyncio.run(main_async(args.ws_port, args.tcp_port))
    except KeyboardInterrupt:
        print("\nGateway shutting down cleanly. Goodbye!")

if __name__ == '__main__':
    main()
