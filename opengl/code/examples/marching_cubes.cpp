/**
 * Copyright 2023
 * Carnegie Robotics, LLC
 * 4501 Hatfield Street, Pittsburgh, PA 15201
 * https://www.carnegierobotics.com
 *
 * This code is provided under the terms of the Master Services Agreement (the Agreement).
 * This code constitutes CRL Background Intellectual Property, as defined in the Agreement.
**/

#include "vephor.h"
#include "vephor_ogl.h"
#include <random>
#include <chrono>

using namespace vephor;

void addGridPlane(
	const Vec2& sizes,
	const Vec2i& divisions,
	const Mat3& world_R_grid,
	const Vec3& world_t_grid,
	vector<Vec3>& grid_lines)
{
	for (int i = 0; i < divisions[0]; i++)
	{
		Vec3 base_pt(0,0,0);
		base_pt[0] = sizes[0] * i / ((float)divisions[0] - 1.0f);
		grid_lines.push_back(world_R_grid * base_pt + world_t_grid);
		
		base_pt[1] += sizes[1];
		grid_lines.push_back(world_R_grid * base_pt + world_t_grid);
	}
	
	for (int i = 0; i < divisions[1]; i++)
	{
		Vec3 base_pt(0,0,0);
		base_pt[1] = sizes[1] * i / ((float)divisions[1] - 1.0f);
		grid_lines.push_back(world_R_grid * base_pt + world_t_grid);
		
		base_pt[0] += sizes[0];
		grid_lines.push_back(world_R_grid * base_pt + world_t_grid);
	}
}

