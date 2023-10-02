#pragma once

#include "window.h"

#include <cctype>

namespace vephor
{

class ObjMesh
{
public:
    ObjMesh(const string& p_path)
	: path(p_path)
	{
	}
	void setColor(const Vec3& p_color){color = p_color;}
	json serialize(vector<vector<char>>*) const
	{
		string temp_dir = getTempDir();

		v4print "Looking for materials in obj:", path;
		
		// Find any used materials
		// TODO: copy textures too
		ifstream fin(path);

		if (!fs::exists(temp_dir+"/scene_assets"))
			fs::create_directory(temp_dir+"/scene_assets");

		string line;
		while (std::getline(fin, line))
		{
			trim(line);
			auto index = line.find("mtllib");
			if (index != string::npos)
			{
				string mtl_file = line.substr(index + 7);
				v4print "\tMaterial file:", mtl_file;
				string mtl_path = fs::path(path).parent_path().string() + "/" + mtl_file;
				string final_mtl_path = temp_dir+"/scene_assets/" + mtl_file;
				if (!fs::exists(final_mtl_path))
					fs::copy(mtl_path, final_mtl_path);		
			}
		}
		
		string final_path = temp_dir+"/scene_assets/"+fs::path(path).filename().string();
		if (!fs::exists(final_path))
			fs::copy(path, final_path);

		return {
            {"type", "obj_mesh"},
            {"path", "scene_assets/"+fs::path(path).filename().string()},
			{"color_rgb", toJson(color)}
        };
	}
private:
	string path;
	Vec3 color = Vec3(1,1,1);
};

}