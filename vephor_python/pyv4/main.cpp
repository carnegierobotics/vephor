#include <vephor.h>
#include <vephor_ext.h>
#include <pybind11/pybind11.h>
#include <pybind11/eigen.h>
#include <pybind11/functional.h>

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
    py::class_<Transform3>(m, "Transform3")
		.def(py::init<>())
		.def(py::init<const Vec3&,const Vec3>(),py::arg("t"),py::arg("r")=Vec3(0,0,0));
    py::class_<TransformSim3>(m, "TransformSim3")
		.def(py::init<>())
		.def(py::init<const Vec3&,const Vec3,float>(),py::arg("t"),py::arg("r")=Vec3(0,0,0),py::arg("scale")=1.0f);
    py::class_<TransformNode, shared_ptr<TransformNode>>(m, "TransformNode");
    py::class_<RenderNode, shared_ptr<RenderNode>>(m, "RenderNode")
		.def("setPos", &RenderNode::setPos)
		.def("getPos", &RenderNode::getPos)
		.def("setOrient", &RenderNode::setOrient)
		.def("setOrient", [](const shared_ptr<RenderNode>& node, const Vec3& r){node->setOrient(Orient3(r));})
		.def("getOrient", &RenderNode::getOrient)
		.def("setScale", &RenderNode::setScale)
		.def("getScale", &RenderNode::getScale)
		.def("setParent", static_cast<void (RenderNode::*)(const shared_ptr<RenderNode>&)>(&RenderNode::setParent))
		.def("setParent", static_cast<void (RenderNode::*)(const shared_ptr<TransformNode>&)>(&RenderNode::setParent))
		.def("setShow", &RenderNode::setShow)
		.def("getShow", &RenderNode::getShow)
		.def("setDestroy", &RenderNode::setDestroy)
		.def("getDestroy", &RenderNode::getDestroy);

	m.def("clamp", &clamp);

	py::class_<ShowMetadata>(m, "ShowMetadata");

	py::class_<Axes, shared_ptr<Axes>>(m, "Axes")
        .def(py::init<float>(),py::arg("size")=1.0f);

    py::class_<Sphere, shared_ptr<Sphere>>(m, "Sphere")
        .def(py::init<float,int,int>(),py::arg("rad")=1.0f,py::arg("slices")=12,py::arg("stacks")=12);

    py::class_<Cylinder, shared_ptr<Cylinder>>(m, "Cylinder")
        .def(py::init<float,float,int>(),py::arg("rad")=1.0f,py::arg("height")=1.0f,py::arg("slices")=12);

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
        .def(py::init<float,float,int>(),py::arg("rad")=1.0f,py::arg("height")=1.0f,py::arg("slices")=12);

	py::class_<Grid, shared_ptr<Grid>>(m, "Grid")
        .def(py::init<float,Vec3,Vec3,float>(),py::arg("rad"),py::arg("normal")=Vec3(0,0,1),py::arg("right")=Vec3(1,0,0),py::arg("cell_size")=1.0f);

	py::class_<Text, shared_ptr<Text>>(m, "Text")
		.def(py::init<string>())
		.def("setColor",[](Text& t, 
			const Vec3& rgb){
				t.setColor(Color(rgb));
			}, 
			py::arg("rgb"))
		.def("setColor",[](Text& t, 
			const Vec4& rgba){
				t.setColor(Color(rgba));
			}, 
			py::arg("rgba"));

	py::class_<Lines, shared_ptr<Lines>>(m, "Lines")
		.def(py::init([](const MatX& verts, const MatX& colors){
			return make_shared<Lines>(verts, colors);
		}), py::arg("verts"), py::arg("colors")=MatX())
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
				
				v4print info.ndim;
				for (int i = 0; i < info.ndim; i++)
				{
					v4print "\t", info.shape[i], info.strides[i];
				}
				v4print info.itemsize, info.size, info.format;

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
					throw std::runtime_error("Sprite only supports uint8 or double typed arrays.");
				}

				return make_shared<Sprite>(image, nearest);
		}), py::arg("buf"), py::arg("nearest")=false);
	
    py::class_<Window>(m, "Window")
        .def(py::init<int,int,std::string>(),
			py::arg("width")=-1,
			py::arg("height")=-1,
			py::arg("name")="show")
		.def("clear", &Window::clear)
        .def("render", &Window::render, py::arg("wait_close")=true, py::arg("wait_key")=true)
		.def_static("setClientMode", &Window::setClientMode, 
			py::arg("wait")=false, 
			py::arg("host")="localhost", 
			py::arg("port")=2001,
			py::arg("record_also")=false,
			py::arg("record_path")="")
		.def_static("setClientModeBYOS", &Window::setClientModeBYOS, 
			py::arg("record_also")=false,
			py::arg("record_path")="")
		.def_static("setServerMode", &Window::setServerMode, 
			py::arg("wait")=false, 
			py::arg("port")=2001,
			py::arg("record_also")=false,
			py::arg("record_path")="",
			py::arg("show_metadata")=ShowMetadata())
		.def_static("setServerModeBYOC", &Window::setServerModeBYOC, 
			py::arg("record_also")=false,
			py::arg("record_path")="")
		.def("getWindowBottomRightNode", &Window::getWindowBottomRightNode)
		.def("getWindowTopLeftNode", &Window::getWindowTopLeftNode)
		.def("getWindowBottomLeftNode", &Window::getWindowBottomLeftNode)
		.def("getWindowTopRightNode", &Window::getWindowTopRightNode)
		.def("setTrackballMode", &Window::setTrackballMode, 
			py::arg("to")=Vec3(0,0,0), 
			py::arg("from")=Vec3(-1,0,-1), 
			py::arg("up")=Vec3(0,0,-1), 
			py::arg("use_3d")=false)
		.def("setKeyPressCallback", &Window::setKeyPressCallback)
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
			py::arg("t"),
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
			py::arg("layer")=0);

	py::class_<Plot>(m, "Plot")
        .def(py::init<string>(),
			py::arg("title")="plot")
		.def("window", &Plot::window, py::return_value_policy::reference)
		.def("title", &Plot::title)
		.def("xlabel", &Plot::xlabel)
		.def("ylabel", &Plot::ylabel)
		.def("yflip", &Plot::yflip, py::arg("equal")=true)
		.def("equal", &Plot::equal, py::arg("equal")=true)
		.def("limits", &Plot::limits, py::arg("min_x"), py::arg("max_x"), py::arg("min_y"), py::arg("max_y"))
		.def("plot", [](Plot& p, 
				const VecX& x, 
				const MatX& y, 
				const Vec3& color,
				const string& linestyle,
				float thickness,
				const string& label){
				PlotLineOptions opts;
				opts.color = color;
				opts.label = label;
				opts.linestyle = linestyle;
				opts.thickness = thickness;
				p.plot(x,y,opts);
			}, 
			py::arg("x"), 
			py::arg("y"), 
			py::arg("color")=Vec3(-1,-1,-1), 
			py::arg("linestyle") = "",
			py::arg("thickness") = 0,
			py::arg("label") = "")
		.def("scatter", [](Plot& p, 
				const VecX& x, 
				const MatX& y,
				float size, 
				const Vec3& color, 
				const string& marker,
				const string& label){
				PlotScatterOptions opts;
				opts.size = size;
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
				else
					throw std::runtime_error("Invalid marker type: " + marker);
				p.scatter(x,y,opts);
			}, 
			py::arg("x"), 
			py::arg("y"),
			py::arg("size")=0.01,
			py::arg("color")=Vec3(-1,-1,-1),
			py::arg("marker")="circle",
			py::arg("label") = "")
		.def("text", [](Plot& p, const string& text, float size, const Vec2& offset, const Vec3& color){
			p.text(text, size, offset, color);
		})
		// TODO: polygon
		.def("circle", &Plot::circle)
		.def("rect", &Plot::rect)
		// TODO: line
		.def("imshow", [](Plot& p,
			py::buffer buf,
			bool nearest,
			const Vec2& offset){
				py::buffer_info info = buf.request();
				
				v4print info.ndim;
				for (int i = 0; i < info.ndim; i++)
				{
					v4print "\t", info.shape[i], info.strides[i];
				}
				v4print info.itemsize, info.size, info.format;
				
				Image<uint8_t> image(info.shape[1], info.shape[0], info.shape[2]);
				
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
					throw std::runtime_error("imshow only supports uint8 or double typed arrays.");
				}
				
				p.imshow(image, nearest, offset);
			},
			py::arg("image"), py::arg("nearest")=false, py::arg("offset")=Vec2::Zero())
		.def("show", &Plot::show, py::arg("wait_close")=true, py::arg("wait_key")=true)
		.def("clear", &Plot::clear);


#ifdef VERSION_INFO
    m.attr("__version__") = MACRO_STRINGIFY(VERSION_INFO);
#else
    m.attr("__version__") = "dev";
#endif
}