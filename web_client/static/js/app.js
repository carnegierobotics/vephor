/**
 * Vephor Web Client
 * Copyright 2026 Carnegie Robotics, LLC / Steve Landers
 * Complete multi-panel, interactive Three.js-based rendering engine.
 * Supports dynamic grid viewport splits for connections containing multiple C++ Windows
 * (e.g. test_show_two_windows), providing independent scenes, cameras, and controls.
 * Implements pure orthographic flat 2D plotting viewports for 2D plot feeds (e.g. test_plot).
 * Features sharp HTML5 canvas overlays for coordinate grids and matplotlib-style dynamic numbered ticks.
 * Supports left-click drag to pan, scroll to zoom, and right-click drag for non-uniform X/Y axis scaling.
 * Supports fluid HTML5 drag-and-drop to re-order panels and a draggable vertical splitter bar for custom split resizes.
 * Supports premium CAD/Maya style nested split layouts for 3 panels with dual vertical and horizontal draggable resizing splitters!
 * Driven by an exceptionally robust state-oriented panel order synchronization pipeline.
 */

// Application State
const state = {
    ws: null,
    
    // Scoped multi-window panel states:
    // keys: connId (int) -> windowId (int) -> panelObj
    panels: {},
    panelOrder: {}, // keys: connId (int) -> Array of windowIds (int) in active layout order
    
    // Track active connection telemetry
    activeConnId: null,
    objectCounters: {}, // Track total objects per connection
    fpsHistory: [],
    lastFrameTime: null,
    
    // Active UI flag controls
    activeFlags: {},
    
    // Global render options
    wireframe: false,
    gridEnabled: false,
    axesEnabled: false,
    
    // Draggable and Resizable state parameters
    draggedCard: null,
    isResizingSplit: false,
    isResizingSplitH: false,
    dragSplitIndex: 0,
    dragStartPos: 0,
    dragStartSizes: [],
    pixelsPerFr: 1,
    gridSizes: {}, // keys: connId -> { count, cols: [], rows: [] }
    
    // Recent connections history
    recentConnections: [],
    
    // List of current active connections from gateway
    activeConnections: []
};

// Initial setup on DOM ready
document.addEventListener('DOMContentLoaded', () => {
    loadRecentConnections();
    initUI();
    renderRecentConnections();
    connectWebSocket();
    animate(); // Starts global animation frame
});

// ==========================================
// 0. Recent Connections Manager
// ==========================================
function loadRecentConnections() {
    try {
        const saved = localStorage.getItem('vephor_recent_connections');
        if (saved) {
            state.recentConnections = JSON.parse(saved);
        }
    } catch (e) {
        console.warn("Failed to load recent connections from local storage:", e);
    }
}

function saveRecentConnections() {
    try {
        localStorage.setItem('vephor_recent_connections', JSON.stringify(state.recentConnections));
    } catch (e) {
        console.warn("Failed to save recent connections to local storage:", e);
    }
}

function addOrUpdateRecentConnection(peer, status) {
    // Remove if exists to bring to top
    state.recentConnections = state.recentConnections.filter(c => c.peer !== peer);
    
    // Add to top
    state.recentConnections.unshift({
        peer: peer,
        status: status,
        timestamp: Date.now()
    });
    
    // Limit to 5
    if (state.recentConnections.length > 5) {
        state.recentConnections = state.recentConnections.slice(0, 5);
    }
    
    saveRecentConnections();
    renderRecentConnections();
}

function renderRecentConnections() {
    const list = document.getElementById('recent-connections-list');
    if (!list) return;
    
    if (state.recentConnections.length === 0) {
        list.innerHTML = `<div class="empty-state">No recent connections.</div>`;
        return;
    }
    
    list.innerHTML = '';
    state.recentConnections.forEach(conn => {
        const item = document.createElement('div');
        item.className = 'conn-item';
        
        // Status indicator color
        let statusColor = 'var(--text-muted)';
        if (conn.status === 'success') statusColor = 'var(--success-color)';
        if (conn.status === 'failed') statusColor = 'var(--danger-color)';
        if (conn.status === 'attempting') statusColor = 'var(--accent-color)';
        
        item.innerHTML = `
            <div class="conn-info">
                <span class="conn-host">
                    <i class="fa-solid fa-circle" style="color: ${statusColor}; font-size: 8px; margin-right: 6px; vertical-align: middle;"></i>
                    ${conn.peer}
                </span>
                <span class="conn-details">Last: ${conn.status}</span>
            </div>
            <div class="conn-actions">
                <button class="btn btn-secondary btn-small retry-conn-btn" data-peer="${conn.peer}" title="Connect"><i class="fa-solid fa-play"></i></button>
            </div>
        `;
        
        list.appendChild(item);
    });
    
    document.querySelectorAll('.retry-conn-btn').forEach(btn => {
        btn.addEventListener('click', (e) => {
            const peer = e.currentTarget.getAttribute('data-peer');
            document.getElementById('target-host').value = peer;
            document.getElementById('connect-btn').click();
        });
    });
}

