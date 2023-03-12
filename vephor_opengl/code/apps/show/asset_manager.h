#pragma once

#include "vephor.h"
#include <filesystem>

namespace fs = std::filesystem;
using namespace vephor;

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