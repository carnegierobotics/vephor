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

#ifdef USE_EXTERNAL_JSON
#include "nlohmann/json.hpp"
#else
#include "thirdparty/nlohmann/json.hpp"
#endif

namespace vephor{

using json = nlohmann::json;

inline json toJson(const Vec2i& v)
{
	return {v[0], v[1]};
}

inline json toJson(const Vec2& v)
{
	return {v[0], v[1]};
}

inline json toJson(const Vec3& v)
{
	return {v[0], v[1], v[2]};
}

inline json toJson(const Vec3u& v)
{
	return {v[0], v[1], v[2]};
}

inline json toJson(const Vec4& v)
{
	return {v[0], v[1], v[2], v[3]};
}

inline json toJson(const Transform3& T)
{
	Eigen::AngleAxisf aa;
    aa.fromRotationMatrix(T.rotation());
	
	const Vec3& t = T.translation();
	Vec3 r = aa.axis() * aa.angle();
	
	return {
		{"t", toJson(t)},
		{"r", toJson(r)}
	};
}

inline json toJson(const TransformSim3& T)
{
	Eigen::AngleAxisf aa;
    aa.fromRotationMatrix(T.rotation());
	
	const Vec3& t = T.translation();
	Vec3 r = aa.axis() * aa.angle();
	
	return {
		{"t", toJson(t)},
		{"r", toJson(r)},
		{"scale", T.scale}
	};
}

template <typename T>
inline json toJson(const vector<T>& v)
{
	json data;
	
	for (const auto& item : v)
	{
		data.push_back(toJson(item));
	}
	
	return data;
}

inline json toJson(const MatX& m)
{
	json data;
	
	for (int r = 0; r < m.rows(); r++)
	{
		json row;
		for (int c = 0; c < m.cols(); c++)
		{
			if (std::isnan(m(r,c)))
				throw std::runtime_error("Value in MatX toJson is nan: ("+std::to_string(r)+", "+std::to_string(c)+")");
			json val = m(r,c);
			if (val.is_null())
				throw std::runtime_error("Value in MatX toJson is null: ("+std::to_string(r)+", "+std::to_string(c)+")");
			row.push_back(val);
		}
		data.push_back(row);
	}
	
	return data;
}

inline Vec2i readVec2i(const json& data)
{
    return Vec2i(data[0], data[1]);
}

inline Vec2 readVec2(const json& data)
{
    return Vec2(data[0], data[1]);
}

inline Vec3 readVec3(const json& data)
{
    return Vec3(data[0], data[1], data[2]);
}

inline Vec4 readVec4(const json& data)
{
    return Vec4(data[0], data[1], data[2], data[3]);
}

inline Transform3 readTransform3(const json& data)
{
    Vec3 t(0,0,0);
    Vec3 r(0,0,0);

    if (data.contains("t"))
        t = readVec3(data["t"]);

    if (data.contains("r"))
        r = readVec3(data["r"]);

    return Transform3(t,r);
}

inline TransformSim3 readTransformSim3(const json& data)
{
    Vec3 t(0,0,0);
    Vec3 r(0,0,0);
	float scale = 1.0f;

	if (data.contains("t"))
		t = readVec3(data["t"]);
	
	if (data.contains("r"))
	    r = readVec3(data["r"]);
	
	if (data.contains("scale"))
		scale = data["scale"];

    return TransformSim3(t,r,scale);
}

inline MatX readMatX(const json& data)
{
	vector<vector<float>> cells;
	
	for (int r = 0; r < data.size(); r++)
	{
		const auto& row = data[r];
		vector<float> row_cells;
		for (int c = 0; c < row.size(); c++)
		{
			const auto& col = row[c];

			try
			{
				row_cells.push_back(col);
			}
			catch (...)
			{
				v4print "Invalid number:", col, "at", r, c;
				throw;
			}
		}
		cells.push_back(row_cells);
	}
	
	if (cells.empty())
		return MatX();
	
	MatX m(cells.size(), cells[0].size());
	
	for (int r = 0; r < m.rows(); r++)
	{
		for (int c = 0; c < m.cols(); c++)
		{
			m(r,c) = cells[r][c];
		}
	}
	
	return m;
}

template <typename T, typename S>
T convert_from(const S& s)
{
	return (T)s;
}

template <>
inline Vec2 convert_from<Vec2, json>(const json& s)
{
	return readVec2(s);
}

template <>
inline Vec3 convert_from<Vec3, json>(const json& s)
{
	return readVec3(s);
}

template <>
inline Vec4 convert_from<Vec4, json>(const json& s)
{
	return readVec4(s);
}

template <typename T>
T readDefault(const json& data, const string& name, const T& default_value)
{
	if (data.contains(name))
		return convert_from<T, json>(data[name]);
	return default_value;
}

}