int main()
{
    Window window(-1, -1, "marching cubes");
	
	window.setFrameLock(60.0f);
	
	int i_inv_res = 4;
	
	Tensor<3,float> occ({8*i_inv_res+1,8*i_inv_res+1,8*i_inv_res+1});
	occ.fill(0.0f);
	
	float radius = 3.0f;
	float res = 1.0f / i_inv_res;
	Vec3 center = (occ.size().cast<float>() - Vec3(1,1,1))/2.0f;
	
	auto iter = occ.getIter();
	
	while(!iter.atEnd())
	{
		const auto ind = iter.getIndex();
		
		Vec3 pt = (ind.cast<float>() - center) * res;
		float dist = pt.norm();
		
		if (dist <= radius)
			occ(ind) = 1.0f;
		else
		{
			float diff = dist - radius;
			diff *= 10.0f;
			occ(ind) = 1.0f / (1.0f + diff);
		}
		
		if (fabs(pt(1)) < 1.5f && fabs(pt(2)) < 1.5f)
		{
			occ(ind) = 0.0f;
		}
			
		
		iter++;
	}
	
	
	
	
	vector<Vec3> all_grid_lines;
	
	int division_reduction = 1;
	
	{
		Vec3 occ_size_f = (occ.size().cast<float>() - Vec3(1,1,1))*res;
		Mat3 world_R_grid;
		world_R_grid << 1,0,0,0,1,0,0,0,1;
		addGridPlane(
			Vec2(occ_size_f[0], occ_size_f[1]), 
			Vec2i(occ.size()[0], occ.size()[1]) / division_reduction, 
			world_R_grid,
			Vec3(-occ_size_f[0]/2.0f, -occ_size_f[1]/2.0f, -occ_size_f[2]/2.0f),
			all_grid_lines
		);
	}
	
	{
		Vec3 occ_size_f = (occ.size().cast<float>() - Vec3(1,1,1))*res;
		Mat3 world_R_grid;
		world_R_grid << 1,0,0,0,1,0,0,0,1;
		addGridPlane(
			Vec2(occ_size_f[0], occ_size_f[1]), 
			Vec2i(occ.size()[0], occ.size()[1]) / division_reduction, 
			world_R_grid,
			Vec3(-occ_size_f[0]/2.0f, -occ_size_f[1]/2.0f, occ_size_f[2]/2.0f),
			all_grid_lines
		);
	}
	
	{
		Vec3 occ_size_f = (occ.size().cast<float>() - Vec3(1,1,1))*res;
		Mat3 world_R_grid;
		world_R_grid << 0,0,1,1,0,0,0,1,0;
		addGridPlane(
			Vec2(occ_size_f[1], occ_size_f[2]), 
			Vec2i(occ.size()[1], occ.size()[2]) / division_reduction, 
			world_R_grid,
			Vec3(-occ_size_f[0]/2.0f, -occ_size_f[1]/2.0f, -occ_size_f[2]/2.0f),
			all_grid_lines
		);
	}
	
	{
		Vec3 occ_size_f = (occ.size().cast<float>() - Vec3(1,1,1))*res;
		Mat3 world_R_grid;
		world_R_grid << 0,0,1,1,0,0,0,1,0;
		addGridPlane(
			Vec2(occ_size_f[1], occ_size_f[2]), 
			Vec2i(occ.size()[1], occ.size()[2]) / division_reduction, 
			world_R_grid,
			Vec3(occ_size_f[0]/2.0f, -occ_size_f[1]/2.0f, -occ_size_f[2]/2.0f),
			all_grid_lines
		);
	}
	
	{
		Vec3 occ_size_f = (occ.size().cast<float>() - Vec3(1,1,1))*res;
		Mat3 world_R_grid;
		world_R_grid << 0,1,0,0,0,1,1,0,0;
		addGridPlane(
			Vec2(occ_size_f[2], occ_size_f[0]), 
			Vec2i(occ.size()[2], occ.size()[0]) / division_reduction, 
			world_R_grid,
			Vec3(-occ_size_f[0]/2.0f, -occ_size_f[1]/2.0f, -occ_size_f[2]/2.0f),
			all_grid_lines
		);
	}
	
	{
		Vec3 occ_size_f = (occ.size().cast<float>() - Vec3(1,1,1))*res;
		Mat3 world_R_grid;
		world_R_grid << 0,1,0,0,0,1,1,0,0;
		addGridPlane(
			Vec2(occ_size_f[2], occ_size_f[0]), 
			Vec2i(occ.size()[2], occ.size()[0]) / division_reduction, 
			world_R_grid,
			Vec3(-occ_size_f[0]/2.0f, occ_size_f[1]/2.0f, -occ_size_f[2]/2.0f),
			all_grid_lines
		);
	}
	
	MatX line_verts(all_grid_lines.size(), 3);
	for (int i = 0; i < all_grid_lines.size(); i++)
	{
		const auto& v = all_grid_lines[i];
		line_verts.row(i) = v.transpose();
	}
	
	auto line_obj = make_shared<Lines>(
		line_verts,
        Vec3(1.0,1.0,1.0)
	);
	line_obj->setLineStrip(false);
	window.add(line_obj, Transform3());
	
	auto sphere_mesh_data = formSphere(16, 16);
	
	auto point_light = make_shared<PointLight>();
    window.add(point_light, Transform3(Vec3(0,10,-10)));
	
	Mat4 proj = makePerspectiveProj(45, window.getSize(), 0.1f, 100.0f);
    window.setProjectionMatrix(proj);

    window.setCamFromWorld(makeLookAtTransform(
        Vec3(0,0,0),
        Vec3(-10,0,0),
        Vec3(0,0,-1)
    ));
	
	MeshData mcubes_mesh_data;
	mcubes::calcSurfaces(occ, 0.5f, res, mcubes_mesh_data);
	auto mcubes_mesh = make_shared<Mesh>(
        mcubes_mesh_data,
        Vec3(1.0,1.0,1.0));
    window.add(mcubes_mesh, Transform3());

	float curr_time = 0.0f;
	while (window.render())
    {
		window.setCamFromWorld(makeLookAtTransform(
			Vec3(0,0,0),
			Vec3(-10*cos(curr_time),10*sin(curr_time),0),
			Vec3(0,0,-1)
		));
		
		curr_time += 0.3f / 60.0f;
	}
	
	return 0;
}