/**
 * Copyright 2023
 * Carnegie Robotics, LLC
 * 4501 Hatfield Street, Pittsburgh, PA 15201
 * https://www.carnegierobotics.com
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the Carnegie Robotics, LLC nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL CARNEGIE ROBOTICS, LLC BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
**/

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