#pragma once

#include "window.h"

#include <cctype>

namespace vephor
{

//https://stackoverflow.com/questions/9358718/similar-function-in-c-to-pythons-strip
inline std::string strip(const std::string &inpt)
{
    auto start_it = inpt.begin();
    auto end_it = inpt.rbegin();
    while (std::isspace(*start_it))
        ++start_it;
    while (std::isspace(*end_it))
        ++end_it;
    return std::string(start_it, end_it.base());
}

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
		
		// Find any used materials
		// TODO: copy textures too
		ifstream fin(path);

		if (!fs::exists(temp_dir+"/scene_assets"))
			fs::create_directory(temp_dir+"/scene_assets");

		string line;
		while (std::getline(fin, line))
		{
			line = strip(line);
			auto index = line.find("mtllib");
			if (index != string::npos)
			{
				string mtl_file = line.substr(index + 7);
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