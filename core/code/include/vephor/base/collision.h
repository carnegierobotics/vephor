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

#include "math.h"
#include "spatial_hash.h"

namespace vephor
{

struct SolidLine
{
	Vec3 pt;
	Vec3 slopes;
	float length;
};

struct Solid
{
	MatX verts;
	vector<Vec4> planes;
};

inline void addPlane(
	vector<Vec4>& planes, 
	const Vec4& new_plane, 
	float min_dist_sq)
{	
	bool found = false;
	for (const auto& plane : planes)
	{
		Vec4 vec = plane - new_plane;
		float dist_sq = vec.dot(vec);
		
		if (dist_sq < min_dist_sq)
		{
			found = true;
			break;
		}
	}
	
	if (!found)
	{
		planes.push_back(new_plane);
	}
}

inline void addNormal(
	vector<Vec3>& normals, 
	const Vec3& new_normal, 
	float min_dist_sq)
{	
	bool found = false;
	for (const auto& normal : normals)
	{
		Vec3 vec = normal - new_normal;
		float dist_sq = vec.dot(vec);
		
		if (dist_sq < min_dist_sq)
		{
			found = true;
			break;
		}
	}
	
	if (!found)
	{
		normals.push_back(new_normal);
	}
}

inline Solid createSolidFromTris(const MatX& verts)
{
	// Find matching sets of verts
	vector<int> vert_index_to_first_index;
	vert_index_to_first_index.resize(verts.rows());
	
	const float min_vert_dist_sq = pow(1e-2, 2.0f);
	
	for (int r = 0; r < verts.rows(); r++)
	{
		bool found = false;
		for (int r2 = 0; r2 < r; r2++)
		{
			Vec3 vec = (verts.row(r) - verts.row(r2)).transpose();
			float dist_sq = vec.dot(vec);
			if (dist_sq < min_vert_dist_sq)
			{
				found = true;
				vert_index_to_first_index[r] = r2;
				break;
			}
		}
		
		if (!found)
		{
			vert_index_to_first_index[r] = r;
		}
	}
	
	
	Solid s;
	s.verts = verts;
	
	const float min_plane_dist_sq = pow(1e-2, 2.0f);
	const float min_normal_dist_sq = pow(1e-2, 2.0f);
	
	//vector<pair<Vec3, Vec3>> lines;
	unordered_map<int, vector<Vec3>> vert_normals;
	unordered_map<Vec2i, vector<Vec3>> line_normals;
	for (int i = 0; i < verts.rows(); i += 3)
	{
		//addLine(lines, verts.row(i), verts.row(i+1), min_line_endpoint_dist_sq);
		//addLine(lines, verts.row(i+1), verts.row(i+2), min_line_endpoint_dist_sq);
		//addLine(lines, verts.row(i+2), verts.row(i), min_line_endpoint_dist_sq);
		
		Vec4 plane;
		Vec3 vec1 = (verts.row(i+1) - verts.row(i)).transpose();
		Vec3 vec2 = (verts.row(i+2) - verts.row(i)).transpose();
		plane.head<3>() = vec1.cross(vec2);
		plane.head<3>() /= plane.head<3>().norm();
		plane[3] = -plane.head<3>().dot(verts.row(i));
		addPlane(s.planes, plane, min_plane_dist_sq);
		
		int v0 = vert_index_to_first_index[i+0];
		int v1 = vert_index_to_first_index[i+1];
		int v2 = vert_index_to_first_index[i+2];
		
		addNormal(vert_normals[v0], plane.head<3>(), min_normal_dist_sq);
		addNormal(vert_normals[v1], plane.head<3>(), min_normal_dist_sq);
		addNormal(vert_normals[v2], plane.head<3>(), min_normal_dist_sq);
		
		Vec2i line_01(v0, v1);
		Vec2i line_12(v1, v2);
		Vec2i line_20(v2, v0);
		
		if (line_01[0] > line_01[1])
			swap(line_01[0], line_01[1]);
		if (line_12[0] > line_12[1])
			swap(line_12[0], line_12[1]);
		if (line_20[0] > line_20[1])
			swap(line_20[0], line_20[1]);
		
		addNormal(line_normals[line_01], plane.head<3>(), min_normal_dist_sq);
		addNormal(line_normals[line_12], plane.head<3>(), min_normal_dist_sq);
		addNormal(line_normals[line_20], plane.head<3>(), min_normal_dist_sq);
	}
	
	for (const auto& v : vert_normals)
	{
		Vec3 normal(0,0,0);
		
		for (const auto& vn : v.second)
		{
			normal += vn;
		}
		
		//v4print "Vert normal:", v.first, v.second.size(), (normal / normal.norm()).transpose();
		
		Vec4 plane;
		plane.head<3>() = normal;
		plane.head<3>() /= plane.head<3>().norm();
		plane[3] = -plane.head<3>().dot(verts.row(v.first));
		addPlane(s.planes, plane, min_plane_dist_sq);
	}
	
	for (const auto& l : line_normals)
	{
		Vec3 normal(0,0,0);
		
		for (const auto& ln : l.second)
		{
			normal += ln;
		}
		
		//v4print "Line normal:", l.first.transpose(), l.second.size(), (normal / normal.norm()).transpose();
		
		Vec4 plane;
		plane.head<3>() = normal;
		plane.head<3>() /= plane.head<3>().norm();
		plane[3] = -plane.head<3>().dot(verts.row(l.first[0]));
		addPlane(s.planes, plane, min_plane_dist_sq);
	}
	
	/*for (const auto& line : lines)
	{
		Vec3 vec = line.second - line.first;
		float length = vec.norm();
		s.lines.push_back({line.first, vec / length, length});
	}*/
	
	return s;
}

inline Vec4 calcPlane(const Vec3& normal, const Vec3& pt_on)
{
	Vec4 plane;
	plane.head<3>() = normal;
	plane.head<3>() /= plane.head<3>().norm();
	plane[3] = -plane.head<3>().dot(pt_on);
	return plane;
}

inline vector<pair<Vec3, Solid>> createSolidsFromHeightMap(const MatX& heights, float res)
{
	vector<pair<Vec3, Solid>> solids;
	
	Vec2 center(heights.rows()/2 * res, heights.cols()/2 * res);
	
	for (int r = 0; r < heights.rows() - 1; r++)
    {
        float x = r * res - center[0];

        for (int c = 0; c < heights.cols() - 1; c++)
        {
            float y = c * res - center[1];

            Vec3 v00(x, y, -heights(r,c));
            Vec3 v10(x+res, y, -heights(r+1,c));
            Vec3 v01(x, y+res, -heights(r,c+1));
            Vec3 v11(x+res, y+res, -heights(r+1,c+1));
			
			Vec3 cell_center(x+res/2, y+res/2, (v00[2]+v10[2]+v01[2]+v11[2])/4);
			
			{
				Solid s;
				
				s.planes.push_back(calcPlane(Vec3(1,0,0), v11 - cell_center));
				s.planes.push_back(calcPlane(Vec3(0,-1,0), v00 - cell_center));
				s.planes.push_back(calcPlane(Vec3(-0.7071,0.7071,0), v00 - cell_center));
				s.planes.push_back(calcPlane((v11-v00).cross(v10-v00), v00 - cell_center));
			
				//v4print (v10-v00).cross(v11-v00).transpose();
				
				/*for (auto& plane : s.planes)
				{
					plane *= -1;
				}*/
			
				solids.push_back(pair<Vec3, Solid>(cell_center, s));
			}
			
			{
				Solid s;
				
				s.planes.push_back(calcPlane(Vec3(-1,0,0), v00 - cell_center));
				s.planes.push_back(calcPlane(Vec3(0,1,0), v11 - cell_center));
				s.planes.push_back(calcPlane(Vec3(0.7071,-0.7071,0), v00 - cell_center));
				s.planes.push_back(calcPlane((v01-v00).cross(v11-v00), v00 - cell_center));
				
				/*for (auto& plane : s.planes)
				{
					plane *= -1;
				}*/
				
				solids.push_back(pair<Vec3, Solid>(cell_center, s));
			}
		}
	}
	
	return solids;
}

inline float calcSphereSolidPushDist(
	const Vec3& sphere_pt,
	float radius,
	const Solid& s,
	Vec3& push_dir
)
{	
	float max_dist = std::numeric_limits<float>::lowest();
	for (const auto& plane : s.planes)
	{
		float dist = plane.head<3>().dot(sphere_pt) + plane[3] - radius;
		
		if (dist > max_dist)
		{
			max_dist = dist;
			push_dir = plane.head<3>();
		}
	}
	
	/*if (max_dist < 0)
	{
		v4print sphere_pt.transpose(), max_dist;
		for (const auto& plane : s.planes)
		{
			float dist = plane.head<3>().dot(sphere_pt) + plane[3] - radius;
			v4print "\t", plane.transpose(), dist;
		}
	}*/
		
	return max_dist;
}

}