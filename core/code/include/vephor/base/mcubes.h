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

#include "tensor.h"
#include "geometry.h"
#include "union_find.h"

namespace vephor
{

namespace mcubes
{

inline bool initialized = false;

enum class EdgeIndex : uint16_t
{
	EDGE_000_100 = 0, // X edges
	EDGE_010_110 = 1,
	EDGE_001_101 = 2,
	EDGE_011_111 = 3,
	EDGE_000_010 = 4, // Y edges
	EDGE_001_110 = 5,
	EDGE_001_011 = 6,
	EDGE_101_111 = 7,
	EDGE_000_001 = 8, // Z edges
	EDGE_100_101 = 9,
	EDGE_010_011 = 10,
	EDGE_110_111 = 11
}; 

struct EdgeInfo
{
	uint16_t id;
	uint8_t axis;
	Vec2i corner_inds;
	Vec3 center_vert;
};

inline const array<EdgeInfo, 12> edge_infos = {
	EdgeInfo{   1,0,{ 0, 1}, { 0,-1,-1}},
	EdgeInfo{   2,0,{ 2, 3}, { 0, 1,-1}},
	EdgeInfo{   4,0,{ 4, 5}, { 0,-1, 1}},
	EdgeInfo{   8,0,{ 6, 7}, { 0, 1, 1}},
	EdgeInfo{  16,1,{ 0, 2}, {-1, 0,-1}},
	EdgeInfo{  32,1,{ 1, 3}, { 1, 0,-1}},
	EdgeInfo{  64,1,{ 4, 6}, {-1, 0, 1}},
	EdgeInfo{ 128,1,{ 5, 7}, { 1, 0, 1}},
	EdgeInfo{ 256,2,{ 0, 4}, {-1,-1, 0}},
	EdgeInfo{ 512,2,{ 1, 5}, { 1,-1, 0}},
	EdgeInfo{1024,2,{ 2, 6}, {-1, 1, 0}},
	EdgeInfo{2048,2,{ 3, 7}, { 1, 1, 0}}
};

inline const array<Vec3i, 8> corner_offsets = {
	Vec3i(0,0,0),
	Vec3i(1,0,0),
	Vec3i(0,1,0),
	Vec3i(1,1,0),
	Vec3i(0,0,1),
	Vec3i(1,0,1),
	Vec3i(0,1,1),
	Vec3i(1,1,1)
};

struct TriInfo
{
    uint16_t pattern_id = 0;
    vector<Vec3i> indices;
};

inline std::vector<TriInfo> tri_per_case;

inline void addTriangle(TriInfo& tri_info, uint8_t edge_index_1, uint8_t edge_index_2, uint8_t edge_index_3, const Vec3& normal_sum)
{
	const Vec3& v1 = edge_infos[edge_index_1].center_vert;
	const Vec3& v2 = edge_infos[edge_index_2].center_vert;
	const Vec3& v3 = edge_infos[edge_index_3].center_vert;
	
	Vec3 v_normal = (v3 - v1).cross(v2 - v1);

	if (v_normal.dot(normal_sum) < 0)
	{
		tri_info.indices.push_back({edge_index_1, edge_index_2, edge_index_3});
	}
	else
	{
		tri_info.indices.push_back({edge_index_1, edge_index_3, edge_index_2});
	}
}

inline vector<size_t> formTravelingSalesmanLoopBruteForce(const vector<Vec3>& pts)
{
	MatX dists(pts.size(), pts.size());
	dists.fill(0.0f);
	
	for (size_t i = 0; i < pts.size(); i++)
	{
		for (size_t j = i + 1; j < pts.size(); j++)
		{
			float dist = (pts[i] - pts[j]).norm();
			dists(i,j) = dist;
			dists(j,i) = dist;
		}
	}
	
	vector<size_t> curr_perm(pts.size());
	for (size_t i = 0; i < pts.size(); i++)
		curr_perm[i] = i;
	
	float min_dist = std::numeric_limits<float>::max();
	vector<size_t> best_perm;
	do{
		float dist = 0.0f;
		for (size_t i = 0; i < pts.size(); i++)
		{
			dist += dists(curr_perm[i], curr_perm[(i+1)%pts.size()]);
		}
		if (dist < min_dist)
		{
			min_dist = dist;
			best_perm = curr_perm;
		}
	}while(std::next_permutation(curr_perm.begin(), curr_perm.end()));
	
	return best_perm;
}

inline void init()
{
	// Form a list of triangles for each case, with references to which edge each vertex comes from
	v4print "Marching cubes init...";
	
	tri_per_case.resize(256);

	for (uint16_t id = 0; id < 256; id++)
	{
		//v4print "Id:", (int)id;
		
		uint8_t inv_id = ~id;
		
		// Find connected groups of verts with 1s
		// Collect all edges with a vert in that group where the other vert is a 0
		UnionFind<int> occ_groups;
		UnionFind<int> unocc_groups;
		
		for (uint8_t index = 0; index < 8; index++)
		{
			uint8_t vert_id = 1 << index;
			
			if (id & vert_id)
				occ_groups.addEdge(index, index);
			else
				unocc_groups.addEdge(index, index);
		}
		
		for (const auto& edge : edge_infos)
		{
			uint8_t vert_id_1 = 1 << edge.corner_inds[0];
			uint8_t vert_id_2 = 1 << edge.corner_inds[1];
			
			if (id & vert_id_1 && id & vert_id_2)
			{
				occ_groups.addEdge(edge.corner_inds[0], edge.corner_inds[1]);
			}
			
			if (inv_id & vert_id_1 && inv_id & vert_id_2)
			{
				unocc_groups.addEdge(edge.corner_inds[0], edge.corner_inds[1]);
			}
		}
		
		vector<Vec2i> diag_edges = {
			Vec2i(0b000, 0b011),
			Vec2i(0b001, 0b010),
			Vec2i(0b100, 0b111),
			Vec2i(0b101, 0b110),
			Vec2i(0b000, 0b101),
			Vec2i(0b001, 0b100),
			Vec2i(0b010, 0b111),
			Vec2i(0b011, 0b110),
			Vec2i(0b000, 0b110),
			Vec2i(0b100, 0b010),
			Vec2i(0b001, 0b111),
			Vec2i(0b101, 0b011)
		};
		
		for (const auto& edge : diag_edges)
		{
			uint8_t vert_id_1 = 1 << edge[0];
			uint8_t vert_id_2 = 1 << edge[1];
			
			if (id & vert_id_1 && id & vert_id_2)
			{
				occ_groups.addEdge(edge[0], edge[1]);
			}
		}
		
		/*unordered_map<int, unordered_set<int>> occ_group_set;
		unordered_map<int, unordered_set<int>> unocc_group_set;
		for (uint8_t index = 0; index < 8; index++)
		{
			uint8_t vert_id = 1 << index;
			
			if (id & vert_id)
				occ_group_set[0].insert(index);
			else
				unocc_group_set[0].insert(index);
		}*/
		
		TriInfo tri_info;
		
		// For each occupied group
		//for (const auto& occ_group : occ_group_set)
		for (const auto& occ_group : occ_groups.getSets())
		{
			// For each unoccupied group
			struct ConnEdgeInfo
			{
				uint8_t index;
				Vec3 normal;
			};
			
			//for (const auto& unocc_group : unocc_group_set)
			for (const auto& unocc_group : unocc_groups.getSets())
			{	
				vector<ConnEdgeInfo> conn_edges;

				for (uint8_t edge_index = 0; edge_index < edge_infos.size(); edge_index++)
				{
					const auto& edge_info = edge_infos[edge_index];
					const auto& edge = edge_info.corner_inds;
				
					Vec3 normal = Vec3::Zero();
					normal[edge_info.axis] = 1;
					
					if ((find(occ_group.second, edge[0]) && find(unocc_group.second, edge[1])))
					{
						conn_edges.push_back(ConnEdgeInfo{edge_index, normal});
					}
					else if ((find(occ_group.second, edge[1]) && find(unocc_group.second, edge[0])))
					{
						conn_edges.push_back(ConnEdgeInfo{edge_index, -normal});
					}
				}
			
			
				for (const auto& edge : conn_edges)
				{
					tri_info.pattern_id |= edge_infos[edge.index].id;
				}
				
				if (conn_edges.size() < 3)
					continue;
				
				vector<Vec3> conn_edge_pts;
				for (size_t i = 0; i < conn_edges.size(); i++)
				{
					conn_edge_pts.push_back(edge_infos[conn_edges[i].index].center_vert);
				}
				auto index_loop = formTravelingSalesmanLoopBruteForce(conn_edge_pts);
				
				for (size_t i = 0; i < index_loop.size() - 2; i++)
				{
					uint8_t ind1 = index_loop[0];
					uint8_t ind2 = index_loop[i+1];
					uint8_t ind3 = index_loop[i+2];
					
					addTriangle(
						tri_info,
						conn_edges[ind1].index,
						conn_edges[ind2].index,
						conn_edges[ind3].index,
						conn_edges[ind1].normal + conn_edges[ind2].normal + conn_edges[ind3].normal	
					);
				}
			}
		}
		
		tri_per_case[id] = tri_info;
	}
	
	initialized = true;
	v4print "Marching cubes init complete.";
}
	
inline void calcCellSurfaces(const array<float, 8>& cell_occ, float thresh, 
	float cell_size, const Vec3& corner, 
	vector<Vec3>& verts, vector<Vec3>& normals)
{
	uint8_t occ_code = 0;
	for (int i = 7; i >= 0; i--)
	{
		occ_code <<= 1;
		occ_code |= (cell_occ[i] > thresh);
	}
	
	const auto& tri_info = tri_per_case[occ_code];
	
	/*if (occ_code > 0)
	{
		v4print (int)occ_code, corner.transpose(), cell_occ;
		for (const auto& tri : tri_info.indices)
		{
			v4print "\tTri:", tri.transpose();
		}
	}*/
	
	Vec3 edge_pts[12];
	
	for (size_t edge_index = 0; edge_index < 12; edge_index++)
	{
		const auto& info = edge_infos[edge_index];
		if (tri_info.pattern_id & info.id)
		{
			float diff = cell_occ[info.corner_inds[1]] - cell_occ[info.corner_inds[0]];
			
			if (fabs(diff) < 1e-3)
			{
				v4print "Zero diff edge found.";
				v4print "Occ code:", (int)occ_code;
				v4print "Edge pattern:", tri_info.pattern_id;
				for (const auto& tri : tri_info.indices)
				{
					v4print "Tri:", tri.transpose();
				}
				v4print "Edge:", edge_index;
				v4print "Occ:", cell_occ;
				
				std::exit(1);
			}
			
			float perc = (thresh - cell_occ[info.corner_inds[0]]) / diff;
			edge_pts[edge_index] = corner + corner_offsets[info.corner_inds[0]].cast<float>() * cell_size;
			edge_pts[edge_index][info.axis] += cell_size * perc;
			
			//v4print "Edge pt:", edge_index, "C:", corner.transpose(), "E:", edge_pts[edge_index].transpose(), "P:", perc, 
			//	diff, cell_occ[info.corner_inds[0]], cell_occ[info.corner_inds[1]], info.corner_inds[0], info.corner_inds[1];
		}
	}
	
	for (const auto& tri : tri_info.indices)
	{
		for (int i = 0; i < 3; i++)
		{
			verts.push_back(edge_pts[tri[i]]);
			
			for (int j = 0; j < 3; j++)
			{
				if (edge_pts[tri[i]][j] != edge_pts[tri[i]][j])
				{
					v4print "Invalid value in vertex:", edge_pts[tri[i]].transpose();
					std::exit(1);
				}
			}
		}
		const Vec3& v0 = edge_pts[tri[0]];//*(verts.rbegin() - 2);
		const Vec3& v1 = edge_pts[tri[1]];//*(verts.rbegin() - 1);
		const Vec3& v2 = edge_pts[tri[2]];//*(verts.rbegin() - 0);
		Vec3 norm = (v1 - v0).cross(v2 - v0);
		norm /= norm.norm();
		normals.push_back(norm);
		normals.push_back(norm);
		normals.push_back(norm);
		//v4print "Norm:", norm.transpose(), v0.transpose(), v1.transpose(), v2.transpose();
	}
}

inline void calcSurfaces(const Tensor<3, float>& occupancy, float thresh, float cell_size, MeshData& data)
{
	if (!initialized)
		init();
	
	if (occupancy.size()[0] <= 1 ||
		occupancy.size()[1] <= 1 ||
		occupancy.size()[2] <= 1)
	{
		return;
	}
	
	auto start_time = std::chrono::high_resolution_clock::now();
		
	TensorIndex<3> size = occupancy.size() - Vec3i(1,1,1);
	auto iter = TensorIter<3>(size);
	
	v4print "Size:", size.transpose();
	
	Vec3 center = (occupancy.size().cast<float>() - Vec3(1,1,1))/2.0f * cell_size;
	
	array<float, 8> cell_occ;
	vector<Vec3> verts;
	vector<Vec3> normals;
	while(!iter.atEnd())
	{
		auto ind = iter.getIndex();
		
		for (size_t i = 0; i < 8; i++)
		{
			cell_occ[i] = occupancy(ind + corner_offsets[i]);
		}
		
		Vec3 corner(ind[0]*cell_size, ind[1]*cell_size, ind[2]*cell_size);
		corner -= center;
		
		calcCellSurfaces(cell_occ, thresh, cell_size, corner, verts, normals);
		
		iter++;
	}
	
	if (verts.size() != normals.size())
	{
		v4print "mcubes: Verts and normals not of equal size.";
		std::exit(1);
	}
	
	data.verts.resize(verts.size(), 3);
    data.uvs.resize(verts.size(), 2);
	data.norms.resize(verts.size(), 3);

    for (size_t i = 0; i < verts.size(); i++)
    {
		data.verts.row(i) = verts[i];
		data.uvs.row(i) = Vec2::Zero();
        data.norms.row(i) = normals[i];
    }
	
	auto end_time = std::chrono::high_resolution_clock::now();

	float elapsed_time_ms = std::chrono::duration<float, std::milli>(end_time-start_time).count();
	
	v4print "Marching cubes time (ms):", elapsed_time_ms;
}

}

}