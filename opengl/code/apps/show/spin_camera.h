#pragma once

#include "show_camera.h"

class SpinCamera : public ShowCamera
{
public:
	virtual void setup(const json& data, Window& window, AssetManager& assets) override
	{
		auto back_tex = window.getTextureFromImage(*generateGradientImage(Vec2i(64,64), Vec3(0.3,0.3,0.6), Vec3(0.05,0.05,0.1)));
		auto back = make_shared<Background>(back_tex);
		window.add(back, Transform3(), false, -1);
		
		spin_to = readVec3(data["to"]);
		spin_up = readVec3(data["up"]);
		spin_from_dist = data["from_dist"];
		spin_from_angle_rad = data["from_angle_deg"].get<float>() * M_PI / 180.0f;
		spin_s_per_rev = data["s_per_rev"];

		Vec3 fore = findCrossVec(spin_up);
		Vec3 right = spin_up.cross(fore);
		right /= right.norm();

		world_from_spin.col(0) = fore;
		world_from_spin.col(1) = right;
		world_from_spin.col(2) = spin_up;

		float camera_spin_yaw_rad = 0.0f;
		Vec3 offset = Vec3(-cos(camera_spin_yaw_rad)*cos(spin_from_angle_rad),sin(camera_spin_yaw_rad)*cos(spin_from_angle_rad),-sin(spin_from_angle_rad))*spin_from_dist;
		offset = world_from_spin * offset;
		window.setCamFromWorld(makeLookAtTransform(spin_to, spin_to + offset, spin_up));
	}
	virtual void update(Window& window, float dt, const ControlInfo& control_info) override
	{
		Vec3 offset = Vec3(-cos(camera_spin_yaw_rad)*cos(spin_from_angle_rad),sin(camera_spin_yaw_rad)*cos(spin_from_angle_rad),-sin(spin_from_angle_rad))*spin_from_dist;
		offset = world_from_spin * offset;

		window.setCamFromWorld(makeLookAtTransform(spin_to, spin_to + offset, spin_up));

		camera_spin_yaw_rad += dt / spin_s_per_rev * 2 * M_PI;
		while (camera_spin_yaw_rad > 2 * M_PI)
			camera_spin_yaw_rad -= 2 * M_PI;
	}
	virtual json serialize() override
	{
		return {
			{"type", "spin"},
			{"to", toJson(spin_to)},
			{"up", toJson(spin_up)},
			{"from_dist", spin_from_dist},
			{"from_angle_def", spin_from_angle_rad* 180.0f / M_PI},
			{"s_per_rev", spin_s_per_rev}
		};
	}
private:
	Vec3 spin_to, spin_up;
	float spin_from_dist, spin_from_angle_rad, spin_s_per_rev;
	Mat3 world_from_spin;
	float camera_spin_yaw_rad = 0;
};