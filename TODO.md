TODO:
	- check for binding errors
	- check for memory leak in show
	- check animation of trackball window - slow zoom?
	- reposition multiple windows
	- make methods (such as color ctor) more accepting of double vectors
	- check interaction between save and server render
	- Add key press based waiting and close-proof windows
	- Server issue when closing one of multiple animated windows
	- simple measuring tool in 3d - perhaps show size while holding mouse?
	- pose playback server issue
	- fix thread related crash on exit from test show in server mode
	- network bug in show server or test show server:
		Waiting for server connection in background.
		Error on accept.: Invalid argument
	- Add default comm state for vephor
	- Clean comments
	- Docs
	- Python install instructions / Re-evaluate use of scikit-build
	x create combined repo
	- investigate plot usage with large values
	- ensure all functions are available in python
	- elective orthographic mode
	- Eigen ref loading for meshes also
	- Produce static images, python inline?
	- Create service script for linux
	- Click callbacks
	- 2d plotting
		- cv-like render methods
			x circle (filled or not)
			x line (with width)
			x polygon (filled or not, with alpha and border)
			- text
			x rectangle (with alpha and border)
		- ML labels
		- histogram
		- Text labels for y axis instead of numbers
		- Box selection
		- Axis scaling
		- Subplots
		x Thick plot lines
		x imshow
		x Labels for axes
		x Legend
		x Grid
	- Pose/Camera primitive
	- Skybox
	- Tooltips
	- Content aware legend
	- Allow changes to object appearance over ext
	- Lights in ext
	- Shadows
	- Mirrors
	- Sprite font tool
	- Reject invalid point cloud points
	- Close show windows whose connections have died?
	- Evaluate 2d normal direction
	- Materials
	x handle out of order parents
	x check framerates on show windows, esp. for multiple
	x particle broken pipe
	x add recording only mode
	x Link network windows together
	x ability to save
	x Terminate client process at end of run
	x add scroll zoom to plots
	x "video" playback
	x fix show duplication of file messages
	x close show from file if any window is closed
	x Piped input in file mode / Scene deltas over file
	x consider adding texture filter mode to image
	x replace texture creation functions if appropriate
	x ensure Images can be used as all textures
	x sprite normal sheets in ext
	x Fix windows Python library issues
	x double check default port
	x remove non daemon show server
	x investigate high cpu use in dormant show server
	x sometimes window close message seems to not get back to python/ext
	x check show server for mem leaks
	x consider moving network mode call outside of windows
	x install instructions
	x Figure out Python render network's wait key issue
	x Accept network parameters
	x clean obj loading code
	x Remove extra assets
	x Make equal plot zoom smoother
	x Make ext and ogl examples install the same
	x finish image creation functions
	x add anchors for other corners
	x Daemonize show server
	x Text anchors in ext
	x test_show render order for "corner" tags
	x Remove old plot2d
	x Accept scene deltas
	x Add ability for ext to be server as well as client
	x Threaded load
	x Eigen ref loading
	x Allow changes to member objects or move "owned" objects to transform tree
	x Allow changes to independent nodes
	x Support arbitrary transform trees
	x Network use
	x Restore examples
	x Send non GLFW keycodes to client
	x Two windows in ext
	x Set window parameters through ext
	x Don't send scene msg buffers when sending scene assets
	x Don't litter
	x Make composite types first class primitives
	x Take overlay and layer from user in ext
	x Auto resize to show contents
	x Blocking viz over network

Suggestions from presentation:
	- rviz - fly through (first/third person chase cam)
	- save frames out
	- json line files
	- cheap viz when no listeners
	- multiple windows
	- bounding boxes, annotations, semantic segmentation, confidence
	- meshlab - click to center
	- I think it would be useful to have callbacks in vephor. If the user clicks a point, have the option to write events/functions for additional visualizations
	- A comment legend in the json file would be useful to remember what data represents what (e.g. looking at it later after)
	- Please don't let bloat happen, if there is major expansion of features, consider a plugin type system each project can work with so it's not part of the core

pyplot-like interface notes:
	- need an "equal" option.
	- plot, scatter, hist
	- ellipses (for covariance) can be provided by a normal primitive - how to handle order?
	- disable z sorting and use painters alg?
	- imshow
	- Calls on base window, or new Plot class?
		- New plot class would keep this functionality separate

plt = Plot()
plt.plot(x,y)
plt.equal()
plt.render()