// ==========================================
// 1. Panel Manager & Grid Layout
// ==========================================
function createPanel(connId, windowId, title) {
    const container = document.getElementById('canvas-container');
    
    // Create card element
    const card = document.createElement('div');
    card.className = 'viewport-panel-card';
    card.id = `panel-${connId}-${windowId}`;
    
    // Create title tag
    const titleTag = document.createElement('div');
    titleTag.className = 'viewport-panel-title-tag';
    titleTag.textContent = title || `Show ${windowId}`;
    titleTag.setAttribute('title', 'Drag title to re-order panels');
    card.appendChild(titleTag);
    
    container.appendChild(card);
    
    // Create Scene (Transparent background so the DOM card background shines through!)
    const scene = new THREE.Scene();
    const bgColor = new THREE.Color(0x0d1117);
    card.style.backgroundColor = '#' + bgColor.getHexString();
    
    // Create HUD Scene
    const hudScene = new THREE.Scene();
    
    // Create Camera (initial perspective)
    const camera = new THREE.PerspectiveCamera(45, 1, 0.05, 1000000);
    camera.position.set(5, 5, 5);
    
    // Create Orthographic Camera for 2D plot mode (flat Matplotlib feel)
    const orthoCamera = new THREE.OrthographicCamera(-10, 10, 10, -10, 0.05, 10000);
    orthoCamera.position.set(0, 0, 1000);
    orthoCamera.lookAt(0, 0, 0);
    
    // Create HUD Camera
    const hudCamera = new THREE.OrthographicCamera(0, 1, 1, 0, -1000, 1000);
    
    // Create Background Canvas for dynamic 2D grid lines
    const gridCanvas = document.createElement('canvas');
    gridCanvas.className = 'plot-grid-canvas';
    gridCanvas.style.position = 'absolute';
    gridCanvas.style.top = '0';
    gridCanvas.style.left = '0';
    gridCanvas.style.width = '100%';
    gridCanvas.style.height = '100%';
    gridCanvas.style.pointerEvents = 'none'; // Pass clicks through to OrbitControls below
    gridCanvas.style.zIndex = '1'; // Behind WebGL
    card.appendChild(gridCanvas);
    
    // Create Renderer with Alpha enabled for transparency
    const renderer = new THREE.WebGLRenderer({ antialias: true, alpha: true });
    renderer.setPixelRatio(window.devicePixelRatio);
    renderer.shadowMap.enabled = true;
    renderer.autoClear = false;
    
    // Explicitly style WebGL canvas absolutely to guarantee correct compositing layer order OVER gridCanvas
    renderer.domElement.style.position = 'absolute';
    renderer.domElement.style.top = '0';
    renderer.domElement.style.left = '0';
    renderer.domElement.style.width = '100%';
    renderer.domElement.style.height = '100%';
    renderer.domElement.style.zIndex = '4'; // Over Grid, Under Ticks
    
    card.appendChild(renderer.domElement);
    
    // Create Foreground Canvas for tick marks and axis labels
    const tickCanvas = document.createElement('canvas');
    tickCanvas.className = 'plot-tick-canvas';
    tickCanvas.style.position = 'absolute';
    tickCanvas.style.top = '0';
    tickCanvas.style.left = '0';
    tickCanvas.style.width = '100%';
    tickCanvas.style.height = '100%';
    tickCanvas.style.pointerEvents = 'none'; // Pass clicks through to OrbitControls below
    tickCanvas.style.zIndex = '5'; // On top of WebGL
    card.appendChild(tickCanvas);
    
    // Create Controls (Perspective camera)
    const controls = new THREE.OrbitControls(camera, renderer.domElement);
    controls.enableDamping = true;
    controls.dampingFactor = 0.05;
    
    // Remap standard 3D mouse buttons so that LEFT click drags to PAN, and RIGHT click rotates!
    controls.mouseButtons = {
        LEFT: THREE.MOUSE.PAN,
        MIDDLE: THREE.MOUSE.DOLLY,
        RIGHT: null // Disabled so we can implement exact C++ TrackballCamera parity on right-click drag!
    };
    
    // Create Controls (Orthographic camera for flat 2D plotting)
    const orthoControls = new THREE.OrbitControls(orthoCamera, renderer.domElement);
    orthoControls.enableDamping = true;
    orthoControls.dampingFactor = 0.05;
    orthoControls.enableRotate = false; // Completely lock 3D rotation for a pure flat 2D canvas
    
    // Remap OrbitControls mouse buttons so LEFT click drags to PAN the plot!
    orthoControls.mouseButtons = {
        LEFT: THREE.MOUSE.PAN,
        MIDDLE: THREE.MOUSE.DOLLY,
        RIGHT: null // Disable OrbitControls right click so we can use it for custom stretching
    };
    
    // Lights for main scene
    scene.add(new THREE.AmbientLight(0xffffff, 0.4));
    const dirLight = new THREE.DirectionalLight(0xffffff, 0.6);
    dirLight.position.set(10, 15, 10);
    scene.add(dirLight);
    const fillLight = new THREE.DirectionalLight(0xaaccff, 0.25);
    fillLight.position.set(-10, -5, -10);
    scene.add(fillLight);
    
    // Lights for HUD scene
    hudScene.add(new THREE.AmbientLight(0xffffff, 1.0));
    
    // Grid & Axes Helpers
    const gridHelper = new THREE.GridHelper(50, 50, 0x58a6ff, 0x30363d);
    gridHelper.rotation.x = Math.PI / 2;
    scene.add(gridHelper);
    
    const axesHelper = new THREE.AxesHelper(5);
    scene.add(axesHelper);
    
    // Pre-create the 9 HUD anchor groups
    const anchors = [
        "window_top_left", "window_top", "window_top_right",
        "window_left", "window_center", "window_right",
        "window_bottom_left", "window_bottom", "window_bottom_right"
    ];
    const feedHUDGroup = new THREE.Group();
    anchors.forEach(name => {
        const anchorGroup = new THREE.Group();
        anchorGroup.name = name;
        feedHUDGroup.add(anchorGroup);
    });
    hudScene.add(feedHUDGroup);
    
    const feedGroup = new THREE.Group();
    scene.add(feedGroup);
    
    // Create the yellow orbit point indicator sphere
    const orbitGeom = new THREE.SphereGeometry(1, 16, 16);
    const orbitMat = new THREE.MeshBasicMaterial({ color: 0xffff00, wireframe: true, transparent: true, opacity: 0.5 });
    const orbitPointSphere = new THREE.Mesh(orbitGeom, orbitMat);
    orbitPointSphere.visible = false;
    scene.add(orbitPointSphere);
    
    // Create Legend Container (Hidden by default)
    const legendContainer = document.createElement('div');
    legendContainer.className = 'plot-legend-container';
    legendContainer.style.position = 'absolute';
    legendContainer.style.display = 'none';
    legendContainer.style.zIndex = '6'; // On top of ticks
    legendContainer.style.backgroundColor = 'rgba(255, 255, 255, 0.85)';
    legendContainer.style.border = '1px solid rgba(0,0,0,0.1)';
    legendContainer.style.borderRadius = '4px';
    legendContainer.style.padding = '8px 12px';
    legendContainer.style.fontFamily = '"JetBrains Mono", monospace';
    legendContainer.style.fontSize = '11px';
    legendContainer.style.color = '#333';
    legendContainer.style.pointerEvents = 'none'; // Don't block dragging
    card.appendChild(legendContainer);
    
    const panel = {
        connId,
        windowId,
        card,
        titleTag,
        bgColor,
        scene,
        hudScene,
        camera,
        orthoCamera,
        hudCamera,
        gridCanvas,
        tickCanvas,
        legendContainer,
        renderer,
        controls,
        orthoControls,
        gridHelper,
        axesHelper,
        feedGroup,
        feedHUDGroup,
        orbitPointSphere,
        feedObjects: {},
        objectCounters: {},
        wireframe: state.wireframe,
        gridEnabled: state.gridEnabled,
        axesEnabled: state.axesEnabled,
        is2DPlotMode: false,
        hasAutoFitted: false,
        orthoHeightUnits: 10,
        plotEqualAspect: false,
        frameQueue: Promise.resolve(),
        
        // Custom Right-Click Drag Scaling parameters
        isRightDragging: false,
        isLeftDragging: false,
        dragStartMouse: new THREE.Vector2(),
        dragStartFrustum: { left: 0, right: 0, top: 0, bottom: 0 }
    };
    
    // ==========================================
    // Interaction Handlers (Dragging & Scaling)
    // ==========================================
    
    // 1. Pointer down right-click hook (Capture phase to preempt OrbitControls)
    panel.card.addEventListener('pointerdown', (e) => {
        // DO NOT intercept pointerdown if the user is trying to click the title tag for dragging
        if (e.target.classList && e.target.classList.contains('viewport-panel-title-tag')) return;

        if (e.button === 2) { // Right Click
            e.preventDefault();
            e.stopPropagation();
            
            panel.isRightDragging = true;
            panel.dragStartMouse.set(e.clientX, e.clientY);
            
            if (panel.is2DPlotMode) {
                // Record initial frustum states
                panel.dragStartFrustum.left = panel.orthoCamera.left;
                panel.dragStartFrustum.right = panel.orthoCamera.right;
                panel.dragStartFrustum.top = panel.orthoCamera.top;
                panel.dragStartFrustum.bottom = panel.orthoCamera.bottom;
                
                // Disable controls damping temporarily during drag for instant responsiveness
                panel.orthoControls.enableDamping = false;
            } else {
                panel.dragStartCamPos = panel.camera.position.clone();
                panel.controls.enableDamping = false;
                panel.orbitPointSphere.visible = true; // Show yellow trackball anchor on rotate!
            }
            
            // Capture pointer globally for this card element (modern Web standard)
            panel.card.setPointerCapture(e.pointerId);
        } else if (e.button === 0) { // Left Click (Pan)
            if (!panel.is2DPlotMode) {
                panel.isLeftDragging = true;
                panel.orbitPointSphere.visible = true; // Show yellow trackball anchor on pan!
            }
        }
    }, true);
    
    // 2. Pointer move right-click drag scaling hook
    panel.card.addEventListener('pointermove', (e) => {
        if (!panel.isRightDragging) return;
        
        const deltaX = e.clientX - panel.dragStartMouse.x;
        const deltaY = e.clientY - panel.dragStartMouse.y;
        
        if (panel.is2DPlotMode) {
            // C++ style exponential stretching
            // Dragging right/down zooms in (shrinks bounds), left/up zooms out (grows bounds)
            let factorX = Math.exp(-deltaX / 150);
            let factorY = Math.exp(deltaY / 150); // Y screen coords are inverted
            
            if (panel.plotEqualAspect) {
                // If equal aspect ratio is enforced, apply uniform zooming
                const uniformFactor = factorX * factorY;
                factorX = uniformFactor;
                factorY = uniformFactor;
            }
            
            const f = panel.dragStartFrustum;
            const centerX = (f.left + f.right) / 2;
            const centerY = (f.bottom + f.top) / 2;
            
            const spanX = Math.abs(f.right - f.left) * factorX;
            const spanY = Math.abs(f.top - f.bottom) * factorY;
            
            panel.orthoWidthUnits = spanX / 2;
            panel.orthoHeightUnits = spanY / 2;
            
            panel.orthoCamera.left = centerX - panel.orthoWidthUnits;
            panel.orthoCamera.right = centerX + panel.orthoWidthUnits;
            panel.orthoCamera.top = centerY + panel.orthoHeightUnits;
            panel.orthoCamera.bottom = centerY - panel.orthoHeightUnits;

            panel.orthoCamera.scale.x = panel.xFlip ? -1 : 1;
            panel.orthoCamera.scale.y = panel.yFlip ? -1 : 1;
            
            panel.orthoCamera.updateProjectionMatrix();
            
            // Sync OrbitControls coordinates
            panel.orthoControls.update();
        } else {
            // 3D TrackballCamera Parity (Turntable Mode)
            // Mirrors the exact math of TrackballCamera::update from the C++ library
            const offset = new THREE.Vector3().subVectors(panel.dragStartCamPos, panel.controls.target);
            const offsetMag = offset.length();
            
            const up = panel.camera.up.clone().normalize();
            
            // Replicate C++ findCrossVec to establish a static rotation reference frame
            const fore = new THREE.Vector3(1, 0, 0);
            if (Math.abs(up.x) > 0.9) fore.set(0, 1, 0);
            fore.cross(up).normalize();
            
            // trackball_right = trackball_up x trackball_fore
            const right = new THREE.Vector3().crossVectors(up, fore).normalize();
            
            // Calculate absolute initial Euler angles relative to this static frame
            const rightDot = right.dot(offset);
            const foreDot = fore.dot(offset);
            
            let pitch = -Math.atan2(up.dot(offset), Math.sqrt(rightDot * rightDot + foreDot * foreDot));
            let yaw = -Math.atan2(rightDot, foreDot);
            
            // Apply mouse deltas precisely identical to C++ scaling constants
            yaw += deltaX / 100.0;
            pitch -= deltaY / 100.0;
            
            // Clamp pitch to strictly prevent gimbal flips over the poles
            const limit = Math.PI / 2 - 1e-3;
            if (pitch > limit) pitch = limit;
            else if (pitch < -limit) pitch = -limit;
            
            // Reconstruct offset: Orient3(-up * yaw) * Orient3(pitch * right) * (offsetMag * fore)
            const newOffset = fore.clone().multiplyScalar(offsetMag);
            newOffset.applyAxisAngle(right, pitch); // Pitch around static right vector
            newOffset.applyAxisAngle(up, -yaw);     // Yaw around static up vector (inverted angle per C++ formula)
            
            panel.camera.position.copy(panel.controls.target).add(newOffset);
            panel.camera.lookAt(panel.controls.target);
            panel.controls.update(); // Synchronize OrbitControls internal state seamlessly!
        }
    });
    
    // 3. Pointer up right-click release hook
    panel.card.addEventListener('pointerup', (e) => {
        if (e.button === 2 && panel.isRightDragging) {
            panel.isRightDragging = false;
            
            // Release pointer capture
            try {
                panel.card.releasePointerCapture(e.pointerId);
            } catch (err) {}
            
            // Restore smooth damping physics
            if (panel.is2DPlotMode) {
                panel.orthoControls.enableDamping = true;
            } else {
                panel.controls.enableDamping = true;
                panel.orbitPointSphere.visible = false;
            }
        } else if (e.button === 0 && panel.isLeftDragging) {
            panel.isLeftDragging = false;
            panel.orbitPointSphere.visible = false;
        }
    });
    
    // 4. Suppress context menu popup when right clicking on the card (Capture phase to block completely)
    panel.card.addEventListener('contextmenu', (e) => {
        // Block context menu for both 2D and 3D so right-click can be used natively
        e.preventDefault();
        e.stopPropagation();
    }, true);
    
    // ==========================================
    // HTML5 Drag-and-Drop Re-order Handlers
    // ==========================================
    titleTag.setAttribute('draggable', 'true');
    
    titleTag.addEventListener('dragstart', (e) => {
        e.dataTransfer.setData('text/plain', `${connId}-${windowId}`);
        card.style.opacity = '0.4';
        state.draggedCard = card;
        // Set drag image to the whole card if possible, otherwise it just drags the small title
        if (e.dataTransfer.setDragImage) {
            e.dataTransfer.setDragImage(card, 0, 0);
        }
    });
    
    card.addEventListener('dragend', () => {
        card.style.opacity = '1.0';
        state.draggedCard = null;
        document.querySelectorAll('.viewport-panel-card').forEach(c => c.classList.remove('drag-over'));
    });
    
    card.addEventListener('dragover', (e) => {
        e.preventDefault();
        if (state.draggedCard && state.draggedCard !== card) {
            card.classList.add('drag-over');
        }
    });
    
    card.addEventListener('dragleave', () => {
        card.classList.remove('drag-over');
    });
    
    card.addEventListener('drop', (e) => {
        e.preventDefault();
        card.classList.remove('drag-over');
        
        if (state.draggedCard && state.draggedCard !== card) {
            // Extract winIds from card element IDs (panel-[connId]-[winId])
            const winId1 = parseInt(state.draggedCard.id.split('-')[2]);
            const winId2 = parseInt(card.id.split('-')[2]);
            
            const order = state.panelOrder[connId];
            const idx1 = order.indexOf(winId1);
            const idx2 = order.indexOf(winId2);
            
            if (idx1 !== -1 && idx2 !== -1) {
                // Swap in our logical state-driven list
                order[idx1] = winId2;
                order[idx2] = winId1;
                
                // Trigger full state-driven DOM redraw
                updateCanvasGrid();
                
                showToast('Panels re-arranged successfully', 'success');
            }
        }
    });

    // Apply global defaults on creation
    gridHelper.visible = state.gridEnabled;
    axesHelper.visible = state.axesEnabled;
    
    return panel;
}

function resizePanel(panel) {
    const width = panel.card.clientWidth;
    const height = panel.card.clientHeight;
    if (width === 0 || height === 0) return;
    
    // Resize overlay canvases to match device pixels perfectly for ultra-sharp rendering
    const dpr = window.devicePixelRatio || 1;
    panel.gridCanvas.width = width * dpr;
    panel.gridCanvas.height = height * dpr;
    panel.gridCanvas.style.width = `${width}px`;
    panel.gridCanvas.style.height = `${height}px`;
    
    panel.tickCanvas.width = width * dpr;
    panel.tickCanvas.height = height * dpr;
    panel.tickCanvas.style.width = `${width}px`;
    panel.tickCanvas.style.height = `${height}px`;
    
    // Update perspective camera aspect
    panel.camera.aspect = width / height;
    panel.camera.updateProjectionMatrix();
    
    // Update orthographic camera boundaries
    if (panel.is2DPlotMode && !panel.plotEqualAspect && panel.orthoWidthUnits !== undefined) {
        // Non-equal: maintain absolute world coordinate limits, stretching content to fit the new DOM aspect ratio
        panel.orthoCamera.left = -panel.orthoWidthUnits;
        panel.orthoCamera.right = panel.orthoWidthUnits;
        panel.orthoCamera.top = panel.orthoHeightUnits;
        panel.orthoCamera.bottom = -panel.orthoHeightUnits;
    } else {
        // Equal (or 3D defaults): adjust horizontal bounds strictly by aspect ratio to enforce 1:1 scaling
        const aspect = width / height;
        const heightUnits = panel.orthoHeightUnits || 10;
        
        panel.orthoCamera.left = -heightUnits * aspect;
        panel.orthoCamera.right = heightUnits * aspect;
        panel.orthoCamera.top = heightUnits;
        panel.orthoCamera.bottom = -heightUnits;
    }

    panel.orthoCamera.scale.x = panel.xFlip ? -1 : 1;
    panel.orthoCamera.scale.y = panel.yFlip ? -1 : 1;
    panel.orthoCamera.updateProjectionMatrix();
    
    // Update HUD Camera
    panel.hudCamera.left = 0;
    panel.hudCamera.right = width;
    panel.hudCamera.top = height;
    panel.hudCamera.bottom = 0;
    panel.hudCamera.updateProjectionMatrix();
    
    updatePanelHUDAnchors(panel, width, height);
    
    // Update scatter points sizes on resize if in Plot Mode
    if (panel.is2DPlotMode) {
        panel.scene.traverse(child => {
            if (child.isPoints && child.material) {
                const oSize = child.userData.defSize || 0.03;
                child.material.size = height * oSize;
                child.material.needsUpdate = true;
            }
        });
    }
    
    panel.renderer.setSize(width, height);
}

function updatePanelHUDAnchors(panel, width, height) {
    const hudGroup = panel.feedHUDGroup;
    if (!hudGroup) return;
    
    const tl = hudGroup.getObjectByName("window_top_left");
    if (tl) tl.position.set(0, height, 0);
    
    const t = hudGroup.getObjectByName("window_top");
    if (t) t.position.set(width / 2, height, 0);
    
    const tr = hudGroup.getObjectByName("window_top_right");
    if (tr) tr.position.set(width, height, 0);
    
    const l = hudGroup.getObjectByName("window_left");
    if (l) l.position.set(0, height / 2, 0);
    
    const c = hudGroup.getObjectByName("window_center");
    if (c) c.position.set(width / 2, height / 2, 0);
    
    const r = hudGroup.getObjectByName("window_right");
    if (r) r.position.set(width, height / 2, 0);
    
    const bl = hudGroup.getObjectByName("window_bottom_left");
    if (bl) bl.position.set(0, 0, 0);
    
    const b = hudGroup.getObjectByName("window_bottom");
    if (b) b.position.set(width / 2, 0, 0);
    
    const br = hudGroup.getObjectByName("window_bottom_right");
    if (br) br.position.set(width, 0, 0);
}

