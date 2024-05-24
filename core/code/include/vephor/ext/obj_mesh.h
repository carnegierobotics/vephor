/**
 * Copyright 2023
 * Carnegie Robotics, LLC
 * 4501 Hatfield Street, Pittsburgh, PA 15201
 * https://www.carnegierobotics.com
 *
 * This code is provided under the terms of the Master Services Agreement (the Agreement).
 * This code constitutes CRL Background Intellectual Property, as defined in the Agreement.
**/

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
				string final_mtl_path = temp_dir + "/scene_assets/" + mtl_file;
				if (!fs::exists(final_mtl_path))
				{
					v4print "Copying material", mtl_path, "to", final_mtl_path;
					fs::copy(mtl_path, final_mtl_path);

					std::ifstream infile(mtl_path);

					std::string line;
					while (std::getline(infile, line))
					{
						auto pos = line.find("map_Kd ");
						if (pos != std::string::npos)
						{
							std::string texture_file = line.substr(pos + 7);
							trim(texture_file);
							string texture_path = fs::path(path).parent_path().string() + "/" + texture_file;
							string final_texture_path = temp_dir + "/scene_assets/" + texture_file;
							v4print "Copying texture", texture_path, "to", final_texture_path;
							fs::copy(texture_path, final_texture_path);
						}
					}
				}
			}
		}
		
		string final_path = temp_dir+"/scene_assets/"+fs::path(path).filename().string();
		if (!fs::exists(final_path))
		{
			v4print "Copying obj", path, "to", final_path;
			fs::copy(path, final_path);
		}

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