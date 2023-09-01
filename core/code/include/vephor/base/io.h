#pragma once

#include "math.h"
#include "json.h"
#include <filesystem>

#include "thirdparty/gpakosz/whereami/whereami.h"

namespace vephor
{
	
namespace fs = std::filesystem;

inline string getDateString()
{
	auto t = std::time(nullptr);
    auto tm = *std::localtime(&t);
	std::ostringstream oss;
	oss << std::put_time(&tm, "%Y_%m_%d_%H_%M_%S");
	return oss.str();
}

inline string getTempDir()
{
#if defined(_WIN32)
	string temp_dir = "c:/Users/Public/AppData/Local/Temp";
#else
	string temp_dir = "/tmp";
#endif

	temp_dir += "/vephor/tmp";

	static string temp_date_str;
	if (temp_date_str.empty())
	{
		temp_date_str = getDateString();
		
		string mod_date_str = temp_date_str;
		int index = 1;
		while (fs::exists(temp_dir + "/" + mod_date_str))
		{
			index++;
			mod_date_str = temp_date_str + "_" + std::to_string(index);
		}
		temp_date_str = mod_date_str;

		v4print "Temp folder:", temp_dir + "/" + temp_date_str;
	}
	temp_dir += "/" + temp_date_str;

	if (!fs::exists(temp_dir))
	{
		fs::create_directories(temp_dir);
	}
	return temp_dir;
}

inline string getSaveDir()
{
#if defined(_WIN32)
	string temp_dir = "c:/Users/Public/AppData/Local/Temp";
#else
	string temp_dir = "/tmp";
#endif

	temp_dir += "/vephor/save";

	static string save_date_str;
	if (save_date_str.empty())
	{
		save_date_str = getDateString();
		
		string mod_date_str = save_date_str;
		int index = 1;
		while (fs::exists(temp_dir + "/" + mod_date_str))
		{
			index++;
			mod_date_str = save_date_str + "_" + std::to_string(index);
		}
		save_date_str = mod_date_str;

		v4print "Save folder:", temp_dir + "/" + save_date_str;
	}

	temp_dir += "/" + save_date_str;
	
	static int save_index = 1;
	if (save_index > 1)
		temp_dir += "_" + std::to_string(save_index);
	save_index++;

	if (!fs::exists(temp_dir))
	{
		fs::create_directories(temp_dir);
	}
	return temp_dir;
}

inline string getBaseDir()
{
	char buffer[1024];
	int length = 0;
	wai_getExecutablePath(buffer, 1024, &length);
	buffer[length] = '\0';

	string base_dir = buffer;

	if (!fs::exists(base_dir + "/share/vephor/assets"))
	{
		base_dir = fs::path(buffer).parent_path();

		if (!fs::exists(base_dir + "/share/vephor/assets"))
		{
			wai_getModulePath(buffer, 1024, &length);
			buffer[length] = '\0';

			base_dir = fs::path(buffer).parent_path();
		}
	}

	v4print "Base dir:", base_dir, "Executable dir:", buffer;

	return base_dir;
}

inline string getBaseAssetDir()
{
	return getBaseDir() + "/share/vephor";
}

inline json produceVertDataList(const MatX& verts)
{
	return {
		{"type", "list"},
		{"list", toJson(verts)},
		{"size", {verts.rows(), verts.cols()}}
	};
}

inline json produceVertDataRaw(const MatX& verts, vector<vector<char>>* bufs)
{
	int buf_id = bufs->size();
	vector<char> buf;
	const uint8_t* data_start = reinterpret_cast<const uint8_t*>(verts.data());
	uint64_t data_len = verts.size()*sizeof(float);
	buf.assign(data_start, data_start + data_len);
	bufs->push_back(buf);
	
	return {
		{"type", "raw"},
		{"buf", buf_id},
		{"size", {verts.rows(), verts.cols()}}
	};
}

inline json produceVertDataBase64(const MatX& verts)
{
	string verts_string = macaron::Base64::Encode(reinterpret_cast<const uint8_t*>(verts.data()), verts.size()*sizeof(float));
	
	return {
		{"type", "base64"},
		{"data", verts_string},
		{"size", {verts.rows(), verts.cols()}}
	};
}

struct VertexDataRecord
{
	MatX verts;
	const char* buf = NULL;
	int buf_size = 0;
	vector<uint8_t> base64_bytes;
	MatXMap map = MatXMap(NULL,0,0);
};

inline void readVertexData(const json& obj, int base_buf_index, const vector<vector<char>>& bufs, VertexDataRecord& record, json* serial_header = NULL, vector<vector<char>>* serial_bufs = NULL)
{
	if (obj["type"] == "list")
	{
		// TODO: test
		record.verts.resize((int)obj["size"][0], (int)obj["size"][1]);
		for (int i = 0; i < obj["list"].size(); i++)
		{
			auto v = obj["list"][i];
			for (int d = 0; d < v.size(); d++)
				record.verts(d,i) = v[d];
		}
		
		new (&record.map) MatXMap(record.verts.data(), record.verts.rows(), record.verts.cols());
		
		return;
	}
	
	if (obj["type"] == "base64")
	{
		macaron::Base64::Decode(obj["data"], record.base64_bytes);

		record.buf = reinterpret_cast<const char*>(record.base64_bytes.data());
		record.buf_size = record.base64_bytes.size();
	}
	else if (obj["type"] == "raw")
	{
		size_t buf_id = obj["buf"];
		buf_id += base_buf_index;
		const auto& buf = bufs[buf_id];

		record.buf = reinterpret_cast<const char*>(buf.data());
		record.buf_size = buf.size();
		
		if (serial_header && serial_bufs)
		{
			(*serial_header)["buf"] = serial_bufs->size();
			serial_bufs->push_back(buf);
		}
	}
	
	new (&record.map) MatXMap(reinterpret_cast<const float*>(record.buf), obj["size"][0], obj["size"][1]);
}

struct TextureDataRecord
{
	string path;
	Image<uint8_t> tex;
	bool filter_nearest = false;
};

inline json produceTextureData(const TextureDataRecord& tex_data, vector<vector<char>>* bufs)
{
	json data;
	data["filter_nearest"] = tex_data.filter_nearest;
	if (tex_data.tex.getSize()[0] > 0)
	{
		const char* buf_data;
		int buf_size;
		tex_data.tex.getBuffer(buf_data, buf_size);

		data["size"] = toJson(tex_data.tex.getSize());
		data["channels"] = tex_data.tex.getChannels();
			
		if (bufs)
		{
			int buf_id = bufs->size();
			vector<char> buf;
			buf.assign(buf_data, buf_data + buf_size);
			bufs->push_back(buf);
			data["type"] = "raw";
			data["buf"] = buf_id;
		}
		else
		{
			string tex_string = macaron::Base64::Encode(reinterpret_cast<const uint8_t*>(buf_data), buf_size);
			data["type"] = "base64";
			data["data"] = tex_string;
		}
	}
	else if (!tex_data.path.empty())
	{
		string temp_dir = getTempDir();
		
		if (!fs::exists(temp_dir+"/scene_assets"))
		{
			v4print "Creating dir:", temp_dir+"/scene_assets";
			fs::create_directory(temp_dir+"/scene_assets");
		}
		
		string final_path = temp_dir+"/scene_assets/"+fs::path(tex_data.path).filename().string();
		if (!fs::exists(final_path))
			fs::copy(tex_data.path, final_path);
		
		string final_stub = "scene_assets/"+fs::path(tex_data.path).filename().string();

		data["type"] = "file";
		data["path"] = final_stub;
	}
	else
		data["type"] = "none";
	return data;
}

}