function updateCanvasGrid() {
    const container = document.getElementById('canvas-container');
    const order = state.panelOrder[state.activeConnId] || [];
    
    // Build active panels list in their exact logical swapped order
    const activeFeedPanels = order
        .map(winId => state.panels[state.activeConnId] ? state.panels[state.activeConnId][winId] : null)
        .filter(p => p !== null && p !== undefined);
        
    const count = activeFeedPanels.length;
    
    // Clear container completely to rebuild DOM grid from scratch
    container.innerHTML = '';
    
    if (count === 0) {
        container.innerHTML = `<div class="empty-state-canvas"><i class="fa-solid fa-satellite-dish" style="font-size: 2rem; color: var(--text-muted); margin-bottom: 15px;"></i><br>Waiting for visualizer data stream...</div>`;
        return;
    }
    
    const cols = Math.ceil(Math.sqrt(count));
    const rows = Math.ceil(count / cols);
    
    if (!state.gridSizes) state.gridSizes = {};
    if (!state.gridSizes[state.activeConnId] || state.gridSizes[state.activeConnId].count !== count) {
        state.gridSizes[state.activeConnId] = {
            count: count,
            cols: new Array(cols).fill(1),
            rows: new Array(rows).fill(1)
        };
    }
    const sizes = state.gridSizes[state.activeConnId];
    
    // Setup pure CSS grid structure
    container.style.gridTemplateColumns = sizes.cols.map(c => `${c}fr`).join(' 6px ');
    container.style.gridTemplateRows = sizes.rows.map(r => `${r}fr`).join(' 6px ');
    
    // 1. Append all panel cards at precise grid coordinates
    activeFeedPanels.forEach((panel, i) => {
        const r = Math.floor(i / cols);
        const c = i % cols;
        
        panel.card.style.gridRow = `${r * 2 + 1}`;
        
        // If this is the last panel, make it span any remaining columns to avoid empty holes
        if (i === count - 1) {
            const remainingCols = cols - c;
            panel.card.style.gridColumn = `${c * 2 + 1} / span ${remainingCols * 2 - 1}`;
        } else {
            panel.card.style.gridColumn = `${c * 2 + 1}`;
        }
        
        panel.card.style.display = 'block';
        container.appendChild(panel.card);
        
        // Small timeout to allow DOM layout to calculate before WebGL resize
        setTimeout(() => resizePanel(panel), 10);
    });
    
    // 2. Inject draggable vertical splitters only where two panels actually
    // meet. In an incomplete final row, the last panel spans the empty cells;
    // a full-height splitter would otherwise overlay that spanning panel.
    const panelAtCell = (r, c) => {
        const index = r * cols + c;
        if (index < count) return index;

        const lastIndex = count - 1;
        const lastRow = Math.floor(lastIndex / cols);
        const lastCol = lastIndex % cols;
        return r === lastRow && c >= lastCol ? lastIndex : null;
    };

    for (let c = 0; c < cols - 1; c++) {
        for (let r = 0; r < rows; r++) {
            const leftPanel = panelAtCell(r, c);
            const rightPanel = panelAtCell(r, c + 1);
            if (leftPanel === null || rightPanel === null || leftPanel === rightPanel) continue;

            const splitter = document.createElement('div');
            splitter.className = 'grid-splitter';
            splitter.style.gridColumn = `${c * 2 + 2}`;
            splitter.style.gridRow = `${r * 2 + 1}`;

            splitter.addEventListener('mousedown', (e) => {
                e.preventDefault();
                state.isResizingSplit = true;
                state.dragSplitIndex = c;
                state.dragStartPos = e.clientX;
                state.dragStartSizes = [...sizes.cols];

                const totalFr = sizes.cols.reduce((a, b) => a + b, 0);
                const availablePixels = container.clientWidth - (cols - 1) * 6;
                state.pixelsPerFr = availablePixels / totalFr;

                splitter.classList.add('active-dragging');
                document.body.style.cursor = 'col-resize';
            });
            container.appendChild(splitter);
        }
    }
    
    // 3. Inject Draggable Horizontal Splitters
    for (let r = 0; r < rows - 1; r++) {
        const splitterH = document.createElement('div');
        splitterH.className = 'grid-splitter-h';
        splitterH.style.gridRow = `${r * 2 + 2}`;
        splitterH.style.gridColumn = '1 / -1';
        
        splitterH.addEventListener('mousedown', (e) => {
            e.preventDefault();
            state.isResizingSplitH = true;
            state.dragSplitIndex = r;
            state.dragStartPos = e.clientY;
            state.dragStartSizes = [...sizes.rows];
            
            const totalFr = sizes.rows.reduce((a, b) => a + b, 0);
            const availablePixels = container.clientHeight - (rows - 1) * 6;
            state.pixelsPerFr = availablePixels / totalFr;
            
            splitterH.classList.add('active-dragging');
            document.body.style.cursor = 'row-resize';
        });
        container.appendChild(splitterH);
    }
    
    // Hide the manual slider, as we now support infinite multi-panel drag sashes
    const splitRatioRow = document.getElementById('split-ratio-row');
    if (splitRatioRow) splitRatioRow.classList.add('hide');
    
    // Manage visibility of inactive panels
    Object.keys(state.panels).forEach(cid => {
        const c_id = parseInt(cid);
        if (c_id !== state.activeConnId) {
            Object.values(state.panels[c_id]).forEach(panel => {
                panel.card.style.display = 'none';
            });
        }
    });
}

function animate() {
    requestAnimationFrame(animate);
    
    // Update and render each active panel of the selected connection feed
    if (state.activeConnId && state.panels[state.activeConnId]) {
        const activePanels = Object.values(state.panels[state.activeConnId]);
        
        activePanels.forEach(panel => {
            // Update the correct camera controls based on plot mode
            if (panel.is2DPlotMode) {
                panel.orthoControls.update();
            } else {
                panel.controls.update();
            }
            
            // Choose correct active camera (Ortho flat projection vs Perspective 3D)
            const activeCam = panel.is2DPlotMode ? panel.orthoCamera : panel.camera;
            
            // Dynamic Trackball Orbit Point Sphere alignment and scaling (matching C++ orbit_point_scene_scale_mult = 1.0/100.0)
            if (!panel.is2DPlotMode && panel.orbitPointSphere.visible) {
                const target = panel.controls.target;
                panel.orbitPointSphere.position.copy(target);
                
                // Scale sphere dynamically based on distance to camera to maintain constant visual screen-space size
                const dist = panel.camera.position.distanceTo(target);
                const scale = dist * (1.0 / 100.0);
                panel.orbitPointSphere.scale.set(scale, scale, scale);
            }
            
            // 1. Render main scene pass
            panel.renderer.clear();
            panel.renderer.render(panel.scene, activeCam);
            
            // 2. Clear depth and overlay HUD elements
            panel.renderer.clearDepth();
            panel.renderer.render(panel.hudScene, panel.hudCamera);
            
            // 3. Draw Plot 2D grid and labeled ticks on the overlay canvas
            drawPlotOverlay(panel);
        });
    }
    
    // Telemetry tracking
    const now = performance.now();
    if (state.lastFrameTime) {
        const fps = 1000 / (now - state.lastFrameTime);
        state.fpsHistory.push(fps);
        if (state.fpsHistory.length > 30) state.fpsHistory.shift();
        
        // Update average FPS in Telemetry panel
        const avgFps = state.fpsHistory.reduce((a, b) => a + b, 0) / state.fpsHistory.length;
        document.getElementById('tel-fps').textContent = avgFps.toFixed(1);
    }
    state.lastFrameTime = now;
}

// ==========================================
// 2. Labeled Ticks & 2D Grid Vector Drawing
// ==========================================
function drawPlotOverlay(panel) {
    const gridCanvas = panel.gridCanvas;
    const gridCtx = gridCanvas.getContext('2d');
    const tickCanvas = panel.tickCanvas;
    const tickCtx = tickCanvas.getContext('2d');
    
    const width = panel.card.clientWidth;
    const height = panel.card.clientHeight;
    
    // Clear canvases
    gridCtx.clearRect(0, 0, gridCanvas.width, gridCanvas.height);
    tickCtx.clearRect(0, 0, tickCanvas.width, tickCanvas.height);
    
    if (!panel.is2DPlotMode) return;
    
    const renderCam = panel.orthoCamera;
    
    // Unprojects a pixel coordinate back to 3D world space (perspective-free unprojection for Ortho camera)
    const pixelToWorld = (sx, sy) => {
        const vec = new THREE.Vector3(
            (sx / width) * 2 - 1,
            -(sy / height) * 2 + 1,
            0 // NDC Z=0 sits exactly on the ortho viewing plane
        );
        vec.unproject(renderCam);
        return vec;
    };
    
    // Projects a 3D world coordinate back to 2D canvas pixel coordinate
    const worldToPixel = (wx, wy) => {
        const vec = new THREE.Vector3(wx, wy, 0);
        const proj = vec.project(renderCam);
        const x = (proj.x * 0.5 + 0.5) * width;
        const y = (1 - (proj.y * 0.5 + 0.5)) * height;
        return new THREE.Vector2(x, y);
    };
    
    const dpr = window.devicePixelRatio || 1;
    gridCtx.save();
    gridCtx.scale(dpr, dpr);
    tickCtx.save();
    tickCtx.scale(dpr, dpr);
    
    // Calculate world boundaries currently framed by the viewport
    const bottomLeft = pixelToWorld(0, height);
    const topRight = pixelToWorld(width, 0);
    
    const minX = Math.min(bottomLeft.x, topRight.x);
    const maxX = Math.max(bottomLeft.x, topRight.x);
    const minY = Math.min(bottomLeft.y, topRight.y);
    const maxY = Math.max(bottomLeft.y, topRight.y);
    
    const rangeX = maxX - minX;
    const rangeY = maxY - minY;
    if (rangeX <= 0 || rangeY <= 0) {
        gridCtx.restore();
        tickCtx.restore();
        return;
    }
    
    // Calculate adaptive grid spacing for X axis
    const logX = Math.log10(rangeX);
    const powerX = Math.floor(logX);
    const fractionX = rangeX / Math.pow(10, powerX);
    
    let spacingX;
    if (fractionX < 1.5) spacingX = 0.1 * Math.pow(10, powerX);
    else if (fractionX < 3) spacingX = 0.2 * Math.pow(10, powerX);
    else if (fractionX < 7) spacingX = 0.5 * Math.pow(10, powerX);
    else spacingX = 1.0 * Math.pow(10, powerX);
    
    // Calculate adaptive grid spacing for Y axis
    const logY = Math.log10(rangeY);
    const powerY = Math.floor(logY);
    const fractionY = rangeY / Math.pow(10, powerY);
    
    let spacingY;
    if (fractionY < 1.5) spacingY = 0.1 * Math.pow(10, powerY);
    else if (fractionY < 3) spacingY = 0.2 * Math.pow(10, powerY);
    else if (fractionY < 7) spacingY = 0.5 * Math.pow(10, powerY);
    else spacingY = 1.0 * Math.pow(10, powerY);
    
    const formatTickX = (val) => {
        if (Math.abs(val) < 1e-10) return "0";
        const precision = Math.max(0, -Math.floor(Math.log10(spacingX)) + 1);
        return parseFloat(val.toFixed(precision)).toString(); // Strip trailing fractional zeros safely
    };
    
    const formatTickY = (val) => {
        if (Math.abs(val) < 1e-10) return "0";
        const precision = Math.max(0, -Math.floor(Math.log10(spacingY)) + 1);
        return parseFloat(val.toFixed(precision)).toString(); // Strip trailing fractional zeros safely
    };
    
    // Calculate background brightness to adjust grid/label text colors dynamically
    const bg = panel.bgColor;
    const luminance = 0.299 * bg.r + 0.587 * bg.g + 0.114 * bg.b;
    const isDarkBg = luminance < 0.5;
    
    // If the plot specified exact colors via C++, use them. Otherwise, intelligently fallback.
    const gridColor = panel.plotConfig ? panel.plotConfig.gridColor : (isDarkBg ? 'rgba(255, 255, 255, 0.25)' : 'rgba(0, 0, 0, 0.22)');
    const textColor = panel.plotConfig ? panel.plotConfig.fgColor : (isDarkBg ? 'rgba(255, 255, 255, 0.92)' : 'rgba(0, 0, 0, 0.92)');
    const axisColor = panel.plotConfig ? panel.plotConfig.fgColor : (isDarkBg ? 'rgba(255, 255, 255, 0.65)' : 'rgba(0, 0, 0, 0.60)');
    
    tickCtx.font = '11px "JetBrains Mono", monospace';
    
    // Draw X-axis grid lines and bottom tick marks
    const startX = Math.ceil(minX / spacingX) * spacingX;
    for (let x = startX; x <= maxX; x += spacingX) {
        const p = worldToPixel(x, 0);
        
        // Grid Line (Background Canvas)
        gridCtx.beginPath();
        gridCtx.moveTo(p.x, 0);
        gridCtx.lineTo(p.x, height);
        gridCtx.strokeStyle = gridColor;
        gridCtx.lineWidth = 1;
        gridCtx.stroke();
        
        // Tick mark drawn higher (safely inside the canvas area) (Foreground Canvas)
        tickCtx.beginPath();
        tickCtx.moveTo(p.x, height - 35);
        tickCtx.lineTo(p.x, height - 25);
        tickCtx.strokeStyle = axisColor;
        tickCtx.lineWidth = 1.5;
        tickCtx.stroke();
        
        // Numbered label drawn higher (safely inside the canvas area) (Foreground Canvas)
        tickCtx.fillStyle = textColor;
        tickCtx.textAlign = 'center';
        tickCtx.textBaseline = 'bottom';
        tickCtx.fillText(formatTickX(x), p.x, height - 38);
    }
    
    // Draw Y-axis grid lines and left tick marks
    const startY = Math.ceil(minY / spacingY) * spacingY;
    for (let y = startY; y <= maxY; y += spacingY) {
        const p = worldToPixel(0, y);
        
        // Grid Line (Background Canvas)
        gridCtx.beginPath();
        gridCtx.moveTo(0, p.y);
        gridCtx.lineTo(width, p.y);
        gridCtx.strokeStyle = gridColor;
        gridCtx.lineWidth = 1;
        gridCtx.stroke();
        
        // Tick mark on left (Foreground Canvas)
        tickCtx.beginPath();
        tickCtx.moveTo(20, p.y);
        tickCtx.lineTo(28, p.y);
        tickCtx.strokeStyle = axisColor;
        tickCtx.lineWidth = 1.5;
        tickCtx.stroke();
        
        // Numbered label on left (safely padded) (Foreground Canvas)
        tickCtx.fillStyle = textColor;
        tickCtx.textAlign = 'left';
        tickCtx.textBaseline = 'middle';
        tickCtx.fillText(formatTickY(y), 32, p.y);
    }
    
    // Draw origin axes lines if visible
    const originPix = worldToPixel(0, 0);
    if (originPix.x >= 0 && originPix.x <= width) {
        tickCtx.beginPath();
        tickCtx.moveTo(originPix.x, 0);
        tickCtx.lineTo(originPix.x, height);
        tickCtx.strokeStyle = isDarkBg ? 'rgba(0, 240, 255, 0.22)' : 'rgba(0, 150, 255, 0.22)';
        tickCtx.lineWidth = 1.5;
        tickCtx.stroke();
    }
    if (originPix.y >= 0 && originPix.y <= height) {
        tickCtx.beginPath();
        tickCtx.moveTo(0, originPix.y);
        tickCtx.lineTo(width, originPix.y);
        tickCtx.strokeStyle = isDarkBg ? 'rgba(0, 240, 255, 0.22)' : 'rgba(0, 150, 255, 0.22)';
        tickCtx.lineWidth = 1.5;
        tickCtx.stroke();
    }
    
    gridCtx.restore();
    tickCtx.restore();
}

