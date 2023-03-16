#pragma once

#if defined(_WIN32)
#include <winsock2.h>
#include <ws2tcpip.h>
#else
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <signal.h> 
#endif

#include <thread>
#include <mutex>
#include <filesystem>

namespace fs = std::filesystem;

namespace vephor
{

const int VEPHOR_DEFAULT_PORT = 5533;
const int VEPHOR_MIN_RANDOM_PORT = 49152;
const int VEPHOR_MAX_RANDOM_PORT = 65535;

inline void error(const string& msg)
{
	perror(msg.c_str());
	//exit(1);
}

inline string buf_to_string(const vector<char>& buf)
{
	string str;
	for (size_t i = 0; i < buf.size(); i++)
	{
		str += buf[i];
	}
	return str;
}

struct JSONBMessage
{
	bool valid = false;
	json header;
	vector<vector<char>> payloads;
};

inline void writeJSONBMessageToFile(const json& header, const vector<vector<char>>& payloads, const string& msg_dir)
{
	fs::create_directories(msg_dir);

	for (size_t i = 0; i < payloads.size(); i++)
	{
		const auto& buf = payloads[i];
		ofstream fout(msg_dir+"/msg_buf_"+to_string(i), std::ios::binary);
		fout.write(buf.data(), buf.size());
		fout.close();
	}

	ofstream fout(msg_dir+"/msg.json");
	fout << header;
	fout.close();
}
	
class TCPSocket
{
public:
#if defined(_WIN32)
	void init()
	{
		WSADATA wsaData;
		
		int iResult;

		// Initialize Winsock
		v4print "Initializing winsock...";
		iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
		if (iResult != 0) {
			error("WSAStartup failed", iResult);
			throw std::runtime_error("WSAStartup failed");
		}
		needs_init = false;
	}
	bool isConnected()
	{
		return sock != INVALID_SOCKET;
	}
private:
	void enableTimeout()
	{
		struct timeval timeout;      
		timeout.tv_sec = 10;
		timeout.tv_usec = 0;
		
		if (setsockopt (sock, SOL_SOCKET, SO_RCVTIMEO, reinterpret_cast<char*>(&timeout),
					sizeof timeout) < 0)
		{
			throw std::runtime_error("setsockopt failed");
		}

		if (setsockopt (sock, SOL_SOCKET, SO_SNDTIMEO, reinterpret_cast<char*>(&timeout),
					sizeof timeout) < 0)
		{
			throw std::runtime_error("setsockopt failed");
		}
	}
public:
	void connect(const string& host, int port)
	{
		if (needs_init)
			init();
		
		int iResult;
		struct addrinfo *result = NULL, *ptr = NULL, hints;
		
		ZeroMemory( &hints, sizeof(hints) );
		hints.ai_family = AF_UNSPEC;
		hints.ai_socktype = SOCK_STREAM;
		hints.ai_protocol = IPPROTO_TCP;
		
		// Resolve the server address and port
		iResult = getaddrinfo(host.c_str(), std::to_string(port).c_str(), &hints, &result);
		if (iResult != 0) {
			error("getaddrinfo failed", iResult);
			throw std::runtime_error("getaddrinfo failed.");
		}
		
		// Attempt to connect to the first address returned by
		// the call to getaddrinfo
		//ptr=result;

		v4print "Creating socket for connection.";
		for (ptr = result; ptr != NULL; ptr = ptr->ai_next)
		{
			// Create a SOCKET for connecting to server
			sock = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
				
			if (sock == INVALID_SOCKET) {
				freeaddrinfo(result);
				error("socket error", WSAGetLastError());
				throw std::runtime_error("Socket creation error.");
			}
			
			// Connect to server.
			iResult = ::connect( sock, ptr->ai_addr, (int)ptr->ai_addrlen);

			if (iResult == SOCKET_ERROR) {
				closesocket(sock);
				sock = INVALID_SOCKET;
				continue;
			}
			break;
		}
		
		if (sock == INVALID_SOCKET)
		{
			error("could not find address to connect to.", 0);
			throw std::runtime_error("Could not find address to connect to.");
		}
		
		freeaddrinfo(result);
		
		//enableTimeout();
	}
	void bind_and_listen(int port)
	{
		if (needs_init)
			init();
		
		int iResult;
		struct addrinfo *result = NULL, hints;

		if (sock == INVALID_SOCKET)
		{
			ZeroMemory(&hints, sizeof (hints));
			hints.ai_family = AF_INET;
			hints.ai_socktype = SOCK_STREAM;
			hints.ai_protocol = IPPROTO_TCP;
			hints.ai_flags = AI_PASSIVE;

			// Resolve the local address and port to be used by the server
			iResult = getaddrinfo(NULL, std::to_string(port).c_str(), &hints, &result);
			if (iResult != 0) {
				error("getaddrinfo failed", iResult);
				throw std::runtime_error("getaddrinfo failed.");
			}
			
			// Create a SOCKET for the server to listen for client connections

			sock = socket(result->ai_family, result->ai_socktype, result->ai_protocol);	
			
			if (sock == INVALID_SOCKET) {
				freeaddrinfo(result);
				error("socket error", WSAGetLastError());
				throw std::runtime_error("Socket creation failed.");
			}
			
			// Setup the TCP listening socket
			iResult = ::bind( sock, result->ai_addr, (int)result->ai_addrlen);
			if (iResult == SOCKET_ERROR) {
				freeaddrinfo(result);
				error("bind failed", WSAGetLastError());
				throw std::runtime_error("Bind failed.");
			}
			
			freeaddrinfo(result);
			
			listen_socket = true;
			
			v4print "Listening on socket...";
			if ( ::listen( sock, SOMAXCONN ) == SOCKET_ERROR ) {
				error("listen failed", WSAGetLastError());
				throw std::runtime_error("Listen failed.");
			}
		}
	}
	shared_ptr<TCPSocket> accept(int port)
	{
		int iResult;

		bind_and_listen(port);
		
		SOCKET client_sock;
		
		client_sock = INVALID_SOCKET;

		// Accept a client socket
		{
			fd_set rfds;
			FD_ZERO(&rfds);
			FD_SET(sock, &rfds);
			
			timeval timeout;
			timeout.tv_sec = 1;
			timeout.tv_usec = 0;
			
			iResult = select(0, &rfds, NULL, NULL, &timeout);
			if (iResult < 0) {
				error("select failed", WSAGetLastError());
			}
			
			if (FD_ISSET(sock, &rfds))
			{
				v4print "Attempt client connection accept";
				client_sock = ::accept(sock, NULL, NULL);
			}
			else
			{
				return NULL;
			}
		}
		
		v4print "Accepted client socket.";
		
		auto new_sock = make_shared<TCPSocket>();
		new_sock->sock = client_sock;
		//new_sock->enableTimeout();
		return new_sock;
	}
	void send(const char* buf, size_t buf_size)
	{
		if (!isConnected())
			throw std::runtime_error("Can't send from unconnected socket.");
		
		int iResult;
		
		if (listen_socket)
			error("Can't send from listening socket.", 0);

		const int send_size = 1024; // TODO: use getsockopt to find max message size
		while (buf_size > 0)
		{
			int curr_send_size = buf_size;
			if (curr_send_size > send_size)
				curr_send_size = send_size;
			
			iResult = ::send(sock, buf, curr_send_size, 0);
			if (iResult == SOCKET_ERROR) {
				error("send failed", iResult);
				disconnect();
				break;
			}

			buf += curr_send_size;
			buf_size -= curr_send_size;
		}
	}
	void error(const string& message, int error_code)
	{
		printf("%s: %d\n", message.c_str(), error_code);
	}
	vector<char> receive(size_t buf_size = 1024)
	{
		if (!isConnected())
			throw std::runtime_error("Can't receive from unconnected socket.");
		
		int iResult;
		
		vector<char> buf(buf_size);

		if (listen_socket)
			error("Can't receive from listening socket.", 0);
		
		{
			fd_set rfds;
			FD_ZERO(&rfds);
			FD_SET(sock, &rfds);
			
			timeval timeout;
			timeout.tv_sec = 1;
			timeout.tv_usec = 0;
			
			iResult = select(0, &rfds, NULL, NULL, &timeout);
			if (iResult < 0) {
				error("select failed", WSAGetLastError());
			}
			
			if (FD_ISSET(sock, &rfds))
			{
				iResult = recv(sock, buf.data(), buf_size, 0);
			}
			else
			{
				return {};
			}
		}
		
		if (iResult < 0)
		{
			error("recv failed", WSAGetLastError());
			disconnect();
		}
		
		buf.resize(iResult);
		
		return buf;
	}
	void disconnect()
	{
		if (sock == INVALID_SOCKET)
			return;
		
		int iResult;
		
		// shutdown the send half of the connection since no more data will be sent
		iResult = shutdown(sock, SD_SEND);
		if (iResult == SOCKET_ERROR && WSAGetLastError() != WSANOTINITIALISED && WSAGetLastError() != WSAENOTCONN) {
			error("shutdown failed", WSAGetLastError());
		}
		
		// cleanup
		closesocket(sock);
		sock = INVALID_SOCKET;
	}
#else
	bool isConnected()
	{
		return sock_fd >= 0;
	}
private:
	void enableTimeout()
	{
		struct timeval timeout;      
		timeout.tv_sec = 10;
		timeout.tv_usec = 0;
		
		if (setsockopt (sock_fd, SOL_SOCKET, SO_RCVTIMEO, &timeout,
					sizeof timeout) < 0)
		{
			error("setsockopt failed");
			throw std::runtime_error("setsockopt failed");
		}

		if (setsockopt (sock_fd, SOL_SOCKET, SO_SNDTIMEO, &timeout,
					sizeof timeout) < 0)
		{
			error("setsockopt failed");
			throw std::runtime_error("setsockopt failed");
		}
	}
public:
	void connect(const string& host, int port)
	{
		signal(SIGPIPE, SIG_IGN);

		struct sockaddr_in serv_addr;
		struct hostent *server;

		sock_fd = socket(AF_INET, SOCK_STREAM, 0);
		if (sock_fd < 0)
		{			
			error("Error opening socket");
			throw std::runtime_error("Error opening socket.");
		}
		server = gethostbyname(host.c_str());
		if (server == NULL) {
			error("Error, no such host");
			throw std::runtime_error("Error, no such host.");
		}
		bzero((char *) &serv_addr, sizeof(serv_addr));
		serv_addr.sin_family = AF_INET;
		bcopy((char *)server->h_addr, 
			(char *)&serv_addr.sin_addr.s_addr,
			server->h_length);
		serv_addr.sin_port = htons(port);
		if (::connect(sock_fd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) 
		{
			error("Error connecting");
			throw std::runtime_error("Error connecting.");
		}

		//enableTimeout();
	}
	void bind_and_listen(int port)
	{
		signal(SIGPIPE, SIG_IGN);

		if (sock_fd < 0)
		{
			socklen_t clilen;
			struct sockaddr_in serv_addr;
			int n;
			sock_fd = socket(AF_INET, SOCK_STREAM, 0);
			if (sock_fd < 0)
			{
				error("Error opening socket");
				throw std::runtime_error("Error opening socket.");
			}
			bzero((char *) &serv_addr, sizeof(serv_addr));
			serv_addr.sin_family = AF_INET;
			serv_addr.sin_addr.s_addr = INADDR_ANY;
			serv_addr.sin_port = htons(port);
			if (::bind(sock_fd, (struct sockaddr *) &serv_addr,
				sizeof(serv_addr)) < 0)
			{				
				error("Error on binding");
				disconnect();
				throw std::runtime_error("Error on binding.");
			}
			listen_socket = true;
			::listen(sock_fd,5);
		}
	}
	shared_ptr<TCPSocket> accept(int port)
	{
		bind_and_listen(port);
		
		
		fd_set fds;
		int n;
		struct timeval tv;

		// Set up the file descriptor set.
		FD_ZERO(&fds);
		FD_SET(sock_fd, &fds);

		// Set up the struct timeval for the timeout.
		tv.tv_sec = 0;
		tv.tv_usec = 100000;

		// Wait until timeout or data received.
		int result = select(sock_fd+1, &fds, NULL, NULL, &tv);
		if (result == 0)
		{
			return NULL;
		}
		
		socklen_t clilen;
		struct sockaddr_in cli_addr;
		clilen = sizeof(cli_addr);
		int new_sock_fd = ::accept(sock_fd, 
			(struct sockaddr *) &cli_addr, 
			&clilen);
		if (new_sock_fd < 0) 
		{
			error("Error on accept");
			throw std::runtime_error("Error on accept.");
		}
		
		auto new_sock = make_shared<TCPSocket>();
		new_sock->sock_fd = new_sock_fd;
		//new_sock->enableTimeout();
		return new_sock;
	}
	void send(const char* buf, size_t buf_size)
	{
		if (!isConnected())
			throw std::runtime_error("Can't send from unconnected socket.");

		if (listen_socket)
		{
			throw std::runtime_error("Can't send from listening socket.");
		}
		
		while (buf_size > 0)
		{
			int n = ::send(sock_fd,buf,buf_size,0);
			if (n < 0) 
			{
				error("Error writing to socket");
				disconnect();
				break;
			}

			buf += n;
			buf_size -= n;
		}
	}
	vector<char> receive(size_t buf_size = 1024)
	{
		if (!isConnected())
			throw std::runtime_error("Can't receive from unconnected socket.");

		vector<char> buf(buf_size);

		if (listen_socket)
			throw std::runtime_error("Can't receive from listening socket.");
		
		fd_set fds;
		struct timeval tv;

		// Set up the file descriptor set.
		FD_ZERO(&fds);
		FD_SET(sock_fd, &fds);

		// Set up the struct timeval for the timeout.
		tv.tv_sec = 0;
		tv.tv_usec = 100000;

		// Wait until timeout or data received.
		int result = select(sock_fd+1, &fds, NULL, NULL, &tv);
		if (result == 0)
		{
			return {};
		}
		
		int n = ::recv(sock_fd,buf.data(),buf.size(),0);
		
		if (n < 0)
		{
			int errsv = errno;
			if (errsv == EAGAIN)
				return vector<char>();
			error("Error reading from socket");
			disconnect();
		}

		buf.resize(n);
		return buf;
	}
	void disconnect()
	{
		close(sock_fd);
		sock_fd = -1;
	}
#endif
	~TCPSocket()
	{
		disconnect();
	}
	void send(const string& str)
	{
		send(str.data(), str.size());
	}
	void send(const vector<char>& buf)
	{
		send(buf.data(), buf.size());
	}
	void sendMessage(const char* buf, uint64_t buf_size)
	{
		send(reinterpret_cast<char*>(&buf_size), sizeof(uint64_t));
		send(buf, buf_size);
	}
	void sendMessage(const string& str)
	{
		sendMessage(str.data(), str.size());
	}
	void sendMessage(const vector<char>& buf)
	{
		sendMessage(buf.data(), buf.size());
	}
	vector<char> receiveMessage()
	{
		uint64_t size;
        auto size_buf = receive(sizeof(uint64_t));
		
		if (size_buf.empty())
		{
			return vector<char>();
		}
		
        size = *reinterpret_cast<int*>(size_buf.data());

		vector<char> buf;

		while(size > 0 && isConnected())
		{
			vector<char> new_buf = receive(size);

			size -= new_buf.size();

			extend(buf, new_buf);

			if (size > 0)
			{
				std::this_thread::sleep_for(std::chrono::milliseconds(1));
			}
		}

		return buf;
	}
	
	//JSON B
	void sendJSONBMessage(const json& header, const vector<vector<char>>& payloads)
	{
		if (!isConnected())
			return;
		
		string header_str = header.dump();
		
		uint64_t total_length = header_str.size() + sizeof(uint64_t) * (1 + payloads.size());
		for (const auto& payload : payloads)
		{
			total_length += payload.size();
		}
		uint64_t payload_count = payloads.size();
		
		send(reinterpret_cast<char*>(&total_length), sizeof(uint64_t));
		send(reinterpret_cast<char*>(&payload_count), sizeof(uint64_t));
		sendMessage(header_str.data(), header_str.size());
		for (const auto& payload : payloads)
		{
			sendMessage(payload.data(), payload.size());
		}
	}
	JSONBMessage receiveJSONBMessage()
	{
		if (!isConnected())
			return JSONBMessage();
		
		uint64_t size, payload_count;
        
		auto size_buf = receive(sizeof(uint64_t));
		if (size_buf.empty()){return JSONBMessage();}
        size = *reinterpret_cast<int*>(size_buf.data());
		
		auto payload_count_buf = receive(sizeof(uint64_t));
		if (payload_count_buf.empty()){return JSONBMessage();}
        payload_count = *reinterpret_cast<int*>(payload_count_buf.data());
		
		uint64_t received_size = sizeof(uint64_t) * (1 + payload_count);
		
		JSONBMessage msg;

		vector<char> buf = receiveMessage();
		received_size += buf.size();
		msg.header = json::parse(buf_to_string(buf));

		for (uint64_t i = 0; i < payload_count; i++)
		{
			msg.payloads.push_back(receiveMessage());
			received_size += msg.payloads.rbegin()->size();
		}
		
		if (received_size != size)
		{
			throw std::runtime_error(string("Incorrect JSONB message size read: ") + std::to_string(received_size) + "/" + std::to_string(size));
		}
		
		msg.valid = true;
		return msg;
	}

private:
#if defined(_WIN32)
	SOCKET sock = INVALID_SOCKET;
	inline static bool needs_init = true;
#else
	int sock_fd = -1;
#endif
	bool listen_socket = false;
};
	

using ConnectionID = int;

class NetworkManager
{
public:
	~NetworkManager()
	{
		shutdown = true;
		v4print "NetworkManager: Shutting down.";
		if (waiting_for_connections)
			conn_wait_thread.join();
		for (auto& conn : conns)
		{
			conn.second->shutdown();
		}
		v4print "NetworkManager: Shutdown complete.";
	}
	void connectClient(bool wait_for_connection = true, const string& host = "localhost", int port = VEPHOR_DEFAULT_PORT)
	{
		if (client_mode)
			throw std::runtime_error("Client mode already active");
		if (server_mode)
			throw std::runtime_error("Can't use server and client mode at the same time.");
		
		if (wait_for_connection)
		{
			auto sock = make_shared<TCPSocket>();
			
			v4print "Waiting for client connection...";
			sock->connect(host, port);
			addConn(sock);
		}
		else
		{
			waiting_for_connections = true;
			conn_wait_thread = std::thread([&,port](){
				auto sock = make_shared<TCPSocket>();
				v4print "Waiting for client connection in background.";
				sock->connect(host, port);
				v4print "Client connection made.";
				addConn(sock);
				waiting_for_connections = false;
			});
		}

		client_mode = true;
	}
	bool bind_and_listen(int port = VEPHOR_DEFAULT_PORT)
	{
		if (!listen_sock)
		{
			listen_sock = make_shared<TCPSocket>();
			try {
				listen_sock->bind_and_listen(port);
			} catch (...) {
				listen_sock = NULL;
				return false;
			}
		}
		return true;
	}
	bool connectServer(bool wait_for_connection = true, int port = VEPHOR_DEFAULT_PORT)
	{
		if (server_mode)
			throw std::runtime_error("Server mode already active");
		if (client_mode)
			throw std::runtime_error("Can't use server and client mode at the same time.");
		
		if (!bind_and_listen(port))
			return false;

		if (wait_for_connection)
		{
			v4print "Waiting for server connection...";
			shared_ptr<TCPSocket> sock;
			while (true)
			{
				sock = listen_sock->accept(port);
				if (sock)
					break;
			}
			addConn(sock);
		}
		else
		{
			waiting_for_connections = true;
			conn_wait_thread = std::thread([&,port](){
				v4print "Waiting for server connection in background.";
				while (!shutdown)
				{
					shared_ptr<TCPSocket> sock;
					try {
						sock = listen_sock->accept(port);
					}
					catch (...) {
						std::this_thread::sleep_for(std::chrono::milliseconds(100));
						continue;
					}
					if (!sock)
					{
						std::this_thread::sleep_for(std::chrono::milliseconds(100));
						continue;
					}
					v4print "Server connection made.\nWaiting for server connection in background.";
					addConn(sock);
				}
				waiting_for_connections = false;
			});
		}

		server_mode = true;

		return true;
	}
	vector<int> getConnectionIdList() const
	{
		return conn_id_list;
	}
	void cleanConns()
	{
		unordered_map<ConnectionID, shared_ptr<ConnRecord>> active_conns;
		vector<ConnectionID> active_conn_id_list;
		
		for (auto& conn : conns)
		{
			if (conn.second->sock->isConnected())
			{
				active_conns[conn.first] = conn.second;
				active_conn_id_list.push_back(conn.first);
			}
			else
			{
				conn.second->shutdown();
			}
		}
		
		conns = active_conns;
		conn_id_list = active_conn_id_list;
	}
	bool sendJSONBMessage(ConnectionID conn_id, const json& header, const vector<vector<char>>& payloads)
	{
		if (!find(conns, conn_id))
			throw std::runtime_error("Attempt to send using invalid conn id: " + std::to_string(conn_id));
		try {
			conns[conn_id]->sock->sendJSONBMessage(header, payloads);
		} catch (...)
		{
			return false;
		}
		return true;
	}
	vector<JSONBMessage> getIncomingJSONBMessages(int conn_id)
	{
		if (!find(conns, conn_id))
			throw std::runtime_error("Attempt to get using invalid conn id: " + std::to_string(conn_id));
		auto msgs = conns[conn_id]->incoming_messages;
		conns[conn_id]->incoming_messages.clear();
		return msgs;
	}
	bool sendJSONBMessageToAll(const json& header, const vector<vector<char>>& payloads)
	{
		bool any_success = false;
		for (auto& conn : conns)
		{
			try {
				conn.second->sock->sendJSONBMessage(header, payloads);
			} catch (...)
			{
				continue;
			}
			any_success = true;
		}
		return any_success;
	}
	vector<JSONBMessage> getIncomingJSONBMessagesFromAll()
	{
		vector<JSONBMessage> messages;
		
		for (auto& conn : conns)
		{
			std::lock_guard<std::mutex> lock(conn.second->msg_lock);
			extend(messages, conn.second->incoming_messages);
			conn.second->incoming_messages.clear();
		}
		
		return messages;
	}
	float getConnTime(ConnectionID conn_id) const
	{
		if (!find(conns, conn_id))
			throw std::runtime_error("Attempt to get time using invalid conn id: " + std::to_string(conn_id));
		return conns.at(conn_id)->getTime();
	}
private:
	struct ConnRecord
	{
		shared_ptr<TCPSocket> sock;
		std::thread msg_thread;
		std::mutex msg_lock;
		vector<JSONBMessage> incoming_messages;
		std::chrono::time_point<std::chrono::high_resolution_clock> start_time;
		
