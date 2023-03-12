#include "trackball_camera.h"

void TrackballCamera::resizeWindow(Window& window)
{
	Mat4 proj = makePerspectiveProj(fov, window.getSize(), curr_near_z, curr_far_z);
	window.setProjectionMatrix(proj);
}

void TrackballCamera::setup(const json& data, Window& window, AssetManager& assets)
{	
	auto back_tex = getTextureFromImage(*generateGradientImage(Vec2i(64,64), Vec3(0.3,0.3,0.6), Vec3(0.05,0.05,0.1)));
	auto back = make_shared<Background>(back_tex);
	window.add(back, Transform3(), false, -1);
	
	MeshData orbit_sphere;
	formSphere(16,16,orbit_sphere);
	auto orbit_point = make_shared<Mesh>(orbit_sphere);
	orbit_point_render = window.add(orbit_point, Transform3());
	orbit_point_render->setShow(false);

	orbit_point_render->setScale(scene_scale * orbit_point_scene_scale_mult);
	
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
	
	curr_near_z = 0.001f * scene_scale;
	curr_far_z = 10.0f * scene_scale;
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
}

void TrackballCamera::update(Window& window, float dt, const ControlInfo& control_info)
{
	if (control_info.left_drag_on || control_info.total_scroll_amount != 0.0f || control_info.key_motion != Vec3::Zero())
	{
		orbit_point_render->setShow(true);
	}
	else
	{
		orbit_point_render->setShow(false);
	}

	if (control_info.left_drag_on)
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

	if (!control_info.left_drag_on)
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

			curr_near_z = 0.001f * scene_scale;
			curr_far_z = 100.0f * scene_scale;
			resizeWindow(window);

			trackball_from = offset + trackball_to;
			world_from_cam.translation(trackball_from);
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
			world_from_cam.translation(trackball_from);
			window.setCamFromWorld(world_from_cam.inverse());
		}
	}
}