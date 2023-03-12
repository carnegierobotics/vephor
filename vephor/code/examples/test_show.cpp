#include <iostream>
#include "vephor_ext.h"
#include <thread>
#include <unistd.h>

using namespace vephor;

void usage(char* argv[])
{
	v4print "Usage:", argv[0], "\n";
	v4print "\t-m <mode>";
	v4print "\t-o <host>";
	v4print "\t-p <port>";
	v4print "\t-d";
}

int main(int argc, char* argv[])
{
	string mode = "file";
	string host = "localhost";
	int port = VEPHOR_DEFAULT_PORT;
	bool wait = true;
	
	int opt;
    while((opt = getopt(argc, argv, "dhm:o:p:")) != -1) 
    {
		switch(opt)
		{
		case 'd':
			wait = false;
			break;
		case 'h':
			usage(argv);
			break;
		case 'm': 
			mode = optarg;
			break;
		case 'o': 
			host = optarg;
			break;
		case 'p': 
			port = std::atoi(optarg);
			break;
		default:
			usage(argv);
			break;
		}			
	}
	
	bool use_net = false;
	bool use_client = false;
	bool use_client_byos = false;
	bool use_server = false;
	bool use_server_byoc = false;
	bool save = false;

	save = mode == string("save");
	use_client = mode == string("client");
	use_client_byos = mode == string("client_byos");
	use_server = mode == string("server");
	use_server_byoc = mode == string("server_byoc");
	use_net = use_client || use_client_byos || use_server || use_server_byoc;

	Window window;
	if (use_client)
		window.setClientMode(wait, host, port);
	else if (use_client_byos)
		window.setClientModeBYOS();
	else if (use_server)
		window.setServerMode(wait, port);
	else if (use_server_byoc)
		window.setServerModeBYOC();
	
	auto lines = make_shared<Lines>(
		vector<Vec3>{
			{-5,0,0},
			{5,0,0},
			{0,-5,0},
			{0,5,0}
		}
	);
	lines->setColor(Vec3(1,1,0));
	lines->setLineStrip(false);
	window.add(lines, Transform3(Vec3(0,0,-1), Vec3::Zero()));
	
	auto grid = make_shared<Grid>(10);
	window.add(grid, Transform3());
	
	auto axes = make_shared<Axes>();
	window.add(axes, Transform3());
	
	vector<Vec3> pts;
	vector<Vec4> colors;
	
	int parts_per_side = 500;
	for (int i = 0; i <= parts_per_side; i++)
	for (int j = 0; j <= parts_per_side; j++)
	{
		float x_perc = i/(float)parts_per_side;
		float y_perc = j/(float)parts_per_side;
		float z_perc = 1.0f-1.0f/(1.0f+exp(-(-5+100*pow(0.5-x_perc,2.0f)+100*pow(0.5-y_perc,2.0f))));
		
		pts.push_back(Vec3(-10+20*x_perc,-10+20*y_perc,3-2*z_perc));
		colors.push_back(Vec4(x_perc,(1-x_perc*0.5-y_perc*0.5),y_perc,1));
	}
		
	auto particle = make_shared<Particle>(
		pts,
		colors
	);
	particle->setSize(0.025f);
	window.add(particle, Transform3());

	{
		auto text = make_shared<Text>("Corner");
		text->setColor(Vec3(0,1,1));
		window.add(text, TransformSim3(Vec3(10,10,3), Vec3::Zero(), 0.25f), false, 1);
	}
	
	{
		auto text = make_shared<Text>("Corner");
		text->setColor(Vec3(0,1,1));
		window.add(text, TransformSim3(Vec3(-10,10,3), Vec3::Zero(), 0.25f), false, 1);
	}
	
	{
		auto text = make_shared<Text>("Corner");
		text->setColor(Vec3(0,1,1));
		window.add(text, TransformSim3(Vec3(-10,-10,3), Vec3::Zero(), 0.25f), false, 1);
	}
	
	{
		auto text = make_shared<Text>("Corner");
		text->setColor(Vec3(0,1,1));
		window.add(text, TransformSim3(Vec3(10,-10,3), Vec3::Zero(), 0.25f), false, 1);
	}
	
	auto top_node = window.add(Vec3(0,0,-2.5));
	
	auto cone = make_shared<Cone>();
	cone->setColor(Vec3(1,0,0));
	window.add(cone, Transform3(Vec3(0,0,0), Vec3(M_PI,0,0)))->setParent(top_node);
	
	auto cylinder = make_shared<Cylinder>();
	cylinder->setColor(Vec3(0,1,0));
	window.add(cylinder, Transform3(Vec3(0,5,0), Vec3(M_PI,0,0)))->setParent(top_node);
	
	auto sphere = make_shared<Sphere>();
	sphere->setColor(Vec3(0,0,1));
	window.add(sphere, Transform3(Vec3(5,0,0)))->setParent(top_node);
	
	auto cube = make_shared<Cube>();
	cube->setColor(Vec3(0,1,1));
	window.add(cube, Transform3(Vec3(-5,0,0)))->setParent(top_node);
	
	auto plane = make_shared<Plane>(Vec2(10,10));
	
	string base_asset_dir = getBaseAssetDir();
	
	plane->setTexture(base_asset_dir+"/assets/maze.png", true);
	window.add(plane, Transform3(Vec3(0,0,-0.5)));
	
	auto circle = make_shared<Circle>(1.0,0.25);
	circle->setColor(Vec3(1,0,1));
	window.add(circle, Transform3(Vec3(0,-5,0)))->setParent(top_node);
	
	auto arrow = make_shared<Arrow>(Vec3(5,5,0), Vec3(2,2,0), 0.5);
	arrow->setColor(Vec3(1,1,0));
	window.add(arrow, Transform3())->setParent(top_node);

	float obj_mesh_dist = 10.0f;
	auto obj_mesh = make_shared<ObjMesh>(base_asset_dir+"/assets/pyramid.obj");
	auto obj_mesh_node = window.add(obj_mesh, Transform3(Vec3(obj_mesh_dist,0,-2.5)));

	{
		auto text = make_shared<Text>("Bottom Right");
		text->setColor(Vec3(1,0,1));
		text->setAnchorBottomRight();
		window.add(text, TransformSim3(Vec3(-50,50,0), Vec3::Zero(), 25), true)->setParent(window.getWindowBottomRightNode());
	}
	
	{
		auto text = make_shared<Text>("Top Left");
		text->setColor(Vec3(1,0,1));
		text->setAnchorTopLeft();
		window.add(text, TransformSim3(Vec3(50,-50,0), Vec3::Zero(), 25), true)->setParent(window.getWindowTopLeftNode());
	}
	
	{
		auto text = make_shared<Text>("Bottom Left");
		text->setColor(Vec3(1,0,1));
		text->setAnchorBottomLeft();
		window.add(text, TransformSim3(Vec3(50,50,0), Vec3::Zero(), 25), true)->setParent(window.getWindowBottomLeftNode());
	}
	
	{
		auto text = make_shared<Text>("Top Right");
		text->setColor(Vec3(1,0,1));
		text->setAnchorTopRight();
		window.add(text, TransformSim3(Vec3(-50,-50,0), Vec3::Zero(), 25), true)->setParent(window.getWindowTopRightNode());
	}

	{
		auto sprite = make_shared<Sprite>(base_asset_dir+"/assets/world.png", true);
		window.add(sprite, TransformSim3(Vec3(-100,-175,0), Vec3::Zero(), 175), true, 0)->setParent(window.getWindowTopRightNode());
	}

	bool new_object_added = false;
	bool new_object_removed = false;

	shared_ptr<RenderNode> arrow_2_node;

	float time = 0;
	float dt = 0.015f;
	
	if (save)
		window.save(getSaveDir());
	else
		window.render(!use_net);
	
	if (use_net)
	{	
		while (true)
		{
			if (!new_object_added && time > 5.0f)
			{
				auto arrow_2 = make_shared<Arrow>(3.0, 0.5);
				arrow_2->setColor(Vec3(0.5,0.5,0.5));
				arrow_2_node = window.add(arrow_2, Transform3(Vec3(-5,-5,-2.5-3)));

				v4print "Adding object", arrow_2_node->getID();
				
				new_object_added = true;
			}
			
			if (!new_object_removed && time > 10.0f)
			{
				arrow_2_node->setDestroy();

				v4print "Destroying object", arrow_2_node->getID();
				
				new_object_removed = true;
			}
			
			float angle = time;
			obj_mesh_node->setPos(Vec3(obj_mesh_dist*cos(angle),obj_mesh_dist*sin(angle),-2.5f));
			obj_mesh_node->setOrient(Vec3(0,0,angle+M_PI));
			top_node->setOrient(Vec3(0,0,-angle*0.5));
			if (!window.render(!use_net))
				break;
			
			time += dt;
			std::this_thread::sleep_for(std::chrono::milliseconds((int)(dt*1000)));
		}
	}
	
	v4print "Exiting test_show.";

	return 0;
}