		ConnRecord()
		{
			start_time = std::chrono::high_resolution_clock::now();
		}
		float getTime() const
		{
			auto end_time = std::chrono::high_resolution_clock::now();
			return std::chrono::duration<float, std::milli>(end_time-start_time).count() / 1000.0f;
		}
		void shutdown()
		{
			v4print "ConnRecord: Shutting down.";
			
			if (sock->isConnected())
			{
				json disconnect = {
					{"type", "disconnect"}
				};

				try {
					sock->sendJSONBMessage(disconnect, {});
				}
				catch (...) {}
			}


			sock->disconnect();
			msg_thread.join();
			
			v4print "ConnRecord: Shutdown complete.";
		}
	};

	void addConn(const shared_ptr<TCPSocket>& sock)
	{
		ConnectionID id = next_id;
		next_id++;
		
		auto conn = make_shared<ConnRecord>();
		conn->sock = sock;
		conn->msg_thread = std::thread([&,id,conn,sock](){
			v4print "Starting msg thread:", shutdown, sock->isConnected();
			while(!shutdown && sock->isConnected())
			{
				JSONBMessage msg;
				try {
					msg = sock->receiveJSONBMessage();
				} 
				catch (...) {
					v4print "Receive JSONB exception caught.";
					break;
				}
				if (msg.valid)
				{
					if (msg.header.is_array())
						throw std::runtime_error("Array top level JSON values not supported for JSONB messages.");
					
					if (msg.header.contains("type") && msg.header["type"] == "disconnect")
					{
						v4print "Disconnect received for", id;
						sock->disconnect();
						break;
					}

					msg.header["conn_id"] = id;
					
					std::lock_guard<std::mutex> lock(conn->msg_lock);
					conn->incoming_messages.push_back(msg);
				}
				else
					std::this_thread::sleep_for(std::chrono::milliseconds(10));
			}
			v4print "Ending connection thread for", id;
		});
		conns[id] = conn;
		conn_id_list.push_back(id);
	}

	bool shutdown = false;
	ConnectionID next_id = 1;
	shared_ptr<TCPSocket> listen_sock;
	unordered_map<ConnectionID, shared_ptr<ConnRecord>> conns;
	vector<ConnectionID> conn_id_list;
	bool client_mode = false;
	bool server_mode = false;
	bool waiting_for_connections = false;
	std::thread conn_wait_thread;
};
	
}