// ==========================================
// 3. Pose, Translation & Aspect Appliers
// ==========================================
function base64ToFloat32Array(base64Str) {
    const binary = atob(base64Str);
    const len = binary.length;
    const buffer = new ArrayBuffer(len);
    const view = new Uint8Array(buffer);
    for (let i = 0; i < len; i++) {
        view[i] = binary.charCodeAt(i);
    }
    return new Float32Array(buffer);
}

function applyTransform(object3D, pose) {
    if (!pose) return;
    
    // Translation t
    if (pose.t) {
        object3D.position.set(pose.t[0], pose.t[1], pose.t[2]);
    } else {
        object3D.position.set(0, 0, 0);
    }
    
    // Rotation vector r (axis angle lie representation)
    if (pose.r) {
        const rx = pose.r[0], ry = pose.r[1], rz = pose.r[2];
        const angle = Math.sqrt(rx*rx + ry*ry + rz*rz);
        if (angle > 1e-6) {
            const axis = new THREE.Vector3(rx / angle, ry / angle, rz / angle);
            object3D.quaternion.setFromAxisAngle(axis, angle);
        } else {
            object3D.quaternion.set(0, 0, 0, 1);
        }
    } else {
        object3D.quaternion.set(0, 0, 0, 1);
    }
    
    // Scale
    const s = pose.scale !== undefined ? pose.scale : 1.0;

    // Extract individual flipping modifiers
    const xMod = (object3D.userData && object3D.userData.xFlip) ? -1 : 1;
    const yMod = (object3D.userData && object3D.userData.yFlip) ? -1 : 1;

    // If this is a text sprite, apply its aspect-ratio scale factors!
    if (object3D.userData && object3D.userData.isText) {
        const wFactor = object3D.userData.textWidthFactor || 1.0;
        object3D.scale.set(wFactor * s * xMod, s * yMod, 1);
    } else if (object3D.userData && object3D.userData.spriteAspect !== undefined) {
        // Native image sprites keep their true physical aspect ratio based on texture dimensions
        object3D.scale.set(object3D.userData.spriteAspect * s * xMod, s * yMod, 1);
    } else {
        object3D.scale.set(s * xMod, s * yMod, s);
    }
    }

// Memory Cleanup Handler
function disposeObject3D(obj) {
    if (obj.geometry) obj.geometry.dispose();
    if (obj.material) {
        const disposeMaterial = (m) => {
            // Traverse material properties to find and dispose of attached textures
            for (const key in m) {
                const value = m[key];
                if (value && typeof value === 'object' && 'minFilter' in value) {
                    value.dispose();
                }
            }
            m.dispose();
        };

        if (Array.isArray(obj.material)) {
            obj.material.forEach(m => disposeMaterial(m));
        } else {
            disposeMaterial(obj.material);
        }
    }
    if (obj.children) {
        obj.children.forEach(child => disposeObject3D(child));
    }
}

// ==========================================
// 4. Dynamic Interactive Controls
// ==========================================
function renderControlFlags(connId, flagsList) {
    const container = document.getElementById('flags-container');
    const flagsSection = document.getElementById('flags-section');
    
    if (!flagsList || flagsList.length === 0) {
        flagsSection.classList.add('hide');
        return;
    }
    
    flagsSection.classList.remove('hide');
    container.innerHTML = '';
    
    flagsList.forEach(flag => {
        const row = document.createElement('div');
        row.className = 'flag-row';
        
        const label = document.createElement('span');
        label.textContent = flag.name;
        row.appendChild(label);
        
        if (flag.toggle) {
            // Render Switch
            const switchDiv = document.createElement('label');
            switchDiv.className = 'switch';
            
            const input = document.createElement('input');
            input.type = 'checkbox';
            input.checked = flag.state;
            
            // Interaction handler
            input.addEventListener('change', () => {
                sendFlagUpdate(connId, flag.name, input.checked);
            });
            
            const slider = document.createElement('span');
            slider.className = 'slider round';
            
            switchDiv.appendChild(input);
            switchDiv.appendChild(slider);
            row.appendChild(switchDiv);
        } else {
            // Render Trigger Button
            const btn = document.createElement('button');
            btn.className = 'flag-btn';
            btn.textContent = 'Trigger';
            btn.addEventListener('click', () => {
                sendFlagUpdate(connId, flag.name, true);
            });
            row.appendChild(btn);
        }
        
        container.appendChild(row);
    });
}

function sendFlagUpdate(connId, name, value) {
    if (!state.ws || state.ws.readyState !== WebSocket.OPEN) return;

    // Optimistic UI update: instantly mutate the local tracked state so incoming echoes don't bounce the slider!
    if (state.activeFlags[connId]) {
        const flag = state.activeFlags[connId].find(f => f.name === name);
        if (flag) {
            flag.state = value;
        }
    }

    const eventMsg = {
        type: "event",
        conn_id: connId,
        header: {
            type: "flags",
            flags: {
                [name]: value
            }
        },
        payloads: []
    };

    state.ws.send(JSON.stringify(eventMsg));
}

// ==========================================
// 5. WebSocket Client & TCP proxy
// ==========================================
function connectWebSocket() {
    const wsUrl = `ws://${window.location.hostname}:5634`;
    console.log(`[WS] Connecting to gateway at ${wsUrl}...`);
    
    state.ws = new WebSocket(wsUrl);
    
    state.ws.onopen = () => {
        console.log('[WS] Connected to Gateway.');
        showToast('Gateway Connected', 'success');
        
        // Request active TCP list
        state.ws.send(JSON.stringify({ type: 'get_connections' }));
    };
    
    state.ws.onmessage = (event) => {
        try {
            const data = JSON.parse(event.data);
            
            if (data.type === 'connection_list') {
                updateConnectionListUI(data.connections);
            } 
            else if (data.type === 'scene') {
                parseScenePayload(data.conn_id, data.header, data.payloads);
            }
            else if (data.type === 'notification') {
                showToast(data.message, data.level || 'info');
                
                // Parse failures from Gateway: "Connection failed to host:port: error..."
                if (data.level === 'error' && data.message.startsWith('Connection failed to ')) {
                    const match = data.message.match(/Connection failed to ([^:]+:\d+)/);
                    if (match && match[1]) {
                        addOrUpdateRecentConnection(match[1], 'failed');
                    }
                }
            }
        } catch (e) {
            console.error('[WS] Error processing message:', e);
        }
    };
    
    state.ws.onclose = () => {
        console.log('[WS] Disconnected. Reconnecting in 3s...');
        showToast('Gateway Disconnected', 'error');
        setTimeout(connectWebSocket, 3000);
    };
}

function updateConnectionListUI(connections) {
    state.activeConnections = connections || [];
    const list = document.getElementById('connections-list');
    
    // Build set of active connection IDs
    const activeIds = new Set(connections.map(c => c.id));
    
    // Clean up panels of disconnected feeds to prevent memory leaks!
    Object.keys(state.panels).forEach(cid => {
        const c_id = parseInt(cid);
        if (!activeIds.has(c_id)) {
            const feedPanels = state.panels[c_id] || {};
            Object.keys(feedPanels).forEach(winId => {
                const panel = feedPanels[winId];
                panel.card.remove();
                disposeObject3D(panel.feedGroup);
                disposeObject3D(panel.feedHUDGroup);
                panel.renderer.dispose();
            });
            delete state.panels[c_id];
            delete state.panelOrder[c_id];
            delete state.objectCounters[c_id];
            delete state.activeFlags[c_id];
        }
    });

    if (!connections || connections.length === 0) {
        list.innerHTML = `<div class="empty-state">No active connections. Listening for visualizer sources...</div>`;
        state.activeConnId = null;
        renderControlFlags(null, []);
        updateCanvasGrid();
        return;
    }
    
    list.innerHTML = '';
    
    connections.forEach(conn => {
        if (conn.direction === 'outbound') {
            addOrUpdateRecentConnection(conn.peer, 'success');
        }
        
        const item = document.createElement('div');
        item.className = 'conn-item';
        if (state.activeConnId === conn.id) {
            item.classList.add('active-feed');
        }
        
        const sourceLabel = state.activeConnId === conn.id ? 'ACTIVE' : 'READY';
        
        item.innerHTML = `
            <div class="conn-info">
                <span class="conn-host">
                    <span class="conn-badge ${conn.direction === 'inbound' ? 'badge-in' : 'badge-out'}">
                        ${conn.direction === 'inbound' ? 'IN' : 'OUT'}
                    </span>
                    ${conn.peer}
                </span>
                <span class="conn-details">ID: ${conn.id} | Status: ${sourceLabel}</span>
            </div>
            <div class="conn-actions">
                <button class="btn btn-secondary btn-small select-feed-btn" data-id="${conn.id}">View</button>
                <button class="btn btn-secondary btn-small disconnect-feed-btn" data-id="${conn.id}"><i class="fa-solid fa-times"></i></button>
            </div>
        `;
        
        list.appendChild(item);
    });
    
    // Auto-select first connection if none active
    if (!state.activeConnId && connections.length > 0) {
        selectActiveFeed(connections[0].id);
    }
    
    // Event listeners
    document.querySelectorAll('.select-feed-btn').forEach(btn => {
        btn.addEventListener('click', (e) => {
            const cid = parseInt(e.target.getAttribute('data-id'));
            selectActiveFeed(cid);
        });
    });
    
    document.querySelectorAll('.disconnect-feed-btn').forEach(btn => {
        btn.addEventListener('click', (e) => {
            const cid = parseInt(e.currentTarget.getAttribute('data-id'));
            state.ws.send(JSON.stringify({ type: 'disconnect_target', conn_id: cid }));
        });
    });
}

function selectActiveFeed(connId) {
    state.activeConnId = connId;
    console.log(`[UI] Active feed switched to Connection: ${connId}`);

    // Recalculate grid splits for this connection's panels
    updateCanvasGrid();

    // Sync telemetry labels
    document.getElementById('tel-source').textContent = `Conn ${connId}`;

    // Sync active flags
    if (state.activeFlags[connId]) {
        renderControlFlags(connId, state.activeFlags[connId]);
    } else {
        renderControlFlags(connId, []);
    }

    // Request list redraw for active states
    state.ws.send(JSON.stringify({ type: 'get_connections' }));

    // Request the FULL cached state payload (objects + binary buffers) from the gateway 
    // exactly once upon feed selection to populate the late-joiner canvas!
    state.ws.send(JSON.stringify({ type: 'request_full_state', conn_id: connId }));
}

