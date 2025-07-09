/**
 * Copyright 2023
 * Carnegie Robotics, LLC
 * 4501 Hatfield Street, Pittsburgh, PA 15201
 * https://www.carnegierobotics.com
 *
 * This code is provided under the terms of the Master Services Agreement (the Agreement).
 * This code constitutes CRL Background Intellectual Property, as defined in the Agreement.
**/

#include <vephor.h>
#include <vephor_ext.h>
#include <pybind11/pybind11.h>
#include <pybind11/eigen.h>
#include <pybind11/functional.h>
#include <pybind11/operators.h>
#include <pybind11/stl.h>

#define STRINGIFY(x) #x
#define MACRO_STRINGIFY(x) STRINGIFY(x)

namespace py = pybind11;
using namespace vephor;

PYBIND11_MODULE(_core, m) {
    m.doc() = R"pbdoc(
        Vephor Visualization Library for Python
    )pbdoc";

	py::class_<Vec3>(m, "Vec3")
		.def(py::init<float,float,float>());
	py::class_<Orient3>(m, "Orient3")
		.def(py::init<>())
		.def(py::init<const Vec3&>())
		.def("inverse", &Orient3::inverse)
		.def("normalize", &Orient3::normalize)
		.def("rvec", &Orient3::rvec)
		.def(py::self * py::self)
		.def(py::self * Vec3())
		.def_static("fromMatrix", &Orient3::fromMatrix);
    py::class_<Transform3>(m, "Transform3")
		.def(py::init<>())
		.def(py::init<const Vec3&,const Vec3>(),py::arg("t"),py::arg("r")=Vec3(0,0,0))
		.def(py::init<const Vec3&,const Orient3&>())
		.def(py::self * py::self)
		.def(py::self * Vec3())
		.def("inverse", &Transform3::inverse)
		.def("translation", &Transform3::translation)
		.def("rvec", &Transform3::rvec)
		.def("rotation", &Transform3::rotation)
		.def("matrix", &Transform3::matrix)
		.def("interp", &Transform3::interp);
    py::class_<TransformSim3>(m, "TransformSim3")
		.def(py::init<>())
		.def(py::init<const Vec3&,const Vec3,float>(),py::arg("t"),py::arg("r")=Vec3(0,0,0),py::arg("scale")=1.0f)
		.def(py::init<const Transform3&,float>(),py::arg("transform"),py::arg("scale")=1.0f)
		.def(py::self * py::self)
		.def(py::self * Transform3())
		.def(py::self * Vec3());
    py::class_<TransformNode, shared_ptr<TransformNode>>(m, "TransformNode");
    py::class_<RenderNode, shared_ptr<RenderNode>>(m, "RenderNode")
		.def("setPos", &RenderNode::setPos)
		.def("getPos", &RenderNode::getPos)
		.def("setOrient", &RenderNode::setOrient)
		.def("setOrient", [](const shared_ptr<RenderNode>& node, const Vec3& r){node->setOrient(Orient3(r));})
		.def("getOrient", &RenderNode::getOrient)
		.def("setScale", &RenderNode::setScale)
		.def("getScale", &RenderNode::getScale)
		.def("setParent", static_cast<RenderNode* (RenderNode::*)(const shared_ptr<RenderNode>&)>(&RenderNode::setParent))
		.def("setParent", static_cast<RenderNode* (RenderNode::*)(const shared_ptr<TransformNode>&)>(&RenderNode::setParent))
		.def("setShow", &RenderNode::setShow)
		.def("getShow", &RenderNode::getShow)
		.def("setDestroy", &RenderNode::setDestroy)
		.def("getDestroy", &RenderNode::getDestroy)
		.def("enableOverlay", &RenderNode::enableOverlay)
		.def("setLayer", &RenderNode::setLayer)
		.def("disableBounds", &RenderNode::disableBounds)
		.def("setSelectable", &RenderNode::setSelectable);
	py::class_<MeshData>(m, "MeshData")
		.def(py::init<>())
		.def(py::init<const MatX&, const MatX&, const MatX&>())
		.def_readwrite("verts", &MeshData::verts)
		.def_readwrite("uvs", &MeshData::uvs)
		.def_readwrite("norms", &MeshData::norms);
	py::class_<Color>(m, "Color")
		.def(py::init<float,float,float,float>(),py::arg("r"),py::arg("g"),py::arg("b"),py::arg("a")=1.0)
		.def(py::init<const Vec3&>())
		.def(py::init<const Vec3u&>())
		.def(py::init<const Vec4&>())
		.def(py::init<const Vec4u&>())
		.def_static("fromHSL", &Color::fromHSL)
		.def_static("random", &Color::random)
		.def("getRGB", &Color::getRGB)
		.def("getRGBA", &Color::getRGBA)
		.def("getAlpha", &Color::getAlpha);
		
	
	m.def("setTextureCompression", &setTextureCompression, py::arg("compress"), py::arg("quality") = DEFAULT_COMPRESSION_QUALITY);

	m.def("formLine", &formLine, py::arg("vert_list"), py::arg("rad"));
	m.def("formLineLoop", &formLineLoop, py::arg("vert_list"), py::arg("rad"));
	m.def("formPolygon", &formPolygon, py::arg("vert_list"));
	m.def("formCube", &formCube);
	m.def("formSphere", &formSphere, py::arg("slices"), py::arg("stacks"));
	m.def("formCone", &formCone, py::arg("rad"), py::arg("height"), py::arg("slices"), py::arg("smooth")=true);
	m.def("formCylinder", &formCylinder, py::arg("rad"), py::arg("height"), py::arg("slices"), py::arg("smooth")=true);
	m.def("formPlane", &formPlane, py::arg("rads"));
	m.def("formCircle", &formCircle, py::arg("rad"), py::arg("thickness"), py::arg("slices"));
	m.def("formHeightMap", &formHeightMap, py::arg("heights"), py::arg("res"), py::arg("uv_callback")=NULL);
	m.def("formWireframeBox", &formWireframeBox);

	m.def("convertStringToColor", &convertStringToColor);

	m.def("calcSurfaces", [](
			py::buffer occupancy,
			float thresh, 
			float cell_size
		){
			py::buffer_info info = occupancy.request();

			if (info.format != py::format_descriptor<double>::format())
				throw std::runtime_error("Only double occupancy data supported.");

			if (info.shape.size() != 3)
				throw std::runtime_error("Only 3d occupancy data supported.");
				
			// Debug info
			v4print info.ndim;
			for (int i = 0; i < info.ndim; i++)
			{
				v4print "\t", info.shape[i], info.strides[i];
			}
			v4print info.itemsize, info.size, info.format;

			Tensor<3, float> occupancy_t({(int)info.shape[0],(int)info.shape[1],(int)info.shape[2]});

			const double *ptr = static_cast<const double *>(info.ptr);
			for (int i = 0; i < info.shape[0]; i++)
			for (int j = 0; j < info.shape[1]; j++)
			for (int k = 0; k < info.shape[2]; k++)
			{
				occupancy_t({i,j,k}) = *ptr;
				ptr++;
			}

			MeshData data;
			mcubes::calcSurfaces(occupancy_t, thresh, cell_size, data);
			return data;
		}, 
		py::arg("occupancy"), 
		py::arg("thresh"),
		py::arg("cell_size"));

	m.def("clamp", &clamp);
	
	py::class_<Image<uint8_t>, shared_ptr<Image<uint8_t>>>(m, "Image", pybind11::buffer_protocol())
		.def_buffer([](Image<uint8_t>& img) -> pybind11::buffer_info {
			return py::buffer_info(
				(void*)img.getData().data(),                /* Pointer to buffer */
				sizeof(uint8_t),                         /* Size of one scalar */
				py::format_descriptor<uint8_t>::format(), /* Python struct-style format descriptor */
				3,                                       /* Number of dimensions */
				{ img.getSize()[0], img.getSize()[1], img.getChannels() },                  /* Buffer dimensions */
				{ 
					sizeof(uint8_t) * img.getSize()[1] * img.getChannels(),
					sizeof(uint8_t) * img.getChannels(),
					sizeof(uint8_t)
				}
														 /* Strides (in bytes) for each index */
			);
		});
		
	m.def("generateSimpleImage", [](const Vec2i& size, py::buffer color){
		py::buffer_info info = color.request();
		if (info.size != 3 && info.size != 4)
			throw std::runtime_error("Color must be either 3 or 4 elements in size.");
		if (info.format == py::format_descriptor<uint8_t>::format())
		{
			const uint8_t* ptr = reinterpret_cast<const uint8_t*>(info.ptr);
			
			if (info.size == 3)
			{
				Vec3 rgb(ptr[0] / 255.0f, ptr[1] / 255.0f, ptr[2] / 255.0f);
				return generateSimpleImage(size, rgb);
			}
			else
			{
				Vec4 rgba(ptr[0] / 255.0f, ptr[1] / 255.0f, ptr[2] / 255.0f, ptr[3] / 255.0f);
				return generateSimpleImage(size, rgba);
			}
		}
		else if (info.format == py::format_descriptor<double>::format())
		{
			const double* ptr = reinterpret_cast<const double*>(info.ptr);
			
			if (info.size == 3)
			{
				Vec3 rgb(ptr[0], ptr[1], ptr[2]);
				return generateSimpleImage(size, rgb);
			}
			else
			{
				Vec4 rgba(ptr[0], ptr[1], ptr[2], ptr[3]);
				return generateSimpleImage(size, rgba);
			}
		}
		else
			throw std::runtime_error("Only supports uint8 or double typed color.");
		
		return shared_ptr<Image<uint8_t>>(NULL);
	}, py::arg("size"), py::arg("color"));

	py::class_<ShowMetadata>(m, "ShowMetadata");

	py::class_<Axes, shared_ptr<Axes>>(m, "Axes")
        .def(py::init<float>(),py::arg("size")=1.0f)
		.def("setColors",[](Axes& a, 
			const Color& x_c,
			const Color& y_c,
			const Color& z_c){
				a.setColors(
					Color(x_c),
					Color(y_c),
					Color(z_c)
				);
			}, 
			py::arg("x_c"),py::arg("y_c"),py::arg("z_c"))
		.def("setColors",[](Axes& a, 
			const Vec3& x_rgb,
			const Vec3& y_rgb,
			const Vec3& z_rgb){
				a.setColors(
					Color(x_rgb),
					Color(y_rgb),
					Color(z_rgb)
				);
			}, 
			py::arg("x_rgb"),py::arg("y_rgb"),py::arg("z_rgb"))
		.def("setColorsCMY", &Axes::setColorsCMY);

    py::class_<Sphere, shared_ptr<Sphere>>(m, "Sphere")
        .def(py::init<float,int,int>(),py::arg("rad")=1.0f,py::arg("slices")=12,py::arg("stacks")=12)
		.def("setColor",[](Sphere& s, 
			const Vec3& rgb){
				s.setColor(Color(rgb));
			}, 
			py::arg("rgb"))
		.def("setColor",[](Sphere& s, 
			const Vec4& rgba){
				s.setColor(Color(rgba));
			}, 
			py::arg("rgba"));

    py::class_<Cylinder, shared_ptr<Cylinder>>(m, "Cylinder")
        .def(py::init<float,float,int>(),py::arg("rad")=1.0f,py::arg("height")=1.0f,py::arg("slices")=12)
		.def("setColor",[](Cylinder& c, 
			const Vec3& rgb){
				c.setColor(Color(rgb));
			}, 
			py::arg("rgb"))
		.def("setColor",[](Cylinder& c, 
			const Vec4& rgba){
				c.setColor(Color(rgba));
			}, 
			py::arg("rgba"));

	py::class_<Cube, shared_ptr<Cube>>(m, "Cube")
        .def(py::init<float>(),py::arg("rad")=1.0f)
		.def("setColor",[](Cube& c, 
			const Vec3& rgb){
				c.setColor(Color(rgb));
			}, 
			py::arg("rgb"))
		.def("setColor",[](Cube& c, 
			const Vec4& rgba){
				c.setColor(Color(rgba));
			}, 
			py::arg("rgba"));

	py::class_<Cone, shared_ptr<Cone>>(m, "Cone")
        .def(py::init<float,float,int>(),py::arg("rad")=1.0f,py::arg("height")=1.0f,py::arg("slices")=12)
		.def("setColor",[](Cone& c, 
			const Vec3& rgb){
				c.setColor(Color(rgb));
			}, 
			py::arg("rgb"))
		.def("setColor",[](Cone& c, 
			const Vec4& rgba){
				c.setColor(Color(rgba));
			}, 
			py::arg("rgba"));

	py::class_<Grid, shared_ptr<Grid>>(m, "Grid")
        .def(py::init<float,Vec3,Vec3,float>(),py::arg("rad"),py::arg("normal")=Vec3(0,0,1),py::arg("right")=Vec3(1,0,0),py::arg("cell_size")=1.0f)
		.def("setColor",[](Grid& g, 
			const Vec3& rgb){
				g.setColor(rgb);
			}, 
			py::arg("rgb"));

	py::class_<Mesh, shared_ptr<Mesh>>(m, "Mesh")
        .def(py::init<MeshData>())
		.def("setColor",[](Mesh& m, 
			const Vec3& rgb){
				m.setColor(Color(rgb));
			}, 
			py::arg("rgb"))
		.def("setColor",[](Mesh& m, 
			const Vec4& rgba){
				m.setColor(Color(rgba));
			}, 
			py::arg("rgba"))
		.def("setSpecular", &Mesh::setSpecular)
		.def("setCull", &Mesh::setCull)
		.def("setDiffuseStrength", &Mesh::setDiffuseStrength)
		.def("setAmbientStrength", &Mesh::setAmbientStrength)
		.def("setEmissiveStrength", &Mesh::setEmissiveStrength);

	py::class_<ObjMesh, shared_ptr<ObjMesh>>(m, "ObjMesh")
        .def(py::init<string>());

	py::class_<Text, shared_ptr<Text>>(m, "Text")
		.def(py::init<string>())
		.def("setColor",[](Text& t, 
			const Color& c){
				t.setColor(c);
			}, 
			py::arg("color"))
		.def("setColor",[](Text& t, 
			const Vec3& rgb){
				t.setColor(Color(rgb));
			}, 
			py::arg("rgb"))
		.def("setColor",[](Text& t, 
			const Vec4& rgba){
				t.setColor(Color(rgba));
			}, 
			py::arg("rgba"))
		.def("setAnchorBottomLeft", &Text::setAnchorBottomLeft)
		.def("setAnchorLeft", &Text::setAnchorLeft)
		.def("setAnchorTopLeft", &Text::setAnchorTopLeft)
		.def("setAnchorBottom", &Text::setAnchorBottom)
		.def("setAnchorCentered", &Text::setAnchorCentered)
		.def("setAnchorTop", &Text::setAnchorTop)
		.def("setAnchorBottomRight", &Text::setAnchorBottomRight)
		.def("setAnchorRight", &Text::setAnchorRight)
		.def("setAnchorTopRight", &Text::setAnchorTopRight)
		.def("setBillboard", &Text::setBillboard)
		.def("setYFlip", &Text::setYFlip);

	py::class_<Lines, shared_ptr<Lines>>(m, "Lines")
		.def(py::init([](const MatX& verts, const MatX& colors){
			return make_shared<Lines>(verts, colors);
		}), py::arg("verts"), py::arg("colors")=MatX())
		.def("setColor",[](Lines& l, 
			const Color& c){
				l.setColor(c);
			}, 
			py::arg("color"))
		.def("setColor",[](Lines& l, 
			const Vec3& rgb){
				l.setColor(Color(rgb));
			}, 
			py::arg("rgb"))
		.def("setColor",[](Lines& l, 
			const Vec4& rgba){
				l.setColor(Color(rgba));
			}, 
			py::arg("rgba"))
		.def("setLineStrip",&Lines::setLineStrip,py::arg("strip")=true);

	py::class_<Particle, shared_ptr<Particle>>(m, "Particle")
		.def(py::init([](const MatX& verts, const MatX& colors){
			return make_shared<Particle>(verts, colors);
		}), py::arg("verts"), py::arg("colors")=MatX())
		.def("setColor",[](Particle& p, 
			const Color& c){
				p.setColor(c);
			}, 
			py::arg("color"))
		.def("setColor",[](Particle& p, 
			const Vec3& rgb){
				p.setColor(Color(rgb));
			}, 
			py::arg("rgb"))
		.def("setColor",[](Particle& p, 
			const Vec4& rgba){
				p.setColor(Color(rgba));
			}, 
			py::arg("rgba"))
		.def("setSize",&Particle::setSize,py::arg("size"))
		.def("setScreenSpaceMode",&Particle::setScreenSpaceMode,py::arg("ss_mode")=true);

	py::class_<Sprite, shared_ptr<Sprite>>(m, "Sprite")
		.def(py::init<string,bool>(), py::arg("path"), py::arg("nearest")=false)
		.def(py::init([](py::buffer buf, bool nearest){
				py::buffer_info info = buf.request();

				bool is_contiguous = true;
				ssize_t expected_stride = info.itemsize;
				for (ssize_t i = info.ndim - 1; i >= 0; --i) {
					if (info.strides[i] != expected_stride) {
						is_contiguous = false;
						break;
					}
					expected_stride *= info.shape[i];
				}

				if (!is_contiguous)
				{
					throw std::runtime_error("Sprite only supports contiguous arrays.");
				}

				int channels = 1;
				if (info.shape.size() > 2)
					channels = info.shape[2];
				
				Image<uint8_t> image(info.shape[1], info.shape[0], channels);
				
				if (info.format == py::format_descriptor<uint8_t>::format())
					image.copyFromBuffer(reinterpret_cast<const char*>(info.ptr), info.size);
				else if (info.format == py::format_descriptor<double>::format())
				{
					const double* ptr = reinterpret_cast<const double*>(info.ptr);
					
					for (int i = 0; i < info.shape[0]; i++)
					{
						for (int j = 0; j < info.shape[1]; j++)
						{
							const double* vec_ptr = ptr + i * info.shape[1] * channels + j * channels;
							image(j,i) = Vec3u(vec_ptr[0]*255, vec_ptr[1]*255, vec_ptr[2]*255);
						}
					}
				}
				else
				{
					throw std::runtime_error("Sprite only supports uint8 or double typed arrays.");
				}

				return make_shared<Sprite>(image, nearest);
		}), py::arg("buf"), py::arg("nearest")=false);

	py::class_<Plane, shared_ptr<Plane>>(m, "Plane")
        .def(py::init<Vec2>(),py::arg("rads"))
		.def("setColor",[](Plane& p, 
			const Vec3& rgb){
				p.setColor(Color(rgb));
			}, 
			py::arg("rgb"))
		.def("setColor",[](Plane& p, 
			const Vec4& rgba){
				p.setColor(Color(rgba));
			}, 
			py::arg("rgba"))
		.def("setTexture",[](Plane& p, const std::string& path, bool nearest){
				p.setTexture(path, nearest);
			}, py::arg("path"), py::arg("nearest")=false)
		.def("setTexture",[](Plane& p, py::buffer buf, bool nearest){
				py::buffer_info info = buf.request();

				int channels = 1;
				if (info.shape.size() > 2)
					channels = info.shape[2];
				
				Image<uint8_t> image(info.shape[1], info.shape[0], channels);
				
				if (info.format == py::format_descriptor<uint8_t>::format())
					image.copyFromBuffer(reinterpret_cast<const char*>(info.ptr), info.size);
				else if (info.format == py::format_descriptor<double>::format())
				{
					const double* ptr = reinterpret_cast<const double*>(info.ptr);
					
					for (int i = 0; i < info.shape[0]; i++)
					{
						for (int j = 0; j < info.shape[1]; j++)
						{
							const double* vec_ptr = ptr + i * info.shape[1] * info.shape[2] + j * info.shape[2];
							image(j,i) = Vec3u(vec_ptr[0]*255, vec_ptr[1]*255, vec_ptr[2]*255);
						}
					}
				}
				else
				{
					throw std::runtime_error("Plane::setTexture only supports uint8 or double typed arrays.");
				}

				p.setTexture(image, nearest);
			}, py::arg("buf"), py::arg("nearest")=false);

	py::class_<AmbientLight, shared_ptr<AmbientLight>>(m, "AmbientLight")
        .def(py::init<Vec3>(),py::arg("strength"));

	py::class_<DirLight, shared_ptr<DirLight>>(m, "DirLight")
        .def(py::init<Vec3,float>(),py::arg("dir"),py::arg("strength"));
	
    py::class_<Window> window(m, "Window");
    window
        .def(py::init<int,int,std::string>(),
			py::arg("width")=-1,
			py::arg("height")=-1,
			py::arg("name")="show")
		.def("clear", &Window::clear)
		.def("getNumObjects", &Window::getNumObjects)
		.def("getNumActiveObjects", &Window::getNumActiveObjects)
        .def("render", [](Window& w, bool wait_close, bool wait_key, float time_increment_s){
			return w.render(wait_close, wait_key, time_increment_s, [](){
				if (PyErr_CheckSignals() != 0)
                	throw py::error_already_set();
			});
		}, py::arg("wait_close")=true, py::arg("wait_key")=false, py::arg("time_increment_s")=0.0f)
		.def_static("renderInWaiting", &Window::renderInWaiting)
		.def("layoutAbsolute", &Window::layoutAbsolute, 
			py::arg("width"), 
			py::arg("height"), 
			py::arg("x_position")=-1,
			py::arg("y_position")=-1)
		.def("layoutPerUnit", &Window::layoutPerUnit, 
			py::arg("width"), 
			py::arg("height"), 
			py::arg("x_position")=-1,
			py::arg("y_position")=-1)
		.def_static("setRecordMode", &Window::setRecordMode, 
			py::arg("path"))
		.def_static("setClientMode", &Window::setClientMode, 
			py::arg("wait")=false, 
			py::arg("host")="localhost", 
			py::arg("port")=VEPHOR_DEFAULT_PORT,
			py::arg("record_also")=false,
			py::arg("record_path")="")
		.def_static("setClientModeBYOS", &Window::setClientModeBYOS, 
			py::arg("record_also")=false,
			py::arg("record_path")="")
		.def_static("setServerMode", &Window::setServerMode, 
			py::arg("wait")=false, 
			py::arg("port")=VEPHOR_DEFAULT_PORT,
			py::arg("record_also")=false,
			py::arg("record_path")="",
			py::arg("show_metadata")=ShowMetadata())
		.def_static("setServerModeBYOC", &Window::setServerModeBYOC, 
			py::arg("record_also")=false,
			py::arg("record_path")="")
		.def("getWindowTopLeftNode", &Window::getWindowTopLeftNode)
		.def("getWindowTopNode", &Window::getWindowTopNode)
		.def("getWindowTopRightNode", &Window::getWindowTopRightNode)
		.def("getWindowLeftNode", &Window::getWindowLeftNode)
		.def("getWindowCenterNode", &Window::getWindowCenterNode)
		.def("getWindowRightNode", &Window::getWindowRightNode)
		.def("getWindowBottomLeftNode", &Window::getWindowBottomLeftNode)
		.def("getWindowBottomNode", &Window::getWindowBottomNode)
		.def("getWindowBottomRightNode", &Window::getWindowBottomRightNode)
		.def("setStaticCameraMode", &Window::setStaticCameraMode, 
			py::arg("to")=Vec3(0,0,0), 
			py::arg("from")=Vec3(-1,0,-1), 
			py::arg("up")=Vec3(0,0,-1))
		.def("setSpinCameraMode", &Window::setSpinCameraMode, 
			py::arg("to")=Vec3(0,0,0), 
			py::arg("up")=Vec3(0,0,-1), 
			py::arg("from_dist")=10.0f, 
			py::arg("from_angle_deg")=30.0f,
			py::arg("s_per_rev")=10.0f)
		.def("setTrackballMode", &Window::setTrackballMode, 
			py::arg("to")=Vec3(0,0,0), 
			py::arg("from")=Vec3(-1,0,-1), 
			py::arg("up")=Vec3(0,0,-1), 
			py::arg("use_3d")=false)
		.def("setTrackballModeVision", &Window::setTrackballModeVision)
		.def("setSolidBackground",[](Window& w, 
			const Vec3& color){
				w.setSolidBackground(Color(color));
			}, 
			py::arg("color"))
		.def("setGradientBackground",[](Window& w, 
			const Vec3& top,
			const Vec3& bottom){
				w.setGradientBackground(Color(top), Color(bottom));
			}, 
			py::arg("top"),
			py::arg("bottom"))
		.def("setCheckerBackground",[](Window& w, 
			const Vec3& color_1,
			const Vec3& color_2,
			const Vec2i& n_cells){
				w.setCheckerBackground(Color(color_1), Color(color_2), n_cells);
			}, 
			py::arg("color_1"),
			py::arg("color_2"),
			py::arg("n_cells")=Vec2i(8,8))
		.def("setKeyPressCallback", &Window::setKeyPressCallback)
		.def("setMouseClickCallback", &Window::setMouseClickCallback)
		.def("setPlotMode", &Window::setPlotMode, py::arg("equal")=false)
		.def("setOpacity", &Window::setOpacity)
		.def("save", &Window::save)
        .def("add", static_cast<shared_ptr<RenderNode> (Window::*)(
            const Vec3&,
			const Vec3&,
			float)>(&Window::add),
			py::arg("t")=Vec3(0,0,0),
			py::arg("r")=Vec3(0,0,0),
			py::arg("scale")=1.0f)
		.def("add", static_cast<shared_ptr<RenderNode> (Window::*)(
            const shared_ptr<Axes>&,
            const Vec3&,
			const Vec3&,
			float,
            bool, 
            int)>(&Window::add<Axes>),
			py::arg("object"),
			py::arg("t")=Vec3(0,0,0),
			py::arg("r")=Vec3(0,0,0),
			py::arg("scale")=1.0f,
			py::arg("overlay")=false,
			py::arg("layer")=0)
		.def("add", static_cast<shared_ptr<RenderNode> (Window::*)(
            const shared_ptr<Axes>&,
            const TransformSim3&,
            bool, 
            int)>(&Window::add<Axes>),
			py::arg("object"),
			py::arg("T"),
			py::arg("overlay")=false,
			py::arg("layer")=0)
		.def("add", static_cast<shared_ptr<RenderNode> (Window::*)(
            const shared_ptr<Sphere>&,
            const Vec3&,
			const Vec3&,
			float,
            bool, 
            int)>(&Window::add<Sphere>),
			py::arg("object"),
			py::arg("t")=Vec3(0,0,0),
			py::arg("r")=Vec3(0,0,0),
			py::arg("scale")=1.0f,
			py::arg("overlay")=false,
			py::arg("layer")=0)
		.def("add", static_cast<shared_ptr<RenderNode> (Window::*)(
            const shared_ptr<Cylinder>&,
            const Vec3&,
			const Vec3&,
			float,
            bool, 
            int)>(&Window::add<Cylinder>),
			py::arg("object"),
			py::arg("t")=Vec3(0,0,0),
			py::arg("r")=Vec3(0,0,0),
			py::arg("scale")=1.0f,
			py::arg("overlay")=false,
			py::arg("layer")=0)
		.def("add", static_cast<shared_ptr<RenderNode> (Window::*)(
            const shared_ptr<Cube>&,
            const Vec3&,
			const Vec3&,
			float,
            bool, 
            int)>(&Window::add<Cube>),
			py::arg("object"),
			py::arg("t")=Vec3(0,0,0),
			py::arg("r")=Vec3(0,0,0),
			py::arg("scale")=1.0f,
			py::arg("overlay")=false,
			py::arg("layer")=0)
		.def("add", static_cast<shared_ptr<RenderNode> (Window::*)(
            const shared_ptr<Cone>&,
            const Vec3&,
			const Vec3&,
			float,
            bool, 
            int)>(&Window::add<Cone>),
			py::arg("object"),
			py::arg("t")=Vec3(0,0,0),
			py::arg("r")=Vec3(0,0,0),
			py::arg("scale")=1.0f,
			py::arg("overlay")=false,
			py::arg("layer")=0)
		.def("add", static_cast<shared_ptr<RenderNode> (Window::*)(
            const shared_ptr<Grid>&,
            const Vec3&,
			const Vec3&,
			float,
            bool, 
            int)>(&Window::add<Grid>),
			py::arg("object"),
			py::arg("t")=Vec3(0,0,0),
			py::arg("r")=Vec3(0,0,0),
			py::arg("scale")=1.0f,
			py::arg("overlay")=false,
			py::arg("layer")=0)
		.def("add", static_cast<shared_ptr<RenderNode> (Window::*)(
            const shared_ptr<Text>&,
            const Vec3&,
			const Vec3&,
			float,
            bool, 
            int)>(&Window::add<Text>),
			py::arg("object"),
			py::arg("t")=Vec3(0,0,0),
			py::arg("r")=Vec3(0,0,0),
			py::arg("scale")=1.0f,
			py::arg("overlay")=false,
			py::arg("layer")=0)
		.def("add", static_cast<shared_ptr<RenderNode> (Window::*)(
            const shared_ptr<Lines>&,
            const Vec3&,
			const Vec3&,
			float,
            bool, 
            int)>(&Window::add<Lines>),
			py::arg("object"),
			py::arg("t")=Vec3(0,0,0),
			py::arg("r")=Vec3(0,0,0),
			py::arg("scale")=1.0f,
			py::arg("overlay")=false,
			py::arg("layer")=0)
		.def("add", static_cast<shared_ptr<RenderNode> (Window::*)(
            const shared_ptr<Particle>&,
            const Vec3&,
			const Vec3&,
			float,
            bool, 
            int)>(&Window::add<Particle>),
			py::arg("object"),
			py::arg("t")=Vec3(0,0,0),
			py::arg("r")=Vec3(0,0,0),
			py::arg("scale")=1.0f,
			py::arg("overlay")=false,
			py::arg("layer")=0)
		.def("add", static_cast<shared_ptr<RenderNode> (Window::*)(
            const shared_ptr<Sprite>&,
            const Vec3&,
			const Vec3&,
			float,
            bool, 
            int)>(&Window::add<Sprite>),
			py::arg("object"),
			py::arg("t")=Vec3(0,0,0),
			py::arg("r")=Vec3(0,0,0),
			py::arg("scale")=1.0f,
			py::arg("overlay")=false,
			py::arg("layer")=0)
		.def("add", static_cast<shared_ptr<RenderNode> (Window::*)(
            const shared_ptr<Mesh>&,
            const Vec3&,
			const Vec3&,
			float,
            bool, 
            int)>(&Window::add<Mesh>),
			py::arg("object"),
			py::arg("t")=Vec3(0,0,0),
			py::arg("r")=Vec3(0,0,0),
			py::arg("scale")=1.0f,
			py::arg("overlay")=false,
			py::arg("layer")=0)
		.def("add", static_cast<shared_ptr<RenderNode> (Window::*)(
            const shared_ptr<ObjMesh>&,
            const Vec3&,
			const Vec3&,
			float,
            bool, 
            int)>(&Window::add<ObjMesh>),
			py::arg("object"),
			py::arg("t")=Vec3(0,0,0),
			py::arg("r")=Vec3(0,0,0),
			py::arg("scale")=1.0f,
			py::arg("overlay")=false,
			py::arg("layer")=0)
		.def("add", static_cast<shared_ptr<RenderNode> (Window::*)(
            const shared_ptr<Plane>&,
            const Vec3&,
			const Vec3&,
			float,
            bool, 
            int)>(&Window::add<Plane>),
			py::arg("object"),
			py::arg("t")=Vec3(0,0,0),
			py::arg("r")=Vec3(0,0,0),
			py::arg("scale")=1.0f,
			py::arg("overlay")=false,
			py::arg("layer")=0)
		.def("add", static_cast<shared_ptr<RenderNode> (Window::*)(
            const shared_ptr<AmbientLight>&,
            const Vec3&,
			const Vec3&,
			float,
            bool, 
            int)>(&Window::add<AmbientLight>),
			py::arg("object"),
			py::arg("t")=Vec3(0,0,0),
			py::arg("r")=Vec3(0,0,0),
			py::arg("scale")=1.0f,
			py::arg("overlay")=false,
			py::arg("layer")=0)
		.def("add", static_cast<shared_ptr<RenderNode> (Window::*)(
            const shared_ptr<DirLight>&,
            const Vec3&,
			const Vec3&,
			float,
            bool, 
            int)>(&Window::add<DirLight>),
			py::arg("object"),
			py::arg("t")=Vec3(0,0,0),
			py::arg("r")=Vec3(0,0,0),
			py::arg("scale")=1.0f,
			py::arg("overlay")=false,
			py::arg("layer")=0);

    py::enum_<Window::TrajectoryCameraMotionMode>(window, "TrajectoryCameraMotionMode")
        .value("SINGLE", Window::TrajectoryCameraMotionMode::SINGLE)
        .value("OSCILLATE", Window::TrajectoryCameraMotionMode::OSCILLATE)
        .value("LOOP", Window::TrajectoryCameraMotionMode::LOOP)
        .export_values();

    py::class_<Window::CameraTrajectoryNode>(window, "CameraTrajectoryNode")
        .def(py::init<float, Vec3, Vec3, Vec3>(),
             py::arg("time"),
             py::arg("to_point"),   // The argument name is changed here for compatability with python reserved names
             py::arg("from_point"), // The argument name is changed here for compatability with python reserved names
             py::arg("up_hint"))
        .def_readwrite("time", &Window::CameraTrajectoryNode::time)
        .def_readwrite("to", &Window::CameraTrajectoryNode::to)
        .def_readwrite("from", &Window::CameraTrajectoryNode::from)
        .def_readwrite("up_hint", &Window::CameraTrajectoryNode::up_hint);

    // Defined here because it requires the nested enum TrajectoryCameraMotionMode and struct CameraTrajectoryNode
    window
        .def("setTrajectoryCameraMode",
             &Window::setTrajectoryCameraMode,
             py::arg("trajectory"),
             py::arg("motion_mode") = Window::TrajectoryCameraMotionMode::SINGLE,
             py::arg("speed") = 1.0F,
             py::arg("start_time") = 0.0F,
             py::arg("polynomial_degree") = 3);

	py::class_<Plot>(m, "Plot")
        .def(py::init<string,int,int>(),
			py::arg("title")="plot",
			py::arg("width")=800,
			py::arg("height")=800)
		.def("window", &Plot::window, py::return_value_policy::reference)
		.def("title", &Plot::title)
		.def("backColor", [](Plot& p, const Vec3& color){
				p.backColor(color);
			},
			py::arg("color")
		)
		.def("foreColor", [](Plot& p, const Vec3& color){
				p.foreColor(color);
			},
			py::arg("color")
		)
		.def("gridColor", [](Plot& p, const Vec3& color){
				p.gridColor(color);
			},
			py::arg("color")
		)
		.def("darkMode", &Plot::darkMode)
		.def("colorCycle", &Plot::colorCycle)
		.def("xlabel", &Plot::xlabel)
		.def("ylabel", &Plot::ylabel)
		.def("yflip", &Plot::yflip, py::arg("equal")=true)
		.def("equal", &Plot::equal, py::arg("equal")=true)
		.def("textScale", &Plot::textScale)
		.def("limits", &Plot::limits, py::arg("min_x"), py::arg("max_x"), py::arg("min_y"), py::arg("max_y"))
		.def("label", [](Plot& p, 
			const string &text, 
			const Vec3& color,
			const string &marker_name){
				p.label(text, color, marker_name);
			}, py::arg("text"), py::arg("color"), py::arg("marker")="circle")
		.def("cursorCallout", &Plot::cursorCallout, py::arg("enabled"))
		.def("setLegendTopRight", &Plot::setLegendTopRight)
		.def("setLegendTopLeft", &Plot::setLegendTopLeft)
		.def("setLegendBottomRight", &Plot::setLegendBottomRight)
		.def("setLegendBottomLeft", &Plot::setLegendBottomLeft)
		.def("plot", [](Plot& p, 
				const MatX& x, 
				const MatX& y, 
				const Vec3& color,
				const string& linestyle,
				float thickness_in_screen_perc,
				const string& label){
				PlotLineOptions opts;
				opts.color = color;
				opts.label = label;
				opts.linestyle = linestyle;
				opts.thickness_in_screen_perc = thickness_in_screen_perc;
				if (y.rows() == 0)
				{
					if (x.cols() == 1)
					{
						p.plot(x.col(0),opts);
					}
					else
					{
						p.plot(x.col(0),x.col(1),opts);
					}
				}
				else
					p.plot(x,y,opts);
			},
			py::arg("x"), 
			py::arg("y")=MatX(), 
			py::arg("color")=Vec3(-1,-1,-1), 
			py::arg("linestyle") = "",
			py::arg("thickness_in_screen_perc") = 0,
			py::arg("label") = "")
		.def("scatter", [](Plot& p, 
				const MatX& x, 
				const MatX& y,
				float size_in_screen_perc, 
				const Vec3& color, 
				const string& marker,
				const string& label){
				PlotScatterOptions opts;
				opts.size_in_screen_perc = size_in_screen_perc;
				opts.color = color;
				opts.label = label;
				if (marker == "circle")
					opts.marker = PlotScatterMarker::CIRCLE;
				else if (marker == "diamond")
					opts.marker = PlotScatterMarker::DIAMOND;
				else if (marker == "plus")
					opts.marker = PlotScatterMarker::PLUS;
				else if (marker == "square")
					opts.marker = PlotScatterMarker::SQUARE;
				else if (marker == "star" || marker == "*")
					opts.marker = PlotScatterMarker::STAR;
				else
					throw std::runtime_error("Invalid marker type: " + marker);
				if (y.rows() == 0)
				{
					if (x.cols() == 1)
					{
						p.scatter(x.col(0),opts);
					}
					else
					{
						p.scatter(x.col(0),x.col(1),opts);
					}
				}
				else
					p.scatter(x,y,opts);
			}, 
			py::arg("x"), 
			py::arg("y")=MatX(),
			py::arg("size_in_screen_perc")=1.0,
			py::arg("color")=Vec3(-1,-1,-1),
			py::arg("marker")="circle",
			py::arg("label") = "")
		.def("text", [](Plot& p, const string& text, float size, const Vec2& offset, const Vec3& color){
			p.text(text, size, offset, color);
		})
		.def("polygon", [](Plot& p,
				const MatX& verts,
				const Vec3& color,
				float thickness
			){
				if (verts.cols() != 2)
				{
					throw std::runtime_error("Polygon verts matrix must have 2 columns.");
				}

				vector<Vec2> inner_verts;
				for (int r = 0; r < verts.rows(); r++)
					inner_verts.push_back(verts.row(r));

				p.polygon(inner_verts, color, thickness);
			},
			py::arg("verts"),
			py::arg("color"),
			py::arg("thickness")=0
		)
		.def("polygon", [](Plot& p,
			const MatX& verts,
			const Vec4& color,
			float thickness
			){
				if (verts.cols() != 2)
				{
					throw std::runtime_error("Polygon verts matrix must have 2 columns.");
				}

				vector<Vec2> inner_verts;
				for (int r = 0; r < verts.rows(); r++)
					inner_verts.push_back(verts.row(r));

				p.polygon(inner_verts, color, thickness);
			},
			py::arg("verts"),
			py::arg("color"),
			py::arg("thickness")=0
		)
		.def("circle", [](Plot& p, const Vec2& center, float rad, const Vec3& color, float thickness, int slices){
				p.circle(center, rad, color, thickness, slices);
			},
			py::arg("center"),
			py::arg("rad"),
			py::arg("color"),
			py::arg("thickness")=0,
			py::arg("slices")=16
		)
		.def("arrow", [](Plot& p, const Vec2& start, const Vec2& end, const Vec3& color, float radius){
				p.arrow(start, end, color, radius);
			},
			py::arg("start"),
			py::arg("end"),
			py::arg("color"),
			py::arg("radius")=1.0
		)
		.def("arrowhead", [](Plot& p, const Vec2& center, float heading, const Vec3& color, float radius){
				p.arrowhead(center, heading, color, radius);
			},
			py::arg("center"),
			py::arg("heading"),
			py::arg("color"),
			py::arg("radius")=1.0
		)
		.def("rect", &Plot::rect)
		.def("line", [](Plot& p, const Vec2& start, const Vec2& end, const Vec3& color, float thickness){
				p.line(start, end, color, thickness);
			},
			py::arg("start"),
			py::arg("end"),
			py::arg("color"),
			py::arg("thickness")=0
		)
		.def("line", [](Plot& p,
				const MatX& verts,
				const Vec3& color,
				float thickness
			){
				if (verts.cols() != 2)
				{
					throw std::runtime_error("Line verts matrix must have 2 columns.");
				}

				vector<Vec2> inner_verts;
				for (int r = 0; r < verts.rows(); r++)
					inner_verts.push_back(verts.row(r));

				p.line(inner_verts, color, thickness);
			},
			py::arg("verts"),
			py::arg("color"),
			py::arg("thickness")=0
		)
		.def("imshow", [](Plot& p,
			py::buffer buf,
			bool nearest,
			const Vec2& offset,
			float scale){
				py::buffer_info info = buf.request();
				
				// Image debug info
				/*v4print info.ndim;
				for (int i = 0; i < info.ndim; i++)
				{
					v4print "\t", info.shape[i], info.strides[i];
				}
				v4print info.itemsize, info.size, info.format;*/
				
				int n_channels = 1;
				if (info.shape.size() > 2)
					n_channels = info.shape[2];
				Image<uint8_t> image(info.shape[1], info.shape[0], n_channels);
				
				if (info.format == py::format_descriptor<uint8_t>::format())
					image.copyFromBuffer(reinterpret_cast<const char*>(info.ptr), info.size);
				else if (info.format == py::format_descriptor<double>::format())
				{
					const double* ptr = reinterpret_cast<const double*>(info.ptr);
					
					for (int i = 0; i < info.shape[0]; i++)
					{
						for (int j = 0; j < info.shape[1]; j++)
						{
							const double* vec_ptr = ptr + i * info.shape[1] * n_channels + j * n_channels;
							image(j,i) = Vec3u(vec_ptr[0]*255, vec_ptr[1]*255, vec_ptr[2]*255);
						}
					}
				}
				else
				{
					throw std::runtime_error("imshow only supports uint8 or double typed arrays.");
				}
				
				p.imshow(image, nearest, offset, scale);
			},
			py::arg("image"), py::arg("nearest")=false, py::arg("offset")=Vec2::Zero(), py::arg("scale")=1.0)
		.def("show", [](Plot& p, bool wait_close, bool wait_key){
			return p.show(wait_close, wait_key, [](){
				if (PyErr_CheckSignals() != 0)
                	throw py::error_already_set();
			});
		}, py::arg("wait_close")=true, py::arg("wait_key")=false)
		.def("clear", &Plot::clear)
		.def("save", &Plot::save)
		.def("setKeyPressCallback", &Plot::setKeyPressCallback)
		.def("setMouseClickCallback", &Plot::setMouseClickCallback);

	py::class_<Plot3D>(m, "Plot3D")
        .def(py::init<string,int,int>(),
			py::arg("title")="plot",
			py::arg("width")=800,
			py::arg("height")=800)
		.def("window", &Plot3D::window, py::return_value_policy::reference)
		.def("title", &Plot3D::title)
		.def("backColor", [](Plot3D& p, const Vec3& color){
				p.backColor(color);
			},
			py::arg("color")
		)
		.def("foreColor", [](Plot3D& p, const Vec3& color){
				p.foreColor(color);
			},
			py::arg("color")
		)
		.def("gridColor", [](Plot3D& p, const Vec3& color){
				p.gridColor(color);
			},
			py::arg("color")
		)
		.def("plot", [](Plot3D& p, 
				const MatX& x, 
				const MatX& y, 
				const MatX& z, 
				const Vec3& color,
				const string& linestyle,
				float thickness_in_screen_perc,
				const string& label){
				PlotLineOptions opts;
				opts.color = color;
				opts.label = label;
				opts.linestyle = linestyle;
				opts.thickness_in_screen_perc = thickness_in_screen_perc;
				if (x.cols() == 2)
				{
					p.plot(x.col(0),x.col(1),x.col(2),opts);
				}
				else
				{
					p.plot(x,y,z,opts);
				}
			},
			py::arg("x"), 
			py::arg("y")=MatX(),
			py::arg("z")=MatX(),
			py::arg("color")=Vec3(-1,-1,-1), 
			py::arg("linestyle") = "",
			py::arg("thickness_in_screen_perc") = 0,
			py::arg("label") = "")
		.def("colorCycle", &Plot3D::colorCycle)
		.def("xlabel", &Plot3D::xlabel)
		.def("ylabel", &Plot3D::ylabel)
		.def("zlabel", &Plot3D::zlabel)
		.def("show", [](Plot3D& p, bool wait_close, bool wait_key){
			return p.show(wait_close, wait_key, [](){
				if (PyErr_CheckSignals() != 0)
                	throw py::error_already_set();
			});
		}, py::arg("wait_close")=true, py::arg("wait_key")=false)
		.def("clear", &Plot3D::clear)
		.def("save", &Plot3D::save);


#ifdef VERSION_INFO
    m.attr("__version__") = MACRO_STRINGIFY(VERSION_INFO);
#else
    m.attr("__version__") = "dev";
#endif
}