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

#include "vephor_ext.h"

using namespace vephor;

int main()
{
	Window window;
	
	window.setServerMode();
	
	double obj_mesh_dist = 5.0f;
	
	
	// Make a "clock"
	for (int i = 1; i <= 12; i++)
	{
		float angle = i / 12.0 * 2 * M_PI;
		
		Vec3 dir = Vec3(cos(angle), sin(angle), 0);
		auto text = make_shared<Text>(std::to_string(i));
		text->setAnchorCentered();
		window.add(text, dir * obj_mesh_dist * 1.5);
		
		vector<Vec3> verts = {
			Vec3(0,0,0),
			dir * obj_mesh_dist * 1.5
		};
		window.add(make_shared<Lines>(verts), Vec3::Zero());
	}
	
	// Use a mesh for the clock hand
	string base_asset_dir = getBaseAssetDir();
	auto obj_mesh = make_shared<ObjMesh>(base_asset_dir+"/assets/pyramid.obj");
	auto obj_mesh_node = window.add(obj_mesh, Transform3(Vec3(obj_mesh_dist,0,0)));
	
	float angle = 0;
	while (true)
	{
		angle += 0.01;
		obj_mesh_node->setPos(Vec3(cos(angle), sin(angle), 0) * obj_mesh_dist);
		obj_mesh_node->setOrient(Vec3(0,0,angle+M_PI));
		
		window.render(false /*wait*/);
		
		std::this_thread::sleep_for(std::chrono::milliseconds((int)(33)));
	}

	return 0;
}