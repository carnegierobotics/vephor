/**
 * Copyright 2025
 * Carnegie Robotics, LLC
 * 4501 Hatfield Street, Pittsburgh, PA 15201
 * https://www.carnegierobotics.com
 *
 * This source code is licensed under the Apache License, Version 2.0
 * found in the LICENSE file in the root directory of this source tree.
**/

#include "vephor/ogl/camera/plot3d_camera.h"

namespace vephor
{

void Plot3DCamera::resizeWindow(Window& window)
{
	float aspect = (float)window.getSize()[0]/window.getSize()[1];
    Mat4 proj = makeOrthoProj(
        Vec3(-zoom*aspect,-zoom,-1000),//curr_near_z), 
        Vec3(zoom*aspect,zoom,1000)//curr_far_z)
    );
	window.setProjectionMatrix(proj);
}

void Plot3DCamera::moveGrid(Window& window)
{
	float curr_inc = pow(10, floor(log(zoom/5)/log(10)));

	grid_render->setScale(curr_inc);

	Vec3 pos = orbit_point_render->getPos();

	Vec3 cam_forward_in_world = window.getCamFromWorld().rotation().row(2);

	cam_forward_in_world = cam_forward_in_world.array().abs();

	Eigen::Index max_index;
	cam_forward_in_world.maxCoeff(&max_index);

	Vec3 rot_vec = Vec3::Zero();
	if (max_index == 0)
		rot_vec[1] = M_PI / 2;
	else if (max_index == 1)
		rot_vec[0] = M_PI / 2;

	Vec3 clamp_pos = (pos/curr_inc + Vec3(.5f,.5f,.5f)).array().floor().cast<float>()*curr_inc;

	grid_render->setPos(clamp_pos);
	grid_render->setOrient(rot_vec);

	axes_render->setScale(orbit_point_render->getScale()*5);
	axes_render->setPos(pos);

	zero_axes_render->setScale(orbit_point_render->getScale()*5);

	if (curr_pos_text_render)
		curr_pos_text_render->setDestroy();

	auto text = make_shared<Text>("("+
		std::to_string(pos[0])+","+
		std::to_string(pos[1])+","+
		std::to_string(pos[2])+")", text_tex, Vec3(0,0,0));
	text->setAnchorBottom();

	Vec3 curr_pos_text_pos = pos + trackball_up*zoom/30.0f;
	curr_pos_text_render = window.add(text, Transform3(curr_pos_text_pos), false /*overlay*/, 2 /*layer*/);
	curr_pos_text_render->setScale(zoom/30);



	if (curr_inc_text_render)
		curr_inc_text_render->setDestroy();

	curr_inc_text_render = window.add(make_shared<Text>("Unit: " + std::to_string(curr_inc), text_tex, Vec3(0,0,0)), 
		Transform3(), true);
	curr_inc_text_render->setScale(25);
}

void Plot3DCamera::setup(const json& data, Window& window, AssetManager& assets)
{	
	Vec3 light_dir(0,0,1);
    auto dir_light = make_shared<DirLight>(light_dir, 0.0f);
    window.add(dir_light, Transform3());

    auto ambient_light = make_shared<AmbientLight>(Vec3(1,1,1));
    window.add(ambient_light, Transform3());

	Vec3 back_color = readDefault(data, "back_color", Vec3(0.25,0.25,0.25));

	auto back_tex = window.getTextureFromImage(*generateSimpleImage(Vec2i(64,64), back_color));
	auto back = make_shared<Background>(back_tex);
	window.add(back, Transform3(), false, -1);

	{
		auto axes = make_shared<Axes>();
		axes->setColors(
			Vec3(0,1,1),
			Vec3(1,0,1),
			Vec3(1,1,0)
		);
		axes_render = window.add(axes);
	}

	grid_render = window.add(Transform3());

	{
		auto axes = make_shared<Axes>();
		zero_axes_render = window.add(axes);
	}

	{
		//auto grid = make_shared<Grid>(3,Vec3(0,0,1),Vec3(1,0,0),1.0f,Vec3(.9f,.9f,.9f));
		

		int grid_rad = 5;
		float solid_rad = 1.5;

		vector<Vec3> line_verts;
		vector<Vec4> line_colors;

		Vec3 forward(1,0,0);
		Vec3 right(0,1,0);

		// Create horizontal lines
		for (int i = -grid_rad; i <= grid_rad; i++)
		{
			for (int j = -grid_rad; j <= grid_rad - 1; j++)
			{
				int next_j = j + 1;

				float curr_dist = Vec2(i,j).norm();
				float next_dist = Vec2(i,next_j).norm();


				float curr_alpha;
				if (curr_dist < solid_rad)
					curr_alpha = 1.0;
				else
					curr_alpha = (grid_rad - curr_dist) / (grid_rad - solid_rad);

				float next_alpha;
				if (next_dist < solid_rad)
					next_alpha = 1.0;
				else
					next_alpha = (grid_rad - next_dist) / (grid_rad - solid_rad);

				line_verts.push_back(i*forward + j*right);
				line_verts.push_back(i*forward + next_j*right);

				line_colors.push_back(Vec4(.9f,.9f,.9f,curr_alpha));
				line_colors.push_back(Vec4(.9f,.9f,.9f,next_alpha));

				line_verts.push_back(i*right + j*forward);
				line_verts.push_back(i*right + next_j*forward);

				line_colors.push_back(Vec4(.9f,.9f,.9f,curr_alpha));
				line_colors.push_back(Vec4(.9f,.9f,.9f,next_alpha));
			}
		}
		
		auto lines = make_shared<Lines>(toMat(line_verts), toMat(line_colors));
		lines->setLineStrip(false);
		lines->setAlpha(true);

		window.add(lines, Transform3(), false /*overlay*/, 1 /*layer*/)->setParent(grid_render);
	}

	/*{
		auto grid = make_shared<Grid>(100,Vec3(0,0,1),Vec3(1,0,0),10.0f,Vec3(.9f,.9f,.9f));
		window.add(grid)->setParent(grid_render);
	}

	{
		auto grid = make_shared<Grid>(1000,Vec3(0,0,1),Vec3(1,0,0),100.0f,Vec3(.9f,.9f,.9f));
		window.add(grid)->setParent(grid_render);
	}*/
	
	auto orbit_sphere = formSphere(16,16);
	auto orbit_point = make_shared<Mesh>(orbit_sphere, Vec3(0.5,0.5,0.5));
	orbit_point_render = window.add(orbit_point, Transform3());
	orbit_point_render->setShow(false);

	orbit_point_render->setScale(scene_scale * orbit_point_scene_scale_mult);

	auto drag_point = make_shared<Mesh>(orbit_sphere, Vec3(1,1,0));
	drag_point_render = window.add(drag_point, Transform3());
	drag_point_render->setShow(false);

	drag_point_render->setScale(scene_scale * orbit_point_scene_scale_mult);
	
	trackball_to = readVec3(data["to"]);
	trackball_from = readVec3(data["from"]);
	trackball_up = readVec3(data["up"]);
	
	Vec3 offset = trackball_from - trackball_to;

	trackball_fore = findCrossVec(trackball_up);
	trackball_right = trackball_up.cross(trackball_fore);
	trackball_right /= trackball_right.norm();

	float offset_mag = offset.norm();
	if (offset_mag > 0)
	{
		Vec3 norm_offset = offset / offset_mag;
		if (fabs(norm_offset.dot(trackball_up)) >= 1.0f)
		{
			offset -= trackball_fore * 0.001f;
		}
	}
	
	window.setCamFromWorld(makeLookAtTransform(
		trackball_to, trackball_to + offset, trackball_up
	));

	v4print "cam transform:";
	v4print makeLookAtTransform(
		trackball_to, trackball_from, trackball_up
	).matrix();

	if (data.contains("scene_scale"))
		scene_scale = data["scene_scale"];
	else
		scene_scale = (trackball_from - trackball_to).norm();
	orbit_point_render->setScale(scene_scale * orbit_point_scene_scale_mult);
	orbit_point_render->setPos(trackball_to);
	moveGrid(window);
	
	curr_near_z = 0.001f * scene_scale;
	curr_far_z = 10.0f * scene_scale;
	resizeWindow(window);
	
	if (data.contains("auto_fit"))
		auto_fit = data["auto_fit"];
}

void Plot3DCamera::autoFitPoints(Window& window, const vector<Vec3>& pts)
{
	if (!auto_fit)
		return;
	
	auto world_from_cam = window.getCamFromWorld().inverse();
	
	Vec3 up = world_from_cam * Vec3(0,1,0);
	Vec3 offset = world_from_cam.translation() - trackball_to;
	float offset_mag = offset.norm();
	if (offset_mag > 1e-3)
		offset /= offset_mag;
	trackball_to = Vec3::Zero();
	
	for (const auto& pt : pts)
	{
		trackball_to += pt;
	}
	trackball_to /= pts.size();


	Vec3 min_pt(
		std::numeric_limits<float>::max(),
		std::numeric_limits<float>::max(),
		std::numeric_limits<float>::max()
	);

	Vec3 max_pt(
		std::numeric_limits<float>::min(),
		std::numeric_limits<float>::min(),
		std::numeric_limits<float>::min()
	);

	scene_scale = 0;
	for (const auto& pt : pts)
	{
		Vec3 c_pt = pt - trackball_to;
		float offset_dist = offset.dot(c_pt);
		Vec3 cam_pt = window.getCamFromWorld() * c_pt;

		float x_d = fabs(window.getProjectionMatrix()(0,0)*cam_pt[0]);
		float y_d = fabs(window.getProjectionMatrix()(1,1)*cam_pt[1]);

		{
			Vec4 proj = window.getProjectionMatrix() * Vec4(cam_pt[0], cam_pt[1], x_d, 1.0);
			proj /= proj[3];
		}

		{
			Vec4 proj = window.getProjectionMatrix() * Vec4(cam_pt[0], cam_pt[1], y_d, 1.0);
			proj /= proj[3];
		}

		x_d -= offset_dist;
		y_d -= offset_dist;

		if (x_d > scene_scale)
			scene_scale = x_d;
		if (y_d > scene_scale)
			scene_scale = y_d; 

		for (int a = 0; a < 3; a++)
		{
			if (pt[a] < min_pt[a])
				min_pt[a] = pt[a];
			if (pt[a] > max_pt[a])
				max_pt[a] = pt[a];
		}
	}

	offset *= scene_scale;
	zoom = scene_scale;
	
	curr_near_z = 0.001f * scene_scale;
	curr_far_z = 10.0f * scene_scale;
	resizeWindow(window);
	
	v4print "position camera:";
	v4print "\tscale:", scene_scale;
	v4print "\tlook at:", trackball_to.transpose();
	v4print "\tlook offset:", offset.transpose();
	
	trackball_from = trackball_to + offset;
	window.setCamFromWorld(makeLookAtTransform(
		trackball_to, trackball_from, up
	));

	for (const auto& pt : pts)
	{
		Vec3 cam_pt = window.getCamFromWorld() * pt;

		float x_d = fabs(window.getProjectionMatrix()(0,0)*cam_pt[0]);
		float y_d = fabs(window.getProjectionMatrix()(1,1)*cam_pt[1]);

		Vec4 proj = window.getProjectionMatrix() * Vec4(cam_pt[0], cam_pt[1], cam_pt[2], 1.0);
		proj /= proj[3];
	}

	orbit_point_render->setScale(scene_scale * orbit_point_scene_scale_mult);
	orbit_point_render->setPos(trackball_to);
	moveGrid(window);



	// Add the bounding box
	{
		vector<Vec3> line_verts;

		line_verts.push_back(Vec3(min_pt[0], min_pt[1], min_pt[2]));
		line_verts.push_back(Vec3(max_pt[0], min_pt[1], min_pt[2]));

		line_verts.push_back(Vec3(min_pt[0], min_pt[1], min_pt[2]));
		line_verts.push_back(Vec3(min_pt[0], max_pt[1], min_pt[2]));

		line_verts.push_back(Vec3(min_pt[0], min_pt[1], min_pt[2]));
		line_verts.push_back(Vec3(min_pt[0], min_pt[1], max_pt[2]));

		line_verts.push_back(Vec3(max_pt[0], max_pt[1], max_pt[2]));
		line_verts.push_back(Vec3(min_pt[0], max_pt[1], max_pt[2]));

		line_verts.push_back(Vec3(max_pt[0], max_pt[1], max_pt[2]));
		line_verts.push_back(Vec3(max_pt[0], min_pt[1], max_pt[2]));

		line_verts.push_back(Vec3(max_pt[0], max_pt[1], max_pt[2]));
		line_verts.push_back(Vec3(max_pt[0], max_pt[1], min_pt[2]));

		line_verts.push_back(Vec3(max_pt[0], min_pt[1], min_pt[2]));
		line_verts.push_back(Vec3(max_pt[0], max_pt[1], min_pt[2]));

		line_verts.push_back(Vec3(max_pt[0], min_pt[1], min_pt[2]));
		line_verts.push_back(Vec3(max_pt[0], min_pt[1], max_pt[2]));

		line_verts.push_back(Vec3(min_pt[0], max_pt[1], min_pt[2]));
		line_verts.push_back(Vec3(max_pt[0], max_pt[1], min_pt[2]));

		line_verts.push_back(Vec3(min_pt[0], max_pt[1], min_pt[2]));
		line_verts.push_back(Vec3(min_pt[0], max_pt[1], max_pt[2]));

		line_verts.push_back(Vec3(min_pt[0], min_pt[1], max_pt[2]));
		line_verts.push_back(Vec3(max_pt[0], min_pt[1], max_pt[2]));

		line_verts.push_back(Vec3(min_pt[0], min_pt[1], max_pt[2]));
		line_verts.push_back(Vec3(min_pt[0], max_pt[1], max_pt[2]));

		auto lines = make_shared<Lines>(toMat(line_verts), MatX(), Vec4(1,1,1,1));
		lines->setLineStrip(false);

		window.add(lines);
	}
}

void Plot3DCamera::update(Window& window, float dt, const ControlInfo& control_info)
{
	if (control_info.left_drag_on || control_info.right_drag_on || control_info.total_scroll_amount != 0.0f || control_info.key_motion != Vec3::Zero())
	{
		orbit_point_render->setShow(true);
	}
	else
	{
		orbit_point_render->setShow(false);
	}

	if (control_info.right_drag_on)
	{
		Vec2 mouse_delta = window.getMousePos() - control_info.drag_start_mouse_pos;

		if (trackball_3d)
		{
			Vec3 along_vec(-mouse_delta[0],-mouse_delta[1],0);
			float along_vec_mag = along_vec.norm();
			if (along_vec_mag > 1e-3)
			{
				along_vec /= along_vec_mag;
				Vec3 cross_vec = along_vec.cross(Vec3(0,0,1));
				cross_vec /= cross_vec.norm();

				Vec3 rot_vec = cross_vec * (along_vec_mag / 100.0f);

				// We want to rotate the camera about the focal point
				// cam_from_focal * focal_rotation * focal_from_world

				Transform3 world_from_cam = control_info.drag_cam_from_world.inverse();
				Transform3 world_from_focal = Transform3(trackball_to);
				Transform3 drag_cam_from_focal = control_info.drag_cam_from_world * world_from_focal;
				Transform3 cam_from_world = drag_cam_from_focal * 
					Transform3(Vec3::Zero(), world_from_cam.rotation() * rot_vec) * 
					world_from_focal.inverse();
				window.setCamFromWorld(cam_from_world);
				world_from_cam = cam_from_world.inverse();
				trackball_from = world_from_cam.translation();
				trackball_up = world_from_cam.rotation() * Vec3(0,-1,0);
			}
		}
		else
		{
			Transform3 world_from_cam = control_info.drag_cam_from_world.inverse();

			Vec3 offset = world_from_cam.translation() - trackball_to;

			float right = trackball_right.dot(offset);
			float fore = trackball_fore.dot(offset);

			float pitch = -atan2(trackball_up.dot(offset), Vec2(right, fore).norm());
			float yaw = -atan2(right, fore);
			float offset_mag = offset.norm();

			yaw += mouse_delta[0] / 100.0f;
			pitch += mouse_delta[1] / 100.0f;

			if (pitch > M_PI/2 - 1e-3)
				pitch = M_PI/2 - 1e-3;
			else if (pitch < -M_PI/2 + 1e-3)
				pitch = -M_PI/2 + 1e-3;

			offset = Orient3(-trackball_up*yaw) * Orient3(pitch*trackball_right) * (offset_mag*trackball_fore);
			
			trackball_from = offset + trackball_to;
			window.setCamFromWorld(makeLookAtTransform(
				trackball_to, trackball_from, trackball_up
			));
		}

		moveGrid(window);
	}

	if (control_info.left_drag_on)
	{
		Transform3 cam_from_world = window.getCamFromWorld();
		Transform3 world_from_cam = cam_from_world.inverse();
		Vec3 offset = world_from_cam.translation() - trackball_to;

		if (pan_drag_off)
		{
			Vec3 drag_origin, drag_ray;
			window.getWorldRayForMousePos(control_info.drag_start_mouse_pos, drag_origin, drag_ray);

			pan_drag_off = false;
			pan_world_point = drag_origin + drag_ray * (offset.dot(drag_ray));

			scene_scale = offset.norm();
			drag_point_render->setScale(scene_scale * orbit_point_scene_scale_mult);
			drag_point_render->setPos(pan_world_point);
			drag_point_render->setShow(true);
		}

		// Keep the world point selected on initial click under the mouse cursor

		Vec3 curr_origin, curr_ray;
		window.getWorldRayForMousePos(window.getMousePos(), curr_origin, curr_ray);

		// Find the shift in position where this ray points at the given world point, but only move the camera along lateral axes
		// We need to move curr_origin to to somewhere on the line (pan_world_point - curr_ray * lambda)
		// We then apply that same shift to the current camera position
		// target = curr_origin + shift
		// target = pan_world_point - curr_ray * lambda
		// (target - cam_origin) * camera_dir = 0
		// Plane line intersection
		// lambda = camera_dir * (cam_origin - pan_world_point) / (camera_dir * -curr_ray)
		Vec3 camera_dir = offset / offset.norm();
		float lambda = camera_dir.dot(world_from_cam.translation() - pan_world_point) / camera_dir.dot(-curr_ray);
		trackball_from += pan_world_point - curr_ray * lambda - curr_origin;
		trackball_to = trackball_from - offset;

		orbit_point_render->setPos(trackball_to);
		moveGrid(window);

		world_from_cam.setTranslation(trackball_from);
		window.setCamFromWorld(world_from_cam.inverse());
	}
	else
	{
		drag_point_render->setShow(false);

		pan_drag_off = true;
	}

	if (!control_info.right_drag_on)
	{
		if (control_info.total_scroll_amount != 0.0f)
		{
			float mult = exp(-control_info.total_scroll_amount / 10.0f);

			Transform3 cam_from_world = window.getCamFromWorld();
			Transform3 world_from_cam = cam_from_world.inverse();
			Vec3 offset = world_from_cam.translation() - trackball_to;
			offset *= mult;

			scene_scale = offset.norm();
			zoom = scene_scale;
			orbit_point_render->setScale(scene_scale * orbit_point_scene_scale_mult);
			moveGrid(window);

			curr_near_z = 0.001f * scene_scale;
			curr_far_z = 100.0f * scene_scale;
			resizeWindow(window);

			trackball_from = offset + trackball_to;
			world_from_cam.setTranslation(trackball_from);
			window.setCamFromWorld(world_from_cam.inverse());
		}

		if (control_info.key_motion != Vec3::Zero())
		{
			Transform3 cam_from_world = window.getCamFromWorld();
			Transform3 world_from_cam = cam_from_world.inverse();
			Vec3 offset = world_from_cam.translation() - trackball_to;
			
			float movement_mag = scene_scale / 100.0f;
			trackball_to += world_from_cam.rotation() * control_info.key_motion * movement_mag;
			orbit_point_render->setPos(trackball_to);
			moveGrid(window);

			trackball_from = offset + trackball_to;
			world_from_cam.setTranslation(trackball_from);
			window.setCamFromWorld(world_from_cam.inverse());
		}
	}
}

}