// ==========================================
// 5. Scene Parser & Translators
// ==========================================
function parseScenePayload(connId, header, payloads) {
    // Extract root scene data block first
    let data = header.data;
    if (!data) data = header; // Handle direct root scene message objects

    // 1. Extract window ID and details (Window 1 has ID 0 which is omitted, so default to 0)
    // Prioritize checking header (root) where C++ writes window_id, then fall back to data.
    const windowId = (header.window_id !== undefined) ? header.window_id : ((data.window_id !== undefined) ? data.window_id : 0);
    const title = (data.window && data.window.title) || `Show ${windowId + 1}`;
    
    // 2. Retrieve or create scoped panel for this Window ID
    if (!state.panels[connId]) {
        state.panels[connId] = {};
        state.panelOrder[connId] = [];
        state.objectCounters[connId] = {};
    }
    
    let panel = state.panels[connId][windowId];
    if (!panel) {
        panel = createPanel(connId, windowId, title);
        state.panels[connId][windowId] = panel;
        state.panelOrder[connId].push(windowId);
        updateCanvasGrid();
    }
    
    // 3. Handle window settings
    if (data.window) {
        if (data.window.title) {
            panel.titleTag.textContent = data.window.title;
            if (connId === state.activeConnId && windowId === 0) {
                document.title = `Vephor Web Visualizer - ${data.window.title}`;
                document.getElementById('tel-source').textContent = data.window.title;
            }
        }
    }

    // Handle camera settings (detect 2D plotting mode vs standard 3D modes)
    if (data.camera && data.camera.control) {
        console.log(`[Camera] Received for connection ${connId}, Window ${windowId}:`, data.camera.control);
        const cam = data.camera.control;
        
        // Handle solid background setting if sent by C++
        if (data.camera.background && data.camera.background.type === "solid") {
            const bgCol = data.camera.background.color;
            panel.bgColor.setRGB(bgCol[0], bgCol[1], bgCol[2]);
            panel.card.style.backgroundColor = '#' + panel.bgColor.getHexString();
        }
        
        if (cam.type === "plot") {
            panel.plotEqualAspect = (cam.equal === true);
            panel.xFlip = (cam.x_flip === true);
            panel.yFlip = (cam.y_flip === true);
            
            // Extract Plot Color Configuration
            const bgColorRGB = cam.back_color || [0.941, 0.949, 0.960]; // default #f0f2f5
            const fgColorRGB = cam.fore_color || [0.2, 0.2, 0.2];       // default dark text
            const gridColorRGB = cam.grid_color || [0.85, 0.85, 0.85];  // default light grid
            
            // Store for drawPlotOverlay to use
            panel.plotConfig = {
                bgColor: `rgb(${Math.round(bgColorRGB[0]*255)}, ${Math.round(bgColorRGB[1]*255)}, ${Math.round(bgColorRGB[2]*255)})`,
                fgColor: `rgb(${Math.round(fgColorRGB[0]*255)}, ${Math.round(fgColorRGB[1]*255)}, ${Math.round(fgColorRGB[2]*255)})`,
                gridColor: `rgb(${Math.round(gridColorRGB[0]*255)}, ${Math.round(gridColorRGB[1]*255)}, ${Math.round(gridColorRGB[2]*255)})`
            };
            
            panel.bgColor.setRGB(bgColorRGB[0], bgColorRGB[1], bgColorRGB[2]);
            panel.card.style.backgroundColor = '#' + panel.bgColor.getHexString();
            
            // Rebuild legend DOM if labels exist
            if (cam.labels && cam.labels.length > 0) {
                panel.legendContainer.style.display = 'block';
                panel.legendContainer.innerHTML = ''; // clear
                
                // Position logic (respecting dynamic y_flip!)
                let isTop = cam.legend_top !== false;
                if (panel.yFlip) {
                    isTop = !isTop;
                }

                if (isTop) {
                    panel.legendContainer.style.top = '10px';
                    panel.legendContainer.style.bottom = 'auto';
                } else {
                    panel.legendContainer.style.bottom = '35px'; // Above ticks
                    panel.legendContainer.style.top = 'auto';
                }
                
                if (cam.legend_right !== false) {
                    panel.legendContainer.style.right = '10px';
                    panel.legendContainer.style.left = 'auto';
                } else {
                    panel.legendContainer.style.left = '35px'; // Next to ticks
                    panel.legendContainer.style.right = 'auto';
                }
                
                // Dark mode adjustment
                const luminance = 0.299 * panel.bgColor.r + 0.587 * panel.bgColor.g + 0.114 * panel.bgColor.b;
                if (luminance < 0.5) {
                    panel.legendContainer.style.backgroundColor = 'rgba(30, 30, 30, 0.85)';
                    panel.legendContainer.style.color = '#eee';
                    panel.legendContainer.style.border = '1px solid rgba(255,255,255,0.1)';
                } else {
                    panel.legendContainer.style.backgroundColor = 'rgba(255, 255, 255, 0.85)';
                    panel.legendContainer.style.color = '#333';
                    panel.legendContainer.style.border = '1px solid rgba(0,0,0,0.1)';
                }
                
                cam.labels.forEach(label => {
                    const row = document.createElement('div');
                    row.style.display = 'flex';
                    row.style.alignItems = 'center';
                    row.style.marginBottom = '4px';
                    
                    const icon = document.createElement('span');
                    icon.style.display = 'inline-block';
                    icon.style.width = '12px';
                    icon.style.height = '12px';
                    icon.style.marginRight = '8px';
                    
                    if (label.color) {
                        const r = Math.round(label.color[0] * 255);
                        const g = Math.round(label.color[1] * 255);
                        const b = Math.round(label.color[2] * 255);
                        icon.style.backgroundColor = `rgb(${r},${g},${b})`;
                    } else {
                        icon.style.backgroundColor = '#ccc';
                    }
                    
                    if (label.type === 'circle') icon.style.borderRadius = '50%';
                    else if (label.type === 'line') {
                        icon.style.height = '3px';
                        icon.style.width = '16px';
                        icon.style.marginRight = '4px';
                    }
                    else if (label.type === 'diamond') {
                        icon.style.transform = 'rotate(45deg)';
                        icon.style.width = '10px';
                        icon.style.height = '10px';
                        icon.style.margin = '1px 9px 1px 1px';
                    }
                    
                    const text = document.createElement('span');
                    text.textContent = label.text;
                    
                    row.appendChild(icon);
                    row.appendChild(text);
                    panel.legendContainer.appendChild(row);
                });
                
                // Remove margin from last row
                if (panel.legendContainer.lastChild) {
                    panel.legendContainer.lastChild.style.marginBottom = '0';
                }
            } else {
                panel.legendContainer.style.display = 'none';
            }
            
            if (!panel.is2DPlotMode) {
                panel.is2DPlotMode = true;
                
                // Align orthographic camera looking straight down at the 2D XY Plane (looking down Z)
                panel.orthoCamera.position.set(0, 0, 1000);
                panel.orthoCamera.zoom = 1.0; // Reset zoom factor
                panel.orthoControls.target.set(0, 0, 0);
                
                // Update and refresh controls
                panel.orthoControls.update();
                
                // Hide standard 3D helpers that skew 2D plots
                panel.gridHelper.visible = false;
                panel.axesHelper.visible = false;
                
                // Trigger panel resize to configure correct aspect ratios
                resizePanel(panel);
                
                showToast(`Switched to 2D Plot Mode for ${title}`, 'success');
            }
        } else {
            // Restore standard 3D perspective controls if we switch back
            if (panel.is2DPlotMode) {
                panel.is2DPlotMode = false;
                panel.controls.target.set(0, 0, 0);
                panel.controls.update();
                panel.gridHelper.visible = state.gridEnabled;
                panel.axesHelper.visible = state.axesEnabled;
                
                // Trigger panel resize to configure correct aspect ratios
                resizePanel(panel);
            }
            
            // Constantly ensure the 3D up vector is completely respected by OrbitControls for rotations!
            if (cam.up) {
                const newUp = new THREE.Vector3(cam.up[0], cam.up[1], cam.up[2]).normalize();
                
                // Only overwrite if it actually changed, preventing interference with damping momentum!
                if (panel.camera.up.distanceToSquared(newUp) > 1e-6) {
                    panel.camera.up.copy(newUp);
                    panel.controls.update(); // Force OrbitControls to recalculate spherical axes immediately
                }
            }
            
            // On startup (before auto-fit locks in), apply initial trackball/3D vector directions
            if (!panel.hasAutoFitted) {
                if (cam.to) {
                    panel.controls.target.set(cam.to[0], cam.to[1], cam.to[2]);
                }
                if (cam.from) {
                    panel.camera.position.set(cam.from[0], cam.from[1], cam.from[2]);
                }
                panel.controls.update();
            }
        }
    }
    
    // 4. Handle interactive flag updates (metadata messages)
    if (header.flags && Array.isArray(header.flags)) {
        if (!state.activeFlags[connId]) {
            state.activeFlags[connId] = header.flags;
        } else {
            // Safely merge flags. If the user just toggled it ON, ignore C++ echoing 'false' 
            // since C++ checkAndConsumeFlag instantly resets toggles to false!
            header.flags.forEach(incomingFlag => {
                const localFlag = state.activeFlags[connId].find(f => f.name === incomingFlag.name);
                if (localFlag) {
                    if (localFlag.toggle && localFlag.state === true && incomingFlag.state === false) {
                        // User toggled it on, and C++ hasn't yet caught up or is echoing its consumed false state.
                        // Keep our local optimistic TRUE state!
                    } else {
                        localFlag.state = incomingFlag.state;
                    }
                } else {
                    state.activeFlags[connId].push(incomingFlag);
                }
            });
        }
        
        if (connId === state.activeConnId) {
            renderControlFlags(connId, state.activeFlags[connId]);
        }
    }
    
    // 5. Parse 3D Visual Objects list
    if (data && data.objects && Array.isArray(data.objects)) {
        panel.frameQueue = (panel.frameQueue || Promise.resolve()).then(async () => {
            if (!panel.lastObjLogTime || Date.now() - panel.lastObjLogTime > 3000) {
                panel.lastObjLogTime = Date.now();
                console.log(`[Plot Debug] parseScenePayload: windowId=${windowId}, objects.length=${data.objects.length}`);
            }
            
            const createdMeshes = new Map();
            const createPromises = [];
            
            // Pass 1.1: Async Creation (Build objects and wait for all textures/OBJs to load completely)
            data.objects.forEach(obj => {
                if (!obj.destroy && !panel.feedObjects[obj.id]) {
                    const p = createVisualNode(obj, obj.base_buf_index || 0, payloads, panel).then(mesh => {
                        if (mesh) createdMeshes.set(obj.id, mesh);
                    });
                    createPromises.push(p);
                }
            });
            
            // Wait for all new objects in this frame to be fully constructed and their async resources downloaded
            await Promise.all(createPromises);
            
            // Pass 1.2: Synchronous Application (Safely destroy old, add new, and update existing objects)
            data.objects.forEach(obj => {
                const objId = obj.id;
                
                if (obj.destroy) {
                    const mesh = panel.feedObjects[objId];
                    if (mesh) {
                        if (mesh.parent) mesh.parent.remove(mesh);
                        disposeObject3D(mesh);
                        delete panel.feedObjects[objId];
                        delete panel.objectCounters[objId];
                        console.log(`  - Object ${objId} deleted.`);
                    }
                } else {
                    let mesh = panel.feedObjects[objId];
                    if (!mesh) {
                        mesh = createdMeshes.get(objId);
                        if (mesh) {
                            mesh.name = String(objId);
                            if (obj.overlay) panel.feedHUDGroup.add(mesh);
                            else panel.feedGroup.add(mesh);
                            
                            panel.feedObjects[objId] = mesh;
                            panel.objectCounters[objId] = true;
                            console.log(`  - Object ${objId} (${obj.type}) created successfully!`);
                        } else {
                            console.warn(`  - Object ${objId} (${obj.type}) failed to create.`);
                        }
                    }
                    
                    if (mesh) {
                        applyTransform(mesh, obj.pose);
                        if (obj.show !== undefined) mesh.visible = obj.show;
                    }
                }
            });
            
            // Pass 2: Resolve parent relationships & dynamic HUD anchoring
            data.objects.forEach(obj => {
                if (obj.destroy) return;
                
                const mesh = panel.feedObjects[obj.id];
                if (!mesh) return;
                
                if (obj.pose_parent !== undefined && obj.pose_parent !== null) {
                    const parentName = String(obj.pose_parent);
                    const anchors = [
                        "window_top_left", "window_top", "window_top_right",
                        "window_left", "window_center", "window_right",
                        "window_bottom_left", "window_bottom", "window_bottom_right"
                    ];
                    
                    if (anchors.includes(parentName)) {
                        const anchorGroup = panel.feedHUDGroup.getObjectByName(parentName);
                        if (anchorGroup && mesh.parent !== anchorGroup) {
                            anchorGroup.add(mesh);
                        }
                    } else {
                        const parentNode = panel.feedObjects[parentName];
                        if (parentNode && mesh.parent !== parentNode) {
                            parentNode.add(mesh);
                        }
                    }
                } else if (!mesh.parent) {
                    if (obj.overlay) {
                        panel.feedHUDGroup.add(mesh);
                    } else {
                        panel.feedGroup.add(mesh);
                    }
                }
            });
            
            // Auto-fit bounds on startup
            if (!panel.hasAutoFitted && Object.keys(panel.objectCounters).length > 0) {
                panel.hasAutoFitted = true;
                autoFitPanelBounds(panel);
            }
            
            // Update active object counts in telemetry
            if (connId === state.activeConnId) {
                let totalObjects = 0;
                Object.values(state.panels[connId]).forEach(p => {
                    totalObjects += Object.keys(p.objectCounters).length;
                });
                document.getElementById('tel-objects').textContent = totalObjects;
            }
        });
    }
}

