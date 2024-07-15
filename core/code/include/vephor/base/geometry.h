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

#include "common.h"
#include "thirdparty/Bly7/obj_loader.h"

namespace vephor
{

struct MeshData
{
    MatX verts;
    MatX uvs;
    MatX norms;
	
	int curr_index = 0;
	
	MeshData(){}
	MeshData(
		const MatX& p_verts,
		const MatX& p_uvs,
		const MatX& p_norms
	)
	: verts(p_verts), uvs(p_uvs), norms(p_norms)
	{}
	MeshData(int n_verts)
	{
		verts.resize(n_verts, 3);
		uvs.resize(n_verts, 2);
		norms.resize(n_verts, 3);
	}
	void addQuad2D(
		const Vec2& lower, 
		const Vec2& upper, 
		const Vec2& lower_uv,
		const Vec2& upper_uv,
		bool flip_uv_corners = false
	)
	{
		if (curr_index + 6 > verts.rows() || curr_index + 6 > uvs.rows() || curr_index + 6 > norms.rows())
			throw std::runtime_error("Mesh data not large enough to contain Quad2D.");
		
		verts.row(curr_index) = Vec3(lower[0], lower[1], 0);
		uvs.row(curr_index) = Vec2(lower_uv[0], lower_uv[1]);
		norms.row(curr_index) = Vec3(0,0,-1);
		curr_index++;
		
		verts.row(curr_index) = Vec3(upper[0], upper[1], 0);
		uvs.row(curr_index) = Vec2(upper_uv[0], upper_uv[1]);
		norms.row(curr_index) = Vec3(0,0,-1);
		curr_index++;
		
		verts.row(curr_index) = Vec3(lower[0], upper[1], 0);
		if (flip_uv_corners)
			uvs.row(curr_index) = Vec2(upper_uv[0], lower_uv[1]);
		else
			uvs.row(curr_index) = Vec2(lower_uv[0], upper_uv[1]);
		norms.row(curr_index) = Vec3(0,0,-1);
		curr_index++;
		
		verts.row(curr_index) = Vec3(lower[0], lower[1], 0);
		uvs.row(curr_index) = Vec2(lower_uv[0], lower_uv[1]);
		norms.row(curr_index) = Vec3(0,0,-1);
		curr_index++;
		
		verts.row(curr_index) = Vec3(upper[0], lower[1], 0);
		if (flip_uv_corners)
			uvs.row(curr_index) = Vec2(lower_uv[0], upper_uv[1]);
		else
			uvs.row(curr_index) = Vec2(upper_uv[0], lower_uv[1]);
		norms.row(curr_index) = Vec3(0,0,-1);
		curr_index++;
		
		verts.row(curr_index) = Vec3(upper[0], upper[1], 0);
		uvs.row(curr_index) = Vec2(upper_uv[0], upper_uv[1]);
		norms.row(curr_index) = Vec3(0,0,-1);
		curr_index++;
	}
	void addLooseQuad2D(
		const Vec2& v00, 
		const Vec2& v10, 
		const Vec2& v11, 
		const Vec2& v01,  
		const Vec2& uv00 = Vec2(0,0), 
		const Vec2& uv10 = Vec2(1,0), 
		const Vec2& uv11 = Vec2(1,1), 
		const Vec2& uv01 = Vec2(0,1)
	)
	{
		if (curr_index + 6 > verts.rows() || curr_index + 6 > uvs.rows() || curr_index + 6 > norms.rows())
			throw std::runtime_error("Mesh data not large enough to contain LooseQuad2D.");
		
		verts.row(curr_index) = Vec3(v00[0], v00[1], 0);
		uvs.row(curr_index) = Vec2(uv00[0], uv00[1]);
		norms.row(curr_index) = Vec3(0,0,-1);
		curr_index++;
		
		verts.row(curr_index) = Vec3(v10[0], v10[1], 0);
		uvs.row(curr_index) = Vec2(uv10[0], uv10[1]);
		norms.row(curr_index) = Vec3(0,0,-1);
		curr_index++;
		
		verts.row(curr_index) = Vec3(v11[0], v11[1], 0);
		uvs.row(curr_index) = Vec2(uv11[0], uv11[1]);
		norms.row(curr_index) = Vec3(0,0,-1);
		curr_index++;
		
		verts.row(curr_index) = Vec3(v00[0], v00[1], 0);
		uvs.row(curr_index) = Vec2(uv00[0], uv00[1]);
		norms.row(curr_index) = Vec3(0,0,-1);
		curr_index++;
		
		verts.row(curr_index) = Vec3(v11[0], v11[1], 0);
		uvs.row(curr_index) = Vec2(uv11[0], uv11[1]);
		norms.row(curr_index) = Vec3(0,0,-1);
		curr_index++;
		
		verts.row(curr_index) = Vec3(v01[0], v01[1], 0);
		uvs.row(curr_index) = Vec2(uv01[0], uv01[1]);
		norms.row(curr_index) = Vec3(0,0,-1);
		curr_index++;
	}
};

inline vector<Vec2> cleanOrderedVerts(const vector<Vec2>& orig_verts, float dist_min = 1e-3)
{
	vector<Vec2> verts;

	if (orig_verts.empty())
		return verts;

	verts.reserve(orig_verts.size());

	verts.push_back(orig_verts[0]);

	for (size_t i = 1; i < orig_verts.size(); i++)
	{
		Vec2 diff = orig_verts[i] - *verts.rbegin();
		float dist = diff.norm();
		if (dist > dist_min)
			verts.push_back(orig_verts[i]);
	}

	return verts;
}

inline MeshData formLine(const vector<Vec2>& orig_verts, float rad)
{
	MeshData data;
	
	auto verts = cleanOrderedVerts(orig_verts);

	if (verts.size() < 2)
	{
		return data;
	}
	
	data.verts.resize(((verts.size()-1)*2)*3,3);
	data.norms.resize(((verts.size()-1)*2)*3,3);
	data.uvs.resize(((verts.size()-1)*2)*3,2);

	vector<Vec2> outer_miter_verts;
	vector<Vec2> inner_miter_verts;
	
	outer_miter_verts.reserve(verts.size());
	inner_miter_verts.reserve(verts.size());

	for (size_t i = 0; i < verts.size(); i++)
	{
		if (i == 0)
		{
			size_t next = i + 1;

			Vec2 v_i = verts[i];
			Vec2 v_next = verts[next];

			Vec2 dir1 = v_next - v_i;

			dir1 /= dir1.norm();

			Vec2 norm1(-dir1[1], dir1[0]);

			outer_miter_verts.push_back(v_i + norm1 * rad);
			inner_miter_verts.push_back(v_i - norm1 * rad);

			continue;
		}
		else if (i == verts.size() - 1)
		{
			size_t prev = (i + verts.size() - 1) % verts.size();

			Vec2 v_prev = verts[prev];
			Vec2 v_i = verts[i];

			Vec2 dir0 = v_i - v_prev;

			dir0 /= dir0.norm();

			Vec2 norm0(-dir0[1], dir0[0]);
			
			outer_miter_verts.push_back(v_i + norm0 * rad);
			inner_miter_verts.push_back(v_i - norm0 * rad);

			continue;
		}

		size_t prev = (i + verts.size() - 1) % verts.size();
		size_t next = (i + 1) % verts.size();

		Vec2 v_prev = verts[prev];
		Vec2 v_i = verts[i];
		Vec2 v_next = verts[next];

		Vec2 dir0 = v_i - v_prev;
		Vec2 dir1 = v_next - v_i;

		dir0 /= dir0.norm();
		dir1 /= dir1.norm();

		Vec2 norm0(-dir0[1], dir0[0]);
		Vec2 norm1(-dir1[1], dir1[0]);
		
		Vec2 tangent = dir0 + dir1;
		tangent /= tangent.norm();
		
		Vec2 miter(-tangent[1], tangent[0]);
		
		// TODO: check for edge cases - opposing lines will break this
		float len = rad / miter.dot(norm0);
		
		outer_miter_verts.push_back(v_i + miter * len);
		inner_miter_verts.push_back(v_i - miter * len);
	}
	
	for (size_t i = 0; i < verts.size() - 1; i++)
	{
		size_t next = i + 1;
		data.addLooseQuad2D(
			inner_miter_verts[next],
			inner_miter_verts[i],
			outer_miter_verts[i],
			outer_miter_verts[next]
		);
	}
	
	return data;
}

// Do not repeat the first vert for this method
inline MeshData formLineLoop(const vector<Vec2>& verts, float rad)
{
	MeshData data;
	
	data.verts.resize((verts.size()*2)*3,3);
	data.norms.resize((verts.size()*2)*3,3);
	data.uvs.resize((verts.size()*2)*3,2);

	vector<Vec2> outer_miter_verts;
	vector<Vec2> inner_miter_verts;
	
	outer_miter_verts.reserve(verts.size());
	inner_miter_verts.reserve(verts.size());

	for (size_t i = 0; i < verts.size(); i++)
	{
		size_t prev = (i + verts.size() - 1) % verts.size();
		size_t next = (i + 1) % verts.size();

		Vec2 v_prev = verts[prev];
		Vec2 v_i = verts[i];
		Vec2 v_next = verts[next];

		Vec2 dir0 = v_i - v_prev;
		Vec2 dir1 = v_next - v_i;

		dir0 /= dir0.norm();
		dir1 /= dir1.norm();

		Vec2 norm0(-dir0[1], dir0[0]);
		Vec2 norm1(-dir1[1], dir1[0]);
		
		Vec2 tangent = dir0 + dir1;
		tangent /= tangent.norm();
		
		Vec2 miter(-tangent[1], tangent[0]);
		
		// TODO: check for edge cases - opposing lines will break this
		float len = rad / miter.dot(norm0);
		
		outer_miter_verts.push_back(v_i + miter * len);
		inner_miter_verts.push_back(v_i - miter * len);
	}
	
	for (size_t i = 0; i < verts.size(); i++)
	{
		size_t next = (i + 1) % verts.size();
		data.addLooseQuad2D(
			inner_miter_verts[next],
			inner_miter_verts[i],
			outer_miter_verts[i],
			outer_miter_verts[next]
		);
	}
	
	return data;
}

inline float crossProduct(const Vec2& a, const Vec2& b)
{
	return a[0]*b[1] - a[1]*b[0];
}

inline bool isTriCCW(
	const Vec2& a,
	const Vec2& b,
	const Vec2& c
)
{
	return crossProduct(b-a,c-a) > 0;
}

inline bool isPolyCCW(const vector<Vec2>& verts)
{
	size_t last = verts.size() - 1;
	float sum = (verts[0][0] - verts[last][0]) * (verts[0][1] + verts[last][1]);
	
	for (size_t i = 1; i < verts.size(); i++)
	{
		sum += (verts[i][0] - verts[i-1][0]) * (verts[i][1] + verts[i-1][1]);
	}
	
	return sum < 0;
}

inline bool isPointInTriCCW(
	const Vec2& a,
	const Vec2& b,
	const Vec2& c,
	const Vec2& p)
{
	float cross_val_a = crossProduct(b - a, p - a);
	
	if (cross_val_a < 0)
		return false;
	
	float cross_val_b = crossProduct(c - b, p - b);
	
	if (cross_val_b < 0)
		return false;
	
	float cross_val_c = crossProduct(a - c, p - c);
	
	return cross_val_c >= 0;
}

inline bool isEar(size_t vert_index, const vector<Vec2>& verts)
{
	size_t prev_vert_index = (vert_index + verts.size() - 1) % verts.size();
	size_t next_vert_index = (vert_index + 1) % verts.size();
	
	const Vec2& v_0 = verts[prev_vert_index];
	const Vec2& v_1 = verts[vert_index];
	const Vec2& v_2 = verts[next_vert_index];
	
	if (!isTriCCW(v_0, v_1, v_2))
		return false;
	
	for (size_t i = 0; i < verts.size() - 3; i++)
	{
		// Start on the vertex after next vert
		size_t curr_vert_index = (vert_index + 2 + i) % verts.size();
		if (isPointInTriCCW(v_0, v_1, v_2, verts[curr_vert_index]))
		{
			return false;
		}
	}
	
	return true;
}

struct Triangle2
{
	Vec2 a, b, c;
};

inline MeshData formPolygon(vector<Vec2> verts)
{
	MeshData data;
	
	if (!isPolyCCW(verts))
	{
		vector<Vec2> new_verts(verts.size());
		
		for (size_t i = 0; i < verts.size(); i++)
		{
			new_verts[verts.size() - 1 - i] = verts[i];
		}
		
		verts = new_verts;
	}
	
	vector<Triangle2> tris;
	
	while (verts.size() > 3)
	{
		bool ear_found = false;
		for (size_t i = 0; i < verts.size(); i++)
		{
			if (isEar(i, verts))
			{
				tris.push_back({verts[(i+verts.size()-1)%verts.size()], verts[i], verts[(i+1)%verts.size()]});
				verts.erase(verts.begin() + i);
				ear_found = true;
				break;
			}
		}
		if (!ear_found)
		{
			throw std::runtime_error("Could not triangulate polygon.");
			return data;
		}
	}
	
	tris.push_back({verts[0], verts[1], verts[2]});
	
	data.verts.resize(tris.size()*3,3);
	data.norms.resize(tris.size()*3,3);
	
	int index = 0;
	for (const auto& tri : tris)
	{
		data.verts.row(index) = Vec3(tri.a[0], tri.a[1], 0);
		data.norms.row(index) = Vec3(0,0,-1);
		index++;
		
		data.verts.row(index) = Vec3(tri.b[0], tri.b[1], 0);
		data.norms.row(index) = Vec3(0,0,-1);
		index++;
		
		data.verts.row(index) = Vec3(tri.c[0], tri.c[1], 0);
		data.norms.row(index) = Vec3(0,0,-1);
		index++;
	}
	
	return data;
}

inline MeshData formCube()
{
	MeshData data;
	
    data.verts.resize(12*3,3);
    data.verts << 
        -1.0f,-1.0f,-1.0f,
		-1.0f,-1.0f, 1.0f,
		-1.0f, 1.0f, 1.0f,

		-1.0f,-1.0f,-1.0f,
		-1.0f, 1.0f, 1.0f,
		-1.0f, 1.0f,-1.0f,

		 1.0f,-1.0f, 1.0f,
		-1.0f,-1.0f, 1.0f,
		-1.0f,-1.0f,-1.0f,

		 1.0f,-1.0f, 1.0f,
		-1.0f,-1.0f,-1.0f,
		 1.0f,-1.0f,-1.0f,

		 1.0f, 1.0f,-1.0f,
		-1.0f,-1.0f,-1.0f,
		-1.0f, 1.0f,-1.0f,

		 1.0f, 1.0f,-1.0f,
		 1.0f,-1.0f,-1.0f,
		-1.0f,-1.0f,-1.0f,

		 1.0f, 1.0f, 1.0f,
		 1.0f,-1.0f,-1.0f,
		 1.0f, 1.0f,-1.0f,

		 1.0f,-1.0f,-1.0f,
		 1.0f, 1.0f, 1.0f,
		 1.0f,-1.0f, 1.0f,

		 1.0f, 1.0f, 1.0f,
		 1.0f, 1.0f,-1.0f,
		-1.0f, 1.0f,-1.0f,

		 1.0f, 1.0f, 1.0f,
		-1.0f, 1.0f,-1.0f,
		-1.0f, 1.0f, 1.0f,

		-1.0f, 1.0f, 1.0f,
		-1.0f,-1.0f, 1.0f,
		 1.0f,-1.0f, 1.0f,

		 1.0f, 1.0f, 1.0f,
		-1.0f, 1.0f, 1.0f,
		 1.0f,-1.0f, 1.0f;
	

	data.uvs.resize(12*3,2);
    data.uvs << 
        0.f,0.f,
		0.f,1.f,
		1.f,1.f,
		
		0.f,0.f,
		1.f,1.f,
		1.f,0.f,
		
		1.f,1.f,
		0.f,1.f,
		0.f,0.f,
		
		1.f,1.f,
		0.f,0.f,
		1.f,0.f,

		1.f,1.f,
		0.f,0.f,
		0.f,1.f,
		 
		1.f,1.f,
		1.f,0.f,
		0.f,0.f,

		1.f,1.f,
		0.f,0.f,
		1.f,0.f,

		0.f,0.f,
		1.f,1.f,
		0.f,1.f,

		1.f,1.f,
		1.f,0.f,
		0.f,0.f,

		1.f,1.f,
		0.f,0.f,
		0.f,1.f,

		0.f,1.f,
		0.f,0.f,
		1.f,0.f,

		1.f,1.f,
		0.f,1.f,
		1.f,0.f;

    data.norms.resize(12*3,3);
    data.norms << 
        -1.0f,-0.0f,-0.0f,
		-1.0f,-0.0f,-0.0f,
		-1.0f,-0.0f,-0.0f,
		-1.0f,-0.0f,-0.0f,
		-1.0f,-0.0f,-0.0f,
		-1.0f,-0.0f,-0.0f,
		 0.0f,-1.0f, 0.0f,
		 0.0f,-1.0f, 0.0f,
		 0.0f,-1.0f, 0.0f,
		 0.0f,-1.0f, 0.0f,
		 0.0f,-1.0f, 0.0f,
		 0.0f,-1.0f, 0.0f,
		 0.0f, 0.0f,-1.0f,
		 0.0f, 0.0f,-1.0f,
		 0.0f, 0.0f,-1.0f,
		 0.0f, 0.0f,-1.0f,
		 0.0f, 0.0f,-1.0f,
		 0.0f, 0.0f,-1.0f,
		 1.0f, 0.0f, 0.0f,
		 1.0f, 0.0f, 0.0f,
		 1.0f, 0.0f, 0.0f,
		 1.0f, 0.0f, 0.0f,
		 1.0f, 0.0f, 0.0f,
		 1.0f, 0.0f, 0.0f,
		 0.0f, 1.0f, 0.0f,
		 0.0f, 1.0f, 0.0f,
		 0.0f, 1.0f, 0.0f,
		 0.0f, 1.0f, 0.0f,
		 0.0f, 1.0f, 0.0f,
		 0.0f, 1.0f, 0.0f,
		 0.0f, 0.0f, 1.0f,
		 0.0f, 0.0f, 1.0f,
		 0.0f, 0.0f, 1.0f,
		 0.0f, 0.0f, 1.0f,
		 0.0f, 0.0f, 1.0f,
		 0.0f, 0.0f, 1.0f;
		 
	return data;
}

inline MeshData formSphere(int slices, int stacks)
{
	MeshData data;
	
	vector<Vec3> verts;
	vector<Vec3> norms;

	for (int stack = 0; stack < stacks; stack++)
	{
		float height_0 = -1.0f + stack * 2.0f / stacks;
		float height_1 = -1.0f + (stack + 1) * 2.0f / stacks;

		float radius_0 = sqrt(1.0 - height_0*height_0);
		float radius_1 = sqrt(1.0 - height_1*height_1);

		for (int slice = 0; slice < slices; slice++)
		{
			float ang_0 = slice * 2.0f * M_PI / slices;
			float ang_1 = (slice + 1) * 2.0f * M_PI / slices;

			Vec3 vec_0(cos(ang_0), sin(ang_0), 0);
			Vec3 vec_1(cos(ang_1), sin(ang_1), 0);

			Vec3 v00 = vec_0 * radius_0 + Vec3(0,0,height_0);
			Vec3 v01 = vec_0 * radius_1 + Vec3(0,0,height_1);
			Vec3 v10 = vec_1 * radius_0 + Vec3(0,0,height_0);
			Vec3 v11 = vec_1 * radius_1 + Vec3(0,0,height_1);

			verts.push_back(v00);
			verts.push_back(v10);
			verts.push_back(v11);

			verts.push_back(v00);
			verts.push_back(v11);
			verts.push_back(v01);

			norms.push_back(v00);
			norms.push_back(v10);
			norms.push_back(v11);

			norms.push_back(v00);
			norms.push_back(v11);
			norms.push_back(v01);
		}
	}

	data.verts.resize(verts.size(), 3);
	data.norms.resize(norms.size(), 3);

    for (size_t i = 0; i < verts.size(); i++)
    {
        data.verts.row(i) = verts[i];
        data.norms.row(i) = norms[i];
    }
	
	return data;
}

inline MeshData formCone(float rad, float height, int slices, bool smooth = true)
{
	MeshData data;
	
	//2 * slices faces
	vector<Vec3> verts;
	vector<Vec3> norms;
	
	for (int slice = 0; slice < slices; slice++)
	{
		float angle = (float)slice / slices * 2 * M_PI;
		float next_angle = (float)(slice + 1) / slices * 2 * M_PI;
		
		Vec3 angle_vert = Vec3(cos(angle), sin(angle), 0)*rad;
		Vec3 next_angle_vert = Vec3(cos(next_angle), sin(next_angle), 0)*rad;
		
		verts.push_back(Vec3::Zero());
		verts.push_back(next_angle_vert);
		verts.push_back(angle_vert);
		
		{
			Vec3 norm(0,0,-1);
			norms.push_back(norm);
			norms.push_back(norm);
			norms.push_back(norm);
		}
		
		Vec3 top_vert = Vec3(0,0,height);
		verts.push_back(top_vert);
		verts.push_back(angle_vert);
		verts.push_back(next_angle_vert);

		if (smooth)
		{
			norms.push_back(Vec3(0,0,1));
			norms.push_back(angle_vert / angle_vert.norm());
			norms.push_back(next_angle_vert / next_angle_vert.norm());
		}
		else
		{
			Vec3 norm = (angle_vert - top_vert).cross(next_angle_vert - top_vert);
			norm /= norm.norm();
			norms.push_back(norm);
			norms.push_back(norm);
			norms.push_back(norm);
		}
	}
	
	data.verts.resize(verts.size(), 3);
	data.norms.resize(norms.size(), 3);

    for (size_t i = 0; i < verts.size(); i++)
    {
        data.verts.row(i) = verts[i];
        data.norms.row(i) = norms[i];
    }
	
	return data;
}

inline MeshData formCylinder(float rad, float height, int slices, bool smooth = true)
{
	MeshData data;
	
	vector<Vec3> verts;
	vector<Vec3> norms;
	
	for (int slice = 0; slice < slices; slice++)
	{
		float angle = (float)slice / slices * 2 * M_PI;
		float next_angle = (float)(slice + 1) / slices * 2 * M_PI;
		
		Vec3 angle_vert = Vec3(cos(angle), sin(angle), 0)*rad;
		Vec3 next_angle_vert = Vec3(cos(next_angle), sin(next_angle), 0)*rad;
		
		Vec3 top_vert = Vec3(0,0,height/2);
		Vec3 bottom_vert = Vec3(0,0,-height/2);
		
		Vec3 top_angle_vert = angle_vert + top_vert;
		Vec3 top_next_angle_vert = next_angle_vert + top_vert;
		
		Vec3 bottom_angle_vert = angle_vert + bottom_vert;
		Vec3 bottom_next_angle_vert = next_angle_vert + bottom_vert;
		
		verts.push_back(bottom_vert);
		verts.push_back(bottom_next_angle_vert);
		verts.push_back(bottom_angle_vert);
		
		{
			Vec3 norm(0,0,-1);
			norms.push_back(norm);
			norms.push_back(norm);
			norms.push_back(norm);
		}
		
		verts.push_back(top_vert);
		verts.push_back(top_angle_vert);
		verts.push_back(top_next_angle_vert);
		
		{
			Vec3 norm(0,0,1);
			norms.push_back(norm);
			norms.push_back(norm);
			norms.push_back(norm);
		}
		
		verts.push_back(top_angle_vert);
		verts.push_back(bottom_angle_vert);
		verts.push_back(bottom_next_angle_vert);

		if (smooth)
		{
			norms.push_back(angle_vert / angle_vert.norm());
			norms.push_back(angle_vert / angle_vert.norm());
			norms.push_back(next_angle_vert / next_angle_vert.norm());
		}
		else
		{
			Vec3 norm = (bottom_angle_vert - top_angle_vert).cross(bottom_next_angle_vert - top_angle_vert);
			norm /= norm.norm();
			norms.push_back(norm);
			norms.push_back(norm);
			norms.push_back(norm);
		}
		
		verts.push_back(top_angle_vert);
		verts.push_back(bottom_next_angle_vert);
		verts.push_back(top_next_angle_vert);

		if (smooth)
		{
			norms.push_back(angle_vert / angle_vert.norm());
			norms.push_back(next_angle_vert / next_angle_vert.norm());
			norms.push_back(next_angle_vert / next_angle_vert.norm());
		}
		else
		{
			Vec3 norm = (top_next_angle_vert - top_angle_vert).cross(bottom_next_angle_vert - top_angle_vert);
			norm /= norm.norm();
			norms.push_back(norm);
			norms.push_back(norm);
			norms.push_back(norm);
		}
	}
	
	data.verts.resize(verts.size(), 3);
	data.norms.resize(norms.size(), 3);

    for (size_t i = 0; i < verts.size(); i++)
    {
        data.verts.row(i) = verts[i];
        data.norms.row(i) = norms[i];
    }
	
	return data;
}

inline MeshData formPlane(const Vec2& rads)
{
	MeshData data;
	
	data.verts.resize(12, 3);
	data.uvs.resize(12, 2);
	data.norms.resize(12, 3);

	data.verts << 
		-rads[0],-rads[1],0,
		rads[0],rads[1],0,
		rads[0],-rads[1],0,
		-rads[0],-rads[1],0,
		-rads[0],rads[1],0,
		rads[0],rads[1],0,
		-rads[0],-rads[1],0,
		rads[0],-rads[1],0,
		rads[0],rads[1],0,
		-rads[0],-rads[1],0,
		rads[0],rads[1],0,
		-rads[0],rads[1],0;

	data.uvs <<
		0,0,
		1,1,
		1,0,
		0,0,
		0,1,
		1,1,
		0,0,
		1,0,
		1,1,
		0,0,
		1,1,
		0,1;

	data.norms << 
		0,0,-1,
		0,0,-1,
		0,0,-1,
		0,0,-1,
		0,0,-1,
		0,0,-1,
		0,0,1,
		0,0,1,
		0,0,1,
		0,0,1,
		0,0,1,
		0,0,1;
		
	return data;
}

inline MeshData formCircle(float rad, float thickness, int slices)
{
	MeshData data;
	
	data.verts.resize(4*3*slices, 3);
	data.norms.resize(4*3*slices, 3);
	
	for (int slice = 0; slice < slices; slice++)
	{
		float angle = (float)slice / slices * 2 * M_PI;
		float next_angle = (float)(slice + 1) / slices * 2 * M_PI;
		
		Vec3 far_angle_vert = Vec3(cos(angle), sin(angle), 0);
		Vec3 far_next_angle_vert = Vec3(cos(next_angle), sin(next_angle), 0);

		Vec3 near_angle_vert = far_angle_vert * (rad - thickness);
		Vec3 near_next_angle_vert = far_next_angle_vert * (rad - thickness);
		
		far_angle_vert *= rad;
		far_next_angle_vert *= rad;
		
		data.verts.row(slice*4*3+0+0) = near_angle_vert;
		data.verts.row(slice*4*3+0+1) = far_next_angle_vert;
		data.verts.row(slice*4*3+0+2) = near_next_angle_vert;

		data.norms.row(slice*4*3+0+0) = Vec3(0,0,1);
		data.norms.row(slice*4*3+0+1) = Vec3(0,0,1);
		data.norms.row(slice*4*3+0+2) = Vec3(0,0,1);
		
		data.verts.row(slice*4*3+3+0) = near_angle_vert;
		data.verts.row(slice*4*3+3+1) = far_angle_vert;
		data.verts.row(slice*4*3+3+2) = far_next_angle_vert;

		data.norms.row(slice*4*3+3+0) = Vec3(0,0,1);
		data.norms.row(slice*4*3+3+1) = Vec3(0,0,1);
		data.norms.row(slice*4*3+3+2) = Vec3(0,0,1);
		
		data.verts.row(slice*4*3+6+0) = near_angle_vert;
		data.verts.row(slice*4*3+6+1) = near_next_angle_vert;
		data.verts.row(slice*4*3+6+2) = far_next_angle_vert;

		data.norms.row(slice*4*3+6+0) = Vec3(0,0,-1);
		data.norms.row(slice*4*3+6+1) = Vec3(0,0,-1);
		data.norms.row(slice*4*3+6+2) = Vec3(0,0,-1);
		
		data.verts.row(slice*4*3+9+0) = near_angle_vert;
		data.verts.row(slice*4*3+9+1) = far_next_angle_vert;
		data.verts.row(slice*4*3+9+2) = far_angle_vert;

		data.norms.row(slice*4*3+9+0) = Vec3(0,0,-1);
		data.norms.row(slice*4*3+9+1) = Vec3(0,0,-1);
		data.norms.row(slice*4*3+9+2) = Vec3(0,0,-1);
	}
	
	return data;
}

using HeightMapUVCallback = std::function<void(
	const Vec2i& cell, 
	const Vec3& v00,
	const Vec3& v01,
	const Vec3& v10,
	const Vec3& v11,
	Vec2& uv00,
	Vec2& uv01,
	Vec2& uv10,
	Vec2& uv11
)>;

inline MeshData formHeightMap(const MatX& heights, float res, const HeightMapUVCallback& uv_callback = NULL)
{
	MeshData data;
	
    vector<Vec3> verts; 
	vector<Vec2> uvs; 
    vector<Vec3> norms;

    size_t expected_vert_count = (heights.rows() - 1) * (heights.cols() - 1) * 3 * 2;

    verts.reserve(expected_vert_count);
	uvs.reserve(expected_vert_count);
    norms.reserve(expected_vert_count);

    Vec2 center((heights.rows()-1)/2.0f * res, (heights.cols()-1)/2.0f * res);

    for (int r = 0; r < heights.rows() - 1; r++)
    {
        float x = r * res - center[0];
		float next_x = (r + 1) * res - center[0];// + 1e-2;

        for (int c = 0; c < heights.cols() - 1; c++)
        {
            float y = c * res - center[1];
			float next_y = (c + 1) * res - center[1];// + 1e-2;

            Vec3 v00(x, y, -heights(r,c));
            Vec3 v10(next_x, y, -heights(r+1,c));
            Vec3 v01(x, next_y, -heights(r,c+1));
            Vec3 v11(next_x, next_y, -heights(r+1,c+1));
			
			Vec2 uv00, uv01, uv10, uv11;

			if (uv_callback)
			{
				uv_callback(Vec2i(r,c), v00, v01, v10, v11, uv00, uv01, uv10, uv11);
			}
			else
			{
				uv00 = Vec2((float)r / (heights.rows() - 1), (float)c / (heights.rows() - 1));
				uv10 = Vec2((float)(r+1) / (heights.rows() - 1), (float)c / (heights.rows() - 1));
				uv01 = Vec2((float)r / (heights.rows() - 1), (float)(c+1) / (heights.rows() - 1));
				uv11 = Vec2((float)(r+1) / (heights.rows() - 1), (float)(c+1) / (heights.rows() - 1));
			}

            verts.push_back(v00);
            verts.push_back(v11);
            verts.push_back(v10);

			uvs.push_back(uv00);
            uvs.push_back(uv11);
            uvs.push_back(uv10);

            Vec3 norm_1 = (v11 - v00).cross(v10 - v00);
            norm_1 /= norm_1.norm();

            norms.push_back(norm_1);
            norms.push_back(norm_1);
            norms.push_back(norm_1);                        


            verts.push_back(v00);
            verts.push_back(v01);
            verts.push_back(v11);

			uvs.push_back(uv00);
            uvs.push_back(uv01);
            uvs.push_back(uv11);

            Vec3 norm_2 = (v01 - v00).cross(v11 - v00);
            norm_2 /= norm_2.norm();

            norms.push_back(norm_2);
            norms.push_back(norm_2);
            norms.push_back(norm_2);

        }
    }

    data.verts.resize(verts.size(), 3);
    data.uvs.resize(uvs.size(), 2);
	data.norms.resize(norms.size(), 3);

    for (size_t i = 0; i < verts.size(); i++)
    {
        data.verts.row(i) = verts[i];
		data.uvs.row(i) = uvs[i];
        data.norms.row(i) = norms[i];
    }
	
	return data;
}

struct OBJMeshData
{
	MeshData geometry;
	Vec3 diffuse;
	std::string diffuse_texture;
};

inline vector<OBJMeshData> loadOBJMesh(const string& path)
{
	vector<OBJMeshData> mesh_parts;
	
	objl::Loader Loader;
	Loader.LoadFile(path);

	for (int i = 0; i < Loader.LoadedMeshes.size(); i++)
	{
		objl::Mesh curMesh = Loader.LoadedMeshes[i];

		MeshData curr_mesh_data;
		curr_mesh_data.verts.resize(curMesh.Indices.size(), 3);
		curr_mesh_data.uvs.resize(curMesh.Indices.size(), 2);
		curr_mesh_data.norms.resize(curMesh.Indices.size(), 3);

		for (int j = 0; j < curMesh.Indices.size(); j++)
		{
			size_t index = curMesh.Indices[j];

			curr_mesh_data.verts.row(j) = Vec3(
				curMesh.Vertices[index].Position.X, 
				curMesh.Vertices[index].Position.Y, 
				curMesh.Vertices[index].Position.Z);
			curr_mesh_data.uvs.row(j) = Vec2(
				curMesh.Vertices[index].TextureCoordinate.X, 
				curMesh.Vertices[index].TextureCoordinate.Y);
			curr_mesh_data.norms.row(j) = Vec3(
				curMesh.Vertices[index].Normal.X, 
				curMesh.Vertices[index].Normal.Y, 
				curMesh.Vertices[index].Normal.Z);
			
			if (j % 3 == 2) // Every third vertex
			{
				using Vec2r = Eigen::Matrix<float, 1, 2>;
				using Vec3r = Eigen::Matrix<float, 1, 3>;
				
				const Vec3r& v0 = curr_mesh_data.verts.row(j-2);
				const Vec3r& v1 = curr_mesh_data.verts.row(j-1);
				const Vec3r& v2 = curr_mesh_data.verts.row(j-0);
				Vec3r calc_normal = (v1 - v0).cross(v2 - v0);
				Vec3r sum_normal = (
					curr_mesh_data.norms.row(j-0) +
					curr_mesh_data.norms.row(j-1) +
					curr_mesh_data.norms.row(j-2)
				);

				if (calc_normal.dot(sum_normal) < 0)
				{
					// Wrong vertex order, swap two
					Vec3r v = curr_mesh_data.verts.row(j-0);
					curr_mesh_data.verts.row(j-0) = curr_mesh_data.verts.row(j-1);
					curr_mesh_data.verts.row(j-1) = v;
					
					Vec3r n = curr_mesh_data.norms.row(j-0);
					curr_mesh_data.norms.row(j-0) = curr_mesh_data.norms.row(j-1);
					curr_mesh_data.norms.row(j-1) = n;
					
					Vec2r u = curr_mesh_data.uvs.row(j-0);
					curr_mesh_data.uvs.row(j-0) = curr_mesh_data.uvs.row(j-1);
					curr_mesh_data.uvs.row(j-1) = u;
				}
			}
		}
		
		Vec3 diffuse(curMesh.MeshMaterial.Kd.X, curMesh.MeshMaterial.Kd.Y, curMesh.MeshMaterial.Kd.Z);
		mesh_parts.push_back({curr_mesh_data, diffuse, curMesh.MeshMaterial.map_Kd});
	}
	
	return mesh_parts;
}

// http://www.opengl-tutorial.org/intermediate-tutorials/tutorial-13-normal-mapping/#vertex-shader
inline void computeTangents(const MeshData& data, MatX& tangents, MatX& bitangents)
{
	tangents.resize(data.verts.rows(), 3);
	bitangents.resize(data.verts.rows(), 3);

	if (data.uvs.rows() == 0)
	{
		for (int i=0; i < data.verts.rows(); i+=3)
		{
			Vec3 tangent(1,0,0);
			Vec3 bitangent(0,1,0);
			
			tangents.row(i+0) = tangent;
			tangents.row(i+1) = tangent;
			tangents.row(i+2) = tangent;
			
			bitangents.row(i+0) = bitangent;
			bitangents.row(i+1) = bitangent;
			bitangents.row(i+2) = bitangent;
		}
		
		return;
	}		
	
	for (int i=0; i < data.verts.rows(); i+=3)
	{
        // Shortcuts for vertices
        const Vec3& v0 = data.verts.row(i+0);
        const Vec3& v1 = data.verts.row(i+1);
        const Vec3& v2 = data.verts.row(i+2);

        // Shortcuts for UVs
        const Vec2& uv0 = data.uvs.row(i+0);
        const Vec2& uv1 = data.uvs.row(i+1);
        const Vec2& uv2 = data.uvs.row(i+2);

        // Edges of the triangle : position delta
        Vec3 deltaPos1 = v1-v0;
        Vec3 deltaPos2 = v2-v0;

        // UV delta
        Vec2 deltaUV1 = uv1-uv0;
        Vec2 deltaUV2 = uv2-uv0;
		
		float r = 1.0f / (deltaUV1[0] * deltaUV2[1] - deltaUV1[1] * deltaUV2[0]);
        Vec3 tangent = (deltaPos1 * deltaUV2[1] - deltaPos2 * deltaUV1[1])*r;
        Vec3 bitangent = (deltaPos2 * deltaUV1[0] - deltaPos1 * deltaUV2[0])*r;
		
		float tangent_mag = tangent.norm();
		float bitangent_mag = bitangent.norm();
		
		if (tangent_mag > 1e-3)
			tangent /= tangent_mag;
		else
			tangent = Vec3(1,0,0);
		if (bitangent_mag > 1e-3)
			bitangent /= bitangent_mag;
		else
			bitangent = Vec3(0,1,0);
		
		tangents.row(i+0) = tangent;
		tangents.row(i+1) = tangent;
		tangents.row(i+2) = tangent;
		
		bitangents.row(i+0) = bitangent;
		bitangents.row(i+1) = bitangent;
		bitangents.row(i+2) = bitangent;
	}
}

}