#include "vephor_ext.h"

using namespace vephor;

int main()
{
	Window window;
	
	window.setServerMode();
	
	double obj_mesh_dist = 5.0f;
	
	
	// Make a "clock"
	for (int i = 1; i <= 12; i++)
	{
		float angle = i / 12.0 * 2 * M_PI;
		
		Vec3 dir = Vec3(cos(angle), sin(angle), 0);
		auto text = make_shared<Text>(std::to_string(i));
		text->setAnchorCentered();
		window.add(text, dir * obj_mesh_dist * 1.5);
		
		vector<Vec3> verts = {
			Vec3(0,0,0),
			dir * obj_mesh_dist * 1.5
		};
		window.add(make_shared<Lines>(verts), Vec3::Zero());
	}
	
	// Use a mesh for the clock hand
	string base_asset_dir = getBaseAssetDir();
	auto obj_mesh = make_shared<ObjMesh>(base_asset_dir+"/assets/pyramid.obj");
	auto obj_mesh_node = window.add(obj_mesh, Transform3(Vec3(obj_mesh_dist,0,0)));
	
	float angle = 0;
	while (true)
	{
		angle += 0.01;
		obj_mesh_node->setPos(Vec3(cos(angle), sin(angle), 0) * obj_mesh_dist);
		obj_mesh_node->setOrient(Vec3(0,0,angle+M_PI));
		
		window.render(false /*wait*/);
		
		std::this_thread::sleep_for(std::chrono::milliseconds((int)(33)));
	}

	return 0;
}