async function createVisualNode(obj, baseBufIdx, payloads, panel) {
    const type = obj.type;
    const pose = obj.pose;
    
    let object3D = null;
    
    const getRGBAColor = (rgbaArr) => {
        if (!rgbaArr) return new THREE.Color(0xffffff);
        return new THREE.Color(rgbaArr[0], rgbaArr[1], rgbaArr[2]);
    };
    
    switch (type) {
        case 'null': {
            object3D = new THREE.Group();
            applyTransform(object3D, pose);
            break;
        }
        
        case 'lines':
        case 'thick_lines': {
            let positions = null;
            let colors = null;
            
            if (obj.verts.type === 'raw') {
                const bId = obj.verts.buf + baseBufIdx;
                positions = base64ToFloat32Array(payloads[bId]);
            } else if (obj.verts.type === 'base64') {
                positions = base64ToFloat32Array(obj.verts.data);
            } else if (obj.verts.type === 'list') {
                positions = new Float32Array(obj.verts.list.flat());
            }
            
            if (obj.colors) {
                if (obj.colors.type === 'raw') {
                    const bId = obj.colors.buf + baseBufIdx;
                    colors = base64ToFloat32Array(payloads[bId]);
                } else if (obj.colors.type === 'base64') {
                    colors = base64ToFloat32Array(obj.colors.data);
                } else if (obj.colors.type === 'list') {
                    colors = new Float32Array(obj.colors.list.flat());
                }
            }
            
            if (!positions) return null;
            
            const geometry = new THREE.BufferGeometry();
            geometry.setAttribute('position', new THREE.BufferAttribute(positions, 3));
            
            let material;
            if (colors) {
                const comps = colors.length / (positions.length / 3);
                geometry.setAttribute('color', new THREE.BufferAttribute(colors, comps));
                material = new THREE.LineBasicMaterial({ vertexColors: true, transparent: comps === 4, opacity: 1.0 });
            } else {
                const colRgba = obj.default_color_rgba || [1, 1, 1, 1];
                material = new THREE.LineBasicMaterial({ color: getRGBAColor(colRgba), transparent: colRgba[3] < 1.0, opacity: colRgba[3] });
            }
            
            if (obj.strip !== false) {
                object3D = new THREE.Line(geometry, material);
            } else {
                object3D = new THREE.LineSegments(geometry, material);
            }
            applyTransform(object3D, pose);
            break;
        }
        
        case 'mesh': {
            const geometry = new THREE.BufferGeometry();
            
            if (obj.verts) {
                const verts = new Float32Array(obj.verts.flat());
                geometry.setAttribute('position', new THREE.BufferAttribute(verts, 3));
            }
            if (obj.norms) {
                const norms = new Float32Array(obj.norms.flat());
                geometry.setAttribute('normal', new THREE.BufferAttribute(norms, 3));
            }
            if (obj.uvs) {
                const uvs = new Float32Array(obj.uvs.flat());
                geometry.setAttribute('uv', new THREE.BufferAttribute(uvs, 2));
            }
            
            const colorRgba = obj.color_rgba || [1, 1, 1, 1];
            const material = new THREE.MeshStandardMaterial({
                color: getRGBAColor(colorRgba),
                roughness: 0.5,
                metalness: 0.1,
                transparent: colorRgba[3] < 1.0,
                opacity: colorRgba[3],
                side: obj.cull === false ? THREE.DoubleSide : THREE.FrontSide
            });
            
            if (obj.tex) {
                await loadTextureToMaterial(material, obj.tex, baseBufIdx, payloads);
            }
            
            object3D = new THREE.Mesh(geometry, material);
            applyTransform(object3D, pose);
            break;
        }
        
        case 'obj_mesh': {
            object3D = new THREE.Group();
            applyTransform(object3D, pose);
            
            const objPath = obj.path;
            let mtlPath = null;
            
            // Assume the MTL file has the same base name as the OBJ file
            if (objPath.toLowerCase().endsWith('.obj')) {
                mtlPath = objPath.substring(0, objPath.length - 4) + '.mtl';
            }
            
            await new Promise((resolve) => {
                const loadOBJ = (materials) => {
                    const loader = new THREE.OBJLoader();
                    if (materials) {
                        materials.preload();
                        loader.setMaterials(materials);
                    }
                    
                    loader.load(objPath, (loadedObj) => {
                        loadedObj.traverse(child => {
                            if (child.isMesh) {
                                if (!materials) {
                                    // Fallback if no MTL could be loaded
                                    child.material = new THREE.MeshStandardMaterial({
                                        color: 0xcccccc,
                                        roughness: 0.6
                                    });
                                }
                                child.castShadow = true;
                                child.receiveShadow = true;
                            }
                        });
                        object3D.add(loadedObj);
                        resolve();
                    }, undefined, (err) => {
                        console.warn(`[OBJ] Error loading OBJ mesh from path: ${objPath}`, err);
                        resolve();
                    });
                };
                
                if (mtlPath) {
                    const mtlLoader = new THREE.MTLLoader();
                    // Extract base path for texture loading relative to the MTL file
                    const lastSlash = mtlPath.lastIndexOf('/');
                    if (lastSlash !== -1) {
                        mtlLoader.setPath(mtlPath.substring(0, lastSlash + 1));
                    }
                    const mtlFileName = lastSlash !== -1 ? mtlPath.substring(lastSlash + 1) : mtlPath;
                    
                    mtlLoader.load(mtlFileName, (materials) => {
                        loadOBJ(materials);
                    }, undefined, (err) => {
                        console.warn(`[MTL] Could not load MTL ${mtlPath}, falling back to default material.`, err);
                        loadOBJ(null);
                    });
                } else {
                    loadOBJ(null);
                }
            });
            break;
        }
        
        case 'sphere': {
            const rad = obj.rad || 1.0;
            const geom = new THREE.SphereGeometry(rad, obj.slices || 16, obj.stacks || 16);
            const mat = new THREE.MeshStandardMaterial({ color: getRGBAColor(obj.color_rgb || [1, 1, 1]) });
            object3D = new THREE.Mesh(geom, mat);
            applyTransform(object3D, pose);
            break;
        }
        
        case 'cylinder': {
            const rad = obj.rad || 1.0;
            const h = obj.height || 1.0;
            const geom = new THREE.CylinderGeometry(rad, rad, h, obj.slices || 16);
            geom.rotateX(Math.PI / 2);
            const mat = new THREE.MeshStandardMaterial({ color: getRGBAColor(obj.color_rgb || [1, 1, 1]) });
            object3D = new THREE.Mesh(geom, mat);
            applyTransform(object3D, pose);
            break;
        }
        
        case 'cone': {
            const rad = obj.rad || 1.0;
            const h = obj.height || 1.0;
            const geom = new THREE.ConeGeometry(rad, h, obj.slices || 16);
            geom.rotateX(Math.PI / 2);
            const mat = new THREE.MeshStandardMaterial({ color: getRGBAColor(obj.color_rgb || [1, 1, 1]) });
            object3D = new THREE.Mesh(geom, mat);
            applyTransform(object3D, pose);
            break;
        }
        
        case 'cube': {
            const r = obj.rad || 1.0;
            const geom = new THREE.BoxGeometry(r*2, r*2, r*2);
            const mat = new THREE.MeshStandardMaterial({ color: getRGBAColor(obj.color_rgb || [1, 1, 1]) });
            object3D = new THREE.Mesh(geom, mat);
            applyTransform(object3D, pose);
            break;
        }
        
        case 'plane': {
            const rads = obj.rads || [1, 1];
            const geom = new THREE.PlaneGeometry(rads[0]*2, rads[1]*2);
            const mat = new THREE.MeshStandardMaterial({ 
                color: getRGBAColor(obj.color_rgba || [1, 1, 1, 1]),
                side: THREE.DoubleSide,
                transparent: true,
                alphaTest: 0.05
            });
            
            if (obj.tex) {
                await loadTextureToMaterial(mat, obj.tex, baseBufIdx, payloads);
            }
            
            object3D = new THREE.Mesh(geom, mat);
            applyTransform(object3D, pose);
            break;
        }
        
        case 'arrow': {
            object3D = new THREE.Group();
            
            const start = new THREE.Vector3(...(obj.start || [0, 0, 0]));
            const end = new THREE.Vector3(...(obj.end || [0, 0, 1]));
            const col = getRGBAColor(obj.color_rgba || [1, 1, 1, 1]);
            const shaftRad = obj.rad || 0.1;
            
            const dir = new THREE.Vector3().subVectors(end, start);
            const len = dir.length();
            
            if (len > 1e-4) {
                dir.normalize();
                const headLength = Math.min(len * 0.25, shaftRad * 4);
                const headRad = shaftRad * 2;
                const arrowHelper = new THREE.ArrowHelper(dir, start, len, col.getHex(), headLength, headRad);
                object3D.add(arrowHelper);
            }
            applyTransform(object3D, pose);
            break;
        }
        
        case 'axes': {
            const size = obj.size || 1.0;
            object3D = new THREE.AxesHelper(size);
            applyTransform(object3D, pose);
            break;
        }
        
        case 'circle': {
            const rad = obj.rad || 1.0;
            const geometry = new THREE.RingGeometry(rad - (obj.thickness || 0.05), rad, obj.slices || 32);
            const material = new THREE.MeshBasicMaterial({ color: getRGBAColor(obj.color_rgb || [1, 1, 1]), side: THREE.DoubleSide });
            object3D = new THREE.Mesh(geometry, material);
            applyTransform(object3D, pose);
            break;
        }
        
        case 'grid': {
            const r = obj.rad || 10.0;
            const cellSize = obj.cell_size || 1.0;
            const segments = Math.floor((r * 2) / cellSize);
            const gridHelper = new THREE.GridHelper(r * 2, segments, getRGBAColor(obj.color_rgb || [1, 1, 1]), 0x444444);
            
            const normal = obj.normal ? new THREE.Vector3(obj.normal[0], obj.normal[1], obj.normal[2]).normalize() : new THREE.Vector3(0, 0, 1);
            const right = obj.right ? new THREE.Vector3(obj.right[0], obj.right[1], obj.right[2]).normalize() : new THREE.Vector3(1, 0, 0);
            
            let forward = new THREE.Vector3().crossVectors(right, normal);
            if (forward.lengthSq() < 1e-6) {
                // Safely fallback if right and normal are parallel, preventing NaN matrix corruption!
                forward.crossVectors(new THREE.Vector3(0, 1, 0), normal);
                if (forward.lengthSq() < 1e-6) forward.crossVectors(new THREE.Vector3(1, 0, 0), normal);
            }
            forward.normalize();
            
            const newRight = new THREE.Vector3().crossVectors(normal, forward).normalize();
            
            const matrix = new THREE.Matrix4();
            // THREE.GridHelper sits on the XZ plane natively, meaning its normal is Y.
            // We map local X -> newRight, local Y -> normal, local Z -> forward.
            matrix.makeBasis(newRight, normal, forward);
            
            gridHelper.quaternion.setFromRotationMatrix(matrix);
            
            // Wrap in a group so applyTransform doesn't overwrite our calculated orientation basis!
            object3D = new THREE.Group();
            object3D.add(gridHelper);
            
            applyTransform(object3D, pose);
            break;
        }
        
        case 'text': {
            const text = obj.text || '';
            const col = getRGBAColor(obj.color_rgb || [1, 1, 1]);
            const anchor = obj.anchor || [0, 0];
            const canvasHeight = 128;
            const font = 'bold 80px "JetBrains Mono", monospace';
            
            const canvas = document.createElement('canvas');
            const ctx = canvas.getContext('2d');
            canvas.height = canvasHeight;
            ctx.font = font;

            // Size the quad from the font that will actually render the text. Using
            // a fixed width per character distorted the texture and made horizontal
            // anchor offsets disagree with the visible text bounds.
            const metrics = ctx.measureText(text);
            const leftExtent = metrics.actualBoundingBoxLeft || 0;
            const rightExtent = metrics.actualBoundingBoxRight || metrics.width;
            const measuredWidth = leftExtent + rightExtent;
            canvas.width = Math.max(1, Math.ceil(measuredWidth));
            const textWidthFactor = canvas.width / canvasHeight;
            
            ctx.fillStyle = 'transparent';
            ctx.fillRect(0, 0, canvas.width, canvas.height);
            
            // Resizing a canvas resets its drawing state.
            ctx.font = font;
            ctx.fillStyle = `#${col.getHexString()}`;
            ctx.textAlign = 'left';
            ctx.textBaseline = 'middle';
            ctx.fillText(text, leftExtent, canvas.height / 2);
            
            const tex = new THREE.CanvasTexture(canvas);
            
            const isBillboard = obj.billboard !== false; // Defaults to true
            
            if (isBillboard) {
                const mat = new THREE.SpriteMaterial({ map: tex, transparent: true });
                const sprite = new THREE.Sprite(mat);
                
                sprite.userData = {
                    isText: true,
                    textWidthFactor: textWidthFactor,
                    xFlip: obj.x_flip === true,
                    yFlip: obj.y_flip === true
                };
                
                sprite.center.set(anchor[0], anchor[1]);
                object3D = sprite;
            } else {
                // If not billboarding, render as a flat 3D plane so it fully respects 3D rotation
                const mat = new THREE.MeshBasicMaterial({ 
                    map: tex, 
                    transparent: true, 
                    side: THREE.DoubleSide,
                    depthWrite: false
                });
                
                const geom = new THREE.PlaneGeometry(textWidthFactor, 1.0);
                const mesh = new THREE.Mesh(geom, mat);
                
                // Align plane to mimic standard Sprite coordinate orientation
                // By default, planes face +Z, we need to map anchors correctly
                // Anchor coordinates are [0.0 to 1.0] from bottom-left. Vephor's
                // serialized default is omitted and means bottom-left, not the
                // centered default used by Three.js geometry and sprites.
                const transX = (0.5 - anchor[0]) * textWidthFactor;
                const transY = (0.5 - anchor[1]) * 1.0;
                geom.translate(transX, transY, 0);
                
                mesh.userData = {
                    xFlip: obj.x_flip === true,
                    yFlip: obj.y_flip === true
                };
                
                object3D = mesh;
            }

            applyTransform(object3D, pose);
            break;
        }
        
        case 'particle': {
            let positions = null;
            let colors = null;
            let sizes = null;
            
            if (obj.verts.type === 'raw') {
                const bId = obj.verts.buf + baseBufIdx;
                positions = base64ToFloat32Array(payloads[bId]);
            } else if (obj.verts.type === 'base64') {
                positions = base64ToFloat32Array(obj.verts.data);
            } else if (obj.verts.type === 'list') {
                positions = new Float32Array(obj.verts.list.flat());
            }
            
            if (obj.colors) {
                if (obj.colors.type === 'raw') {
                    const bId = obj.colors.buf + baseBufIdx;
                    colors = base64ToFloat32Array(payloads[bId]);
                } else if (obj.colors.type === 'base64') {
                    colors = base64ToFloat32Array(obj.colors.data);
                } else if (obj.colors.type === 'list') {
                    colors = new Float32Array(obj.colors.list.flat());
                }
            }
            
            if (obj.sizes) {
                if (obj.sizes.type === 'raw') {
                    const bId = obj.sizes.buf + baseBufIdx;
                    sizes = base64ToFloat32Array(payloads[bId]);
                } else if (obj.sizes.type === 'base64') {
                    sizes = base64ToFloat32Array(obj.sizes.data);
                }
            }
            
            if (!positions) return null;
            
            const geometry = new THREE.BufferGeometry();
            geometry.setAttribute('position', new THREE.BufferAttribute(positions, 3));
            
            if (colors) {
                const comps = colors.length / (positions.length / 3);
                geometry.setAttribute('color', new THREE.BufferAttribute(colors, comps));
            }
            
            const defColor = obj.default_color_rgba || [1, 1, 1, 1];
            const defSize = obj.size || 0.03;
            
            const height = panel ? panel.card.clientHeight : 800;
            const useOrtho = panel ? panel.is2DPlotMode : false;
            
            const material = new THREE.PointsMaterial({
                size: useOrtho ? (height * defSize) : defSize,
                color: colors ? 0xffffff : getRGBAColor(defColor),
                vertexColors: colors !== null,
                transparent: defColor[3] < 1.0,
                opacity: defColor[3],
                sizeAttenuation: !useOrtho
            });
            
            if (obj.tex && obj.tex !== "") {
                const texLoader = new THREE.TextureLoader();
                await new Promise((resolve) => {
                    texLoader.load(obj.tex, (loadedTex) => {
                        material.map = loadedTex;
                        material.transparent = true;
                        material.needsUpdate = true;
                        resolve();
                    }, undefined, () => resolve());
                });
            }
            
            object3D = new THREE.Points(geometry, material);
            object3D.userData = {
                defSize: defSize
            };
            applyTransform(object3D, pose);
            break;
        }
        
        case 'sprite': {
            const diffuse = obj.color_rgb || [1, 1, 1];
            const mat = new THREE.SpriteMaterial({ color: getRGBAColor(diffuse), transparent: true });
            
            object3D = new THREE.Sprite(mat);
            object3D.userData = {
                xFlip: obj.x_flip === true,
                yFlip: obj.y_flip === true
            };
            
            if (obj.tex) {
                await loadTextureToMaterial(mat, obj.tex, baseBufIdx, payloads, (loadedTex) => {
                    if (loadedTex.image && loadedTex.image.width && loadedTex.image.height) {
                        const aspect = loadedTex.image.width / loadedTex.image.height;
                        object3D.userData.spriteAspect = aspect;
                    }
                });
            }
            
            applyTransform(object3D, pose);
            break;
        }
        
        default:
            console.warn(`[Node] Unhandled primitive visual type: ${type}`);
            return null;
    }
    
    if (object3D && object3D.material) {
        // Handle array of materials (e.g., for OBJ files) or single material
        const materials = Array.isArray(object3D.material) ? object3D.material : [object3D.material];
        
        materials.forEach(m => {
            m.wireframe = state.wireframe;
            
            // Force depth writing for transparent materials so higher Z values properly occlude lower Z values.
            // By default, Three.js sets depthWrite = false for transparent materials, relying purely on painter's algorithm
            // which breaks down when items perfectly intersect or sit on flat 2D plotting planes.
            if (m.transparent) {
                m.depthTest = true;
                m.depthWrite = true;
                
                // Use a standard alpha test to discard perfectly clear pixels from depth buffer
                if (m.alphaTest === undefined || m.alphaTest === 0) {
                    m.alphaTest = 0.05;
                }
            }
        });
    }
    
    return object3D;
}

