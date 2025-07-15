/**
 * Copyright 2025
 * Carnegie Robotics, LLC
 * 4501 Hatfield Street, Pittsburgh, PA 15201
 * https://www.carnegierobotics.com
 *
 * This source code is licensed under the Apache License, Version 2.0
 * found in the LICENSE file in the root directory of this source tree.
**/

#pragma once

#include "vephor.h"
#include <filesystem>

namespace vephor
{
namespace ogl
{

namespace fs = std::filesystem;

class AssetManager
{
public:
	void addFolder(const string& path)
	{
		v4print "Adding asset path:", path;
		folders.push_back(path);
	}
	string getAssetPath(const string& path)
	{
		if (path.empty())
			throw std::runtime_error("Could not locate empty asset.");
		
		for (const auto& folder : folders)
		{
			string full_path = folder + "/" + path;
			if (fs::exists(full_path))
				return full_path;
		}
		throw std::runtime_error("Could not locate asset: " + path);
		return "";
	}
private:
	vector<string> folders;
};

} // namespace ogl
} // namespace vephor