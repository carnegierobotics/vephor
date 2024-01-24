/**
 * Copyright 2023
 * Carnegie Robotics, LLC
 * 4501 Hatfield Street, Pittsburgh, PA 15201
 * https://www.carnegierobotics.com
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the Carnegie Robotics, LLC nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL CARNEGIE ROBOTICS, LLC BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
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