function loadTextureToMaterial(material, texInfo, baseBufIdx, payloads, onLoadCallback = null) {
    return new Promise((resolve) => {
        const applySampling = (tex) => {
            if (texInfo.filter_nearest) {
                tex.magFilter = THREE.NearestFilter;
                tex.minFilter = THREE.NearestFilter;
            }
            return tex;
        };

        if (texInfo.type === 'file') {
            const path = texInfo.path;
            new THREE.TextureLoader().load(path, (loadedTex) => {
                material.map = applySampling(loadedTex);
                if (path.toLowerCase().endsWith('.png')) {
                    material.transparent = true;
                    material.alphaTest = 0.05;
                    material.depthWrite = true;
                }
                material.needsUpdate = true;
                if (onLoadCallback) onLoadCallback(loadedTex);
                resolve();
            }, undefined, (err) => {
                console.warn(`[Texture] Failed to load from file path: ${path}`, err);
                resolve();
            });
        } 
        else if (texInfo.type === 'jpg') {
            const bId = texInfo.buf + baseBufIdx;
            const b64 = payloads[bId];
            if (b64) {
                const binary = atob(b64);
                const array = [];
                for (let i = 0; i < binary.length; i++) {
                    array.push(binary.charCodeAt(i));
                }
                const blob = new Blob([new Uint8Array(array)], { type: 'image/jpeg' });
                const url = URL.createObjectURL(blob);
                
                new THREE.TextureLoader().load(url, (loadedTex) => {
                    material.map = applySampling(loadedTex);
                    material.needsUpdate = true;
                    if (onLoadCallback) onLoadCallback(loadedTex);
                    URL.revokeObjectURL(url);
                    resolve();
                }, undefined, (err) => {
                    URL.revokeObjectURL(url);
                    resolve();
                });
            } else {
                resolve();
            }
        }
        else if (texInfo.type === 'raw') {
            const bId = texInfo.buf + baseBufIdx;
            const b64 = payloads[bId];
            const size = texInfo.size || [1, 1];
            const channels = texInfo.channels || 3;
            
            if (b64) {
                const rawBytes = base64ToFloat32Array(b64);
                let format = THREE.RGBFormat;
                if (channels === 1) format = THREE.LuminanceFormat;
                else if (channels === 4) format = THREE.RGBAFormat;
                
                const rawTex = new THREE.DataTexture(rawBytes, size[0], size[1], format, THREE.FloatType);
                applySampling(rawTex);
                rawTex.needsUpdate = true;
                material.map = rawTex;
                material.needsUpdate = true;
                
                // For DataTexture, image boundaries are directly assigned to the image property from the size array
                if (!rawTex.image) rawTex.image = {};
                rawTex.image.width = size[0];
                rawTex.image.height = size[1];
                
                if (onLoadCallback) onLoadCallback(rawTex);
            }
            resolve();
        } else {
            resolve();
        }
    });
}

