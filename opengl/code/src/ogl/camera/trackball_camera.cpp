/**
 * Copyright 2025
 * Carnegie Robotics, LLC
 * 4501 Hatfield Street, Pittsburgh, PA 15201
 * https://www.carnegierobotics.com
 *
 * This source code is licensed under the Apache License, Version 2.0
 * found in the LICENSE file in the root directory of this source tree.
**/

#include "vephor/ogl/camera/trackball_camera.h"

namespace vephor
{
namespace ogl
{

const float TRACKBALL_BASE_NEAR_Z = 0.001f;
const float TRACKBALL_BASE_FAR_Z = 100.0f;

void TrackballCamera::resizeWindow(Window& window)
{
	Mat4 proj = makePerspectiveProj(fov, window.getSize(), curr_near_z, curr_far_z);
	window.setProjectionMatrix(proj);
}

void TrackballCamera::setup(const json& data, Window& window, AssetManager& assets)
{	
	bool back_tex_nearest;
	auto back_img = *getCameraBackground(data, back_tex_nearest);
	auto back_tex = window.getTextureFromImage(back_img, back_tex_nearest);
	auto back = make_shared<Background>(back_tex);
	window.add(back, Transform3(), false, -1);
	
	auto orbit_sphere = formSphere(16,16);
	auto orbit_point = make_shared<Mesh>(orbit_sphere);
	orbit_point_render = window.add(orbit_point, Transform3());
	orbit_point_render->setShow(false);

	auto drag_point = make_shared<Mesh>(orbit_sphere, Vec3(1,1,0));
	drag_point_render = window.add(drag_point, Transform3());
	drag_point_render->setShow(false);


	auto selection_sphere = make_shared<Mesh>(orbit_sphere, Vec3(1,1,1));
	selection_sphere->setOpacity(0.25);
	selection_widget_render = window.add(
		selection_sphere, 
		Transform3(),
		false /*overlay*/,
		1 /*layer*/);
	selection_widget_render->setShow(false);
	
	
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

	drag_point_render->setScale(scene_scale * orbit_point_scene_scale_mult);
	
	curr_near_z = TRACKBALL_BASE_NEAR_Z * scene_scale;
	curr_far_z = TRACKBALL_BASE_FAR_Z * scene_scale;
	resizeWindow(window);

	if (data.contains("3d"))
		trackball_3d = data["3d"];
	
	if (data.contains("auto_fit"))
		auto_fit = data["auto_fit"];
}

void TrackballCamera::autoFitPoints(Window& window, const vector<Vec3>& pts)
{
	if (!auto_fit)
		return;
	
	auto world_from_cam = window.getCamFromWorld().inverse();
	
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
	}

	offset *= scene_scale;
	
	curr_near_z = TRACKBALL_BASE_NEAR_Z * scene_scale;
	curr_far_z = TRACKBALL_BASE_FAR_Z * scene_scale;
	resizeWindow(window);
	
	v4print "position camera:";
	v4print "\tscale:", scene_scale;
	v4print "\tlook at:", trackball_to.transpose();
	v4print "\tlook offset:", offset.transpose();
	
	trackball_from = trackball_to + offset;
	window.setCamFromWorld(makeLookAtTransform(
		trackball_to, trackball_from, trackball_up
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
}

void TrackballCamera::update(Window& window, float dt, const ControlInfo& control_info)
{
	if (control_info.left_drag_on || control_info.right_drag_on || control_info.total_scroll_amount != 0.0f || control_info.key_motion != Vec3::Zero())
	{
		orbit_point_render->setShow(true);
	}
	else
	{
		orbit_point_render->setShow(false);
	}

	std::vector<SelectionWidget> remaining_widgets;
	for (const auto& widget : selection_widgets)
	{
		if (widget.node->checkDestroy())
			continue;

		remaining_widgets.push_back(widget);
	}
	selection_widgets = remaining_widgets;

	Vec3 origin, ray;
	window.getWorldRayForMousePos(window.getMousePos(), origin, ray);
	float best_dist = 1e9;
	const SelectionWidget* best_widget = NULL;
	for (const auto& widget : selection_widgets)
	{
		float dist_along = (widget.center - origin).dot(ray);
		Vec3 closest_pt = origin + ray * dist_along;
		float dist_from = (closest_pt - widget.center).norm();
		if (dist_from < widget.radius && dist_from < best_dist)
		{
			best_dist = dist_from;
			best_widget = &widget;
		}
	}
	if (best_widget != NULL)
	{
		selection_widget_render->setScale(best_widget->radius);
		selection_widget_render->setPos(best_widget->center);
		selection_widget_render->setShow(true);

		if (control_info.left_press)
		{
			Transform3 cam_from_world = window.getCamFromWorld();
			Transform3 world_from_cam = cam_from_world.inverse();
			Vec3 offset = world_from_cam.translation() - trackball_to;

			trackball_from = best_widget->center + offset;
			trackball_to = best_widget->center;

			orbit_point_render->setPos(trackball_to); 

			world_from_cam.setTranslation(trackball_from);
			window.setCamFromWorld(world_from_cam.inverse());

			widget_click = true;
		}
	}
	else
	{
		selection_widget_render->setShow(false);
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
	}

	if (control_info.left_drag_on && !widget_click)
	{
		Transform3 cam_from_world = window.getCamFromWorld();
		Transform3 world_from_cam = cam_from_world.inverse();
		Vec3 offset = world_from_cam.translation() - trackball_to;

		if (pan_drag_off)
		{
			Vec3 drag_origin, drag_ray;
			window.getWorldRayForMousePos(control_info.drag_start_mouse_pos, drag_origin, drag_ray);

			pan_drag_off = false;
			pan_world_point = drag_origin + drag_ray * (-offset.dot(drag_ray));

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
		trackball_from = pan_world_point - curr_ray * lambda;
		trackball_to = trackball_from - offset;

		orbit_point_render->setPos(trackball_to); 

		world_from_cam.setTranslation(trackball_from);
		window.setCamFromWorld(world_from_cam.inverse());
	}

	if (!control_info.left_drag_on)
	{
		drag_point_render->setShow(false);
		pan_drag_off = true;
		widget_click = false;
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
			orbit_point_render->setScale(scene_scale * orbit_point_scene_scale_mult);

			curr_near_z = TRACKBALL_BASE_NEAR_Z * scene_scale;
			curr_far_z = TRACKBALL_BASE_FAR_Z * scene_scale;
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

			trackball_from = offset + trackball_to;
			world_from_cam.setTranslation(trackball_from);
			window.setCamFromWorld(world_from_cam.inverse());
		}
	}
}

} // namespace ogl
} // namespace vephor