// ==========================================
// 6. UI Controls & Present Managers
// ==========================================
function initUI() {
    // 1. Peer Connection Form
    const connectBtn = document.getElementById('connect-btn');
    const targetHost = document.getElementById('target-host');
    
    connectBtn.addEventListener('click', () => {
        const val = targetHost.value.trim();
        if (!val) return;
        
        let host = 'localhost';
        let port = 5533;
        
        if (val.includes(':')) {
            const parts = val.split(':');
            host = parts[0];
            port = parseInt(parts[1]) || 5533;
        } else {
            host = val;
        }
        
        const targetPeer = `${host}:${port}`;
        
        // Prevent duplicate connections
        const isAlreadyConnected = state.activeConnections.some(conn => conn.peer === targetPeer);
        if (isAlreadyConnected) {
            showToast(`Already connected to ${targetPeer}`, 'info');
            return;
        }
        
        if (state.ws && state.ws.readyState === WebSocket.OPEN) {
            addOrUpdateRecentConnection(targetPeer, 'attempting');
            state.ws.send(JSON.stringify({
                type: 'connect_target',
                host: host,
                port: port
            }));
            showToast(`Connecting to peer ${targetPeer}...`, 'info');
        }
    });

    // 2. Camera presets click triggers
    document.querySelectorAll('.preset-btn').forEach(btn => {
        btn.addEventListener('click', (e) => {
            const preset = e.target.getAttribute('data-view');
            triggerCameraPreset(preset);
        });
    });

    // 3. Auto-Fit Bounds button
    document.getElementById('fit-bounds-btn').addEventListener('click', () => {
        autoFitSceneBounds();
    });

    // 4. Rendering Toggle switches
    document.getElementById('wireframe-toggle').addEventListener('change', (e) => {
        state.wireframe = e.target.checked;
        Object.values(state.panels[state.activeConnId] || {}).forEach(panel => {
            panel.scene.traverse(child => {
                if (child.isMesh && child.material) {
                    child.material.wireframe = state.wireframe;
                }
            });
        });
    });

    document.getElementById('grid-toggle').addEventListener('change', (e) => {
        state.gridEnabled = e.target.checked;
        Object.values(state.panels[state.activeConnId] || {}).forEach(panel => {
            panel.gridHelper.visible = state.gridEnabled;
        });
    });

    document.getElementById('axes-toggle').addEventListener('change', (e) => {
        state.axesEnabled = e.target.checked;
        Object.values(state.panels[state.activeConnId] || {}).forEach(panel => {
            panel.axesHelper.visible = state.axesEnabled;
        });
    });

    // GLFW Key Mapping for C++ backend parity
    const glfwKeyMap = {
        'Space': 32, 'Quote': 39, 'Comma': 44, 'Minus': 45, 'Period': 46, 'Slash': 47,
        'Digit0': 48, 'Digit1': 49, 'Digit2': 50, 'Digit3': 51, 'Digit4': 52, 'Digit5': 53, 'Digit6': 54, 'Digit7': 55, 'Digit8': 56, 'Digit9': 57,
        'Semicolon': 59, 'Equal': 61,
        'KeyA': 65, 'KeyB': 66, 'KeyC': 67, 'KeyD': 68, 'KeyE': 69, 'KeyF': 70, 'KeyG': 71, 'KeyH': 72, 'KeyI': 73, 'KeyJ': 74, 'KeyK': 75, 'KeyL': 76, 'KeyM': 77, 'KeyN': 78, 'KeyO': 79, 'KeyP': 80, 'KeyQ': 81, 'KeyR': 82, 'KeyS': 83, 'KeyT': 84, 'KeyU': 85, 'KeyV': 86, 'KeyW': 87, 'KeyX': 88, 'KeyY': 89, 'KeyZ': 90,
        'BracketLeft': 91, 'Backslash': 92, 'BracketRight': 93, 'Backquote': 96,
        'Escape': 256, 'Enter': 257, 'Tab': 258, 'Backspace': 259, 'Insert': 260, 'Delete': 261,
        'ArrowRight': 262, 'ArrowLeft': 263, 'ArrowDown': 264, 'ArrowUp': 265,
        'PageUp': 266, 'PageDown': 267, 'Home': 268, 'End': 269,
        'CapsLock': 280, 'ScrollLock': 281, 'NumLock': 282, 'PrintScreen': 283, 'Pause': 284,
        'F1': 290, 'F2': 291, 'F3': 292, 'F4': 293, 'F5': 294, 'F6': 295, 'F7': 296, 'F8': 297, 'F9': 298, 'F10': 299, 'F11': 300, 'F12': 301,
        'ShiftLeft': 340, 'ControlLeft': 341, 'AltLeft': 342, 'MetaLeft': 343,
        'ShiftRight': 344, 'ControlRight': 345, 'AltRight': 346, 'MetaRight': 347
    };

    const activeKeys = new Set();

    // 1. Send `key_press` on initial physical depression (ignoring OS auto-repeats)
    window.addEventListener('keydown', (e) => {
        if (e.repeat) return; // Completely ignore OS-level rapid-fire auto-repeats
        if (!state.activeConnId || !state.ws || state.ws.readyState !== WebSocket.OPEN) return;
        if (e.target.tagName === 'INPUT' || e.target.tagName === 'TEXTAREA') return;

        const glfwCode = glfwKeyMap[e.code];
        if (glfwCode !== undefined) {
            activeKeys.add(e.code);
            Object.keys(state.panels[state.activeConnId] || {}).forEach(windowId => {
                const eventMsg = {
                    type: "event",
                    conn_id: state.activeConnId,
                    header: {
                        type: "key_press",
                        window: parseInt(windowId),
                        key: glfwCode
                    },
                    payloads: []
                };
                state.ws.send(JSON.stringify(eventMsg));
            });
        }
    });

    // 2. Send `key_release` on physical key lift (to complete C++ event callbacks)
    window.addEventListener('keyup', (e) => {
        if (!state.activeConnId || !state.ws || state.ws.readyState !== WebSocket.OPEN) return;
        if (e.target.tagName === 'INPUT' || e.target.tagName === 'TEXTAREA') return;

        const glfwCode = glfwKeyMap[e.code];
        if (glfwCode !== undefined && activeKeys.has(e.code)) {
            activeKeys.delete(e.code);
            Object.keys(state.panels[state.activeConnId] || {}).forEach(windowId => {
                const eventMsg = {
                    type: "event",
                    conn_id: state.activeConnId,
                    header: {
                        type: "key_release",
                        window: parseInt(windowId),
                        key: glfwCode
                    },
                    payloads: []
                };
                state.ws.send(JSON.stringify(eventMsg));
            });
        }
    });

    // 5. Sidebar Toggle UI sliding
    const sidebar = document.getElementById('sidebar');
    const toggleSidebarBtn = document.getElementById('toggle-sidebar-btn');
    const openBtn = document.createElement('button');
    openBtn.id = 'open-sidebar-btn';
    openBtn.innerHTML = '<i class="fa-solid fa-bars"></i>';
    document.body.appendChild(openBtn);
    
    toggleSidebarBtn.addEventListener('click', () => {
        sidebar.classList.add('collapsed');
        setTimeout(() => { openBtn.style.display = 'block'; }, 400);
    });

    openBtn.addEventListener('click', () => {
        sidebar.classList.remove('collapsed');
        openBtn.style.display = 'none';
    });

    // Global mousemove/mouseup resizer events
    window.addEventListener('mousemove', (e) => {
        if (state.isResizingSplit) {
            const delta = e.clientX - state.dragStartPos;
            const deltaFr = delta / state.pixelsPerFr;
            
            const sizes = state.gridSizes[state.activeConnId].cols;
            const idx = state.dragSplitIndex;
            
            let newLeft = state.dragStartSizes[idx] + deltaFr;
            let newRight = state.dragStartSizes[idx + 1] - deltaFr;
            
            const minFr = 0.1;
            if (newLeft < minFr) {
                newRight -= (minFr - newLeft);
                newLeft = minFr;
            } else if (newRight < minFr) {
                newLeft -= (minFr - newRight);
                newRight = minFr;
            }
            
            sizes[idx] = newLeft;
            sizes[idx + 1] = newRight;
            
            const container = document.getElementById('canvas-container');
            container.style.gridTemplateColumns = sizes.map(c => `${c}fr`).join(' 6px ');
            
            Object.values(state.panels[state.activeConnId] || {}).forEach(panel => resizePanel(panel));
        }
        else if (state.isResizingSplitH) {
            const delta = e.clientY - state.dragStartPos;
            const deltaFr = delta / state.pixelsPerFr;
            
            const sizes = state.gridSizes[state.activeConnId].rows;
            const idx = state.dragSplitIndex;
            
            let newTop = state.dragStartSizes[idx] + deltaFr;
            let newBottom = state.dragStartSizes[idx + 1] - deltaFr;
            
            const minFr = 0.1;
            if (newTop < minFr) {
                newBottom -= (minFr - newTop);
                newTop = minFr;
            } else if (newBottom < minFr) {
                newTop -= (minFr - newBottom);
                newBottom = minFr;
            }
            
            sizes[idx] = newTop;
            sizes[idx + 1] = newBottom;
            
            const container = document.getElementById('canvas-container');
            container.style.gridTemplateRows = sizes.map(r => `${r}fr`).join(' 6px ');
            
            Object.values(state.panels[state.activeConnId] || {}).forEach(panel => resizePanel(panel));
        }
    });
    
    window.addEventListener('mouseup', () => {
        if (state.isResizingSplit) {
            state.isResizingSplit = false;
            document.body.style.cursor = 'default';
            document.querySelectorAll('.grid-splitter').forEach(s => s.classList.remove('active-dragging'));
        }
        if (state.isResizingSplitH) {
            state.isResizingSplitH = false;
            document.body.style.cursor = 'default';
            document.querySelectorAll('.grid-splitter-h').forEach(s => s.classList.remove('active-dragging'));
        }
    });

    // Bind Panel Split Ratio range slider
    const splitRatioSlider = document.getElementById('split-ratio-slider');
    if (splitRatioSlider) {
        splitRatioSlider.addEventListener('input', (e) => {
            const container = document.getElementById('canvas-container');
            const leftPercent = parseInt(e.target.value);
            
            container.style.gridTemplateColumns = `${leftPercent}% 6px ${100 - leftPercent}%`;
            
            // Reflow all active WebGL viewports
            Object.values(state.panels[state.activeConnId] || {}).forEach(panel => {
                resizePanel(panel);
            });
        });
    }
}

function triggerCameraPreset(preset) {
    Object.values(state.panels[state.activeConnId] || {}).forEach(panel => {
        panel.controls.reset();
        panel.orthoControls.reset();
        switch (preset) {
            case 'isometric':
                panel.camera.position.set(5, 5, 5);
                break;
            case 'top':
                panel.camera.position.set(0, 0, 10);
                panel.orthoCamera.position.set(0, 0, 1000);
                break;
            case 'front':
                panel.camera.position.set(0, -10, 0);
                break;
            case 'side':
                panel.camera.position.set(10, 0, 0);
                break;
        }
        panel.controls.update();
        panel.orthoControls.update();
    });
}

function autoFitPanelBounds(panel) {
    const box = new THREE.Box3().setFromObject(panel.feedGroup);
    if (box.isEmpty()) {
        console.log(`[AutoFit Debug] Panel ${panel.windowId} bounding box is empty.`);
        return false;
    }
    
    const sphere = box.getBoundingSphere(new THREE.Sphere());
    const radius = sphere.radius;
    const center = sphere.center;
    
    console.log(`[AutoFit Debug] Panel ${panel.windowId}: is2DPlotMode=${panel.is2DPlotMode}, boxMin=(${box.min.x.toFixed(1)}, ${box.min.y.toFixed(1)}), boxMax=(${box.max.x.toFixed(1)}, ${box.max.y.toFixed(1)}), center=(${center.x.toFixed(1)}, ${center.y.toFixed(1)}), radius=${radius.toFixed(1)}`);
    
    if (panel.is2DPlotMode) {
        // Translate both the camera and the target identically to keep the view perfectly flat (no diagonal rotation!)
        panel.orthoCamera.position.set(center.x, center.y, 1000);
        panel.orthoControls.target.copy(center);
        
        if (panel.plotEqualAspect) {
            // Adjust ortho camera frustum bounds to encase sphere radius uniformly
            const aspect = panel.card.clientWidth / panel.card.clientHeight;
            const padRadius = radius * 1.25;
            
            panel.orthoHeightUnits = padRadius;
            panel.orthoWidthUnits = padRadius * aspect;
            
            panel.orthoCamera.left = -panel.orthoWidthUnits;
            panel.orthoCamera.right = panel.orthoWidthUnits;
            panel.orthoCamera.top = panel.orthoHeightUnits;
            panel.orthoCamera.bottom = -panel.orthoHeightUnits;
        } else {
            // For non-equal plots, fit X and Y independently based tightly on true box limits
            const size = box.getSize(new THREE.Vector3());
            
            // Provide a minimum size fallback if dataset is perfectly flat
            const spanX = Math.max(size.x, 1e-5) * 1.25 / 2;
            const spanY = Math.max(size.y, 1e-5) * 1.25 / 2;
            
            panel.orthoHeightUnits = spanY;
            panel.orthoWidthUnits = spanX;
            
            panel.orthoCamera.left = -spanX;
            panel.orthoCamera.right = spanX;
            panel.orthoCamera.top = spanY;
            panel.orthoCamera.bottom = -spanY;
        }

        panel.orthoCamera.scale.x = panel.xFlip ? -1 : 1;
        panel.orthoCamera.scale.y = panel.yFlip ? -1 : 1;
        
        panel.orthoCamera.updateProjectionMatrix();
        panel.orthoControls.update();
    } else {
        const oldTarget = panel.controls.target.clone();
        panel.controls.target.copy(center);
        
        // Move the camera by the same delta so the viewing angle is perfectly preserved!
        const targetDelta = new THREE.Vector3().subVectors(center, oldTarget);
        panel.camera.position.add(targetDelta);
        
        const fov = panel.camera.fov * (Math.PI / 180);
        let cameraDist = Math.max(Math.abs(radius / Math.sin(fov / 2)) * 1.25, 0.1);
        
        // Dynamically update clipping planes to perfectly enclose the data while maximizing depth buffer precision
        panel.camera.near = Math.max(cameraDist / 1000, 0.001);
        panel.camera.far = Math.max(cameraDist * 100, 10000);
        panel.camera.updateProjectionMatrix();
        
        const dir = new THREE.Vector3().subVectors(panel.camera.position, panel.controls.target);
        if (dir.lengthSq() < 1e-6) dir.set(0, 0, 1);
        dir.normalize();
        
        panel.camera.position.copy(dir).multiplyScalar(cameraDist).add(panel.controls.target);
        
        panel.camera.lookAt(center);
        panel.controls.update();
    }
    return true;
}

function autoFitSceneBounds() {
    let fitted = false;
    Object.values(state.panels[state.activeConnId] || {}).forEach(panel => {
        if (autoFitPanelBounds(panel)) {
            fitted = true;
        }
    });
    
    if (fitted) {
        showToast('Aligned all active panels to their scene bounds', 'success');
    }
}

// Notification system helper
function showToast(message, type = 'info') {
    const container = document.getElementById('toast-container');
    const toast = document.createElement('div');
    toast.className = `toast toast-${type}`;
    
    let icon = 'fa-info-circle';
    if (type === 'success') icon = 'fa-check-circle';
    if (type === 'error') icon = 'fa-exclamation-triangle';
    
    toast.innerHTML = `
        <span><i class="fa-solid ${icon}" style="margin-right: 8px;"></i> ${message}</span>
        <button class="toast-close" style="background:transparent; border:none; color:var(--text-muted); cursor:pointer; margin-left:15px;"><i class="fa-solid fa-times"></i></button>
    `;
    
    container.appendChild(toast);
    
    const timeout = setTimeout(() => {
        toast.style.animation = 'slideInRight 0.3s ease reverse';
        setTimeout(() => toast.remove(), 300);
    }, 4000);
    
    toast.querySelector('.toast-close').addEventListener('click', () => {
        clearTimeout(timeout);
        toast.remove();
    });
}
