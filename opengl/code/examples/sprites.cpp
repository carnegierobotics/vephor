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

#include "vephor.h"
#include "vephor_ogl.h"

using namespace vephor;

int main()
{
    Window window(-1, -1, "sprites");

    window.setFrameLock(60.0f);

	auto sprite_sheet_1 = window.loadTexture(getBaseAssetDir()+"/assets/sphere_sprite.png", false);
    auto normal_sprite_sheet_1 = window.loadTexture(getBaseAssetDir()+"/assets/sphere_sprite_normal.png", false);
	
	auto sprite_sheet_2 = window.loadTexture(getBaseAssetDir()+"/assets/diamond_sprite.png", false);
    auto normal_sprite_sheet_2 = window.loadTexture(getBaseAssetDir()+"/assets/diamond_sprite_normal.png", false);

	auto sprite_1 = make_shared<Sprite>(
		sprite_sheet_1
	);
    sprite_1->setNormalSpriteSheet(normal_sprite_sheet_1);
	window.add(sprite_1, Transform3(
		Vec3(0.0,0.0,0.0),
		Vec3(0.0,0.0,0.0)
	));
	
	auto sprite_2 = make_shared<Sprite>(
		sprite_sheet_2
	);
    sprite_2->setNormalSpriteSheet(normal_sprite_sheet_2);
	window.add(sprite_2, Transform3(
		Vec3(2.0,2.0,0.0),
		Vec3(0.0,0.0,0.0)
	));
	
	auto sprite_3 = make_shared<Sprite>(
		sprite_sheet_2
	);
    sprite_3->setNormalSpriteSheet(normal_sprite_sheet_2);
	window.add(sprite_3, Transform3(
		Vec3(-2.0,-2.0,0.0),
		Vec3(0.0,0.0,0.0)
	));
	
	auto sprite_4 = make_shared<Sprite>(
		sprite_sheet_1
	);
    sprite_4->setNormalSpriteSheet(normal_sprite_sheet_1);
	window.add(sprite_4, Transform3(
		Vec3(2.0,-2.0,0.0),
		Vec3(0.0,0.0,0.0)
	));
	
	auto sprite_5 = make_shared<Sprite>(
		sprite_sheet_1
	);
    sprite_5->setNormalSpriteSheet(normal_sprite_sheet_1);
	window.add(sprite_5, Transform3(
		Vec3(-2.0,2.0,0.0),
		Vec3(0.0,0.0,0.0)
	));
	
	auto sphere_mesh_data = formSphere(16, 16);
	
	auto sphere_1 = make_shared<Mesh>(
        sphere_mesh_data,
        Vec3(1.0,1.0,1.0));
    window.add(sphere_1, Transform3(
		Vec3(0.0,2.0,0.0),
		Vec3(0.0,0.0,0.0)
	));
	
	auto sphere_2 = make_shared<Mesh>(
        sphere_mesh_data,
        Vec3(1.0,1.0,1.0));
    window.add(sphere_2, Transform3(
		Vec3(0.0,-2.0,0.0),
		Vec3(0.0,0.0,0.0)
	));
	
	auto sphere_3 = make_shared<Mesh>(
        sphere_mesh_data,
        Vec3(1.0,1.0,1.0));
    window.add(sphere_3, Transform3(
		Vec3(2.0,0.0,0.0),
		Vec3(0.0,0.0,0.0)
	));
	
	auto sphere_4 = make_shared<Mesh>(
        sphere_mesh_data,
        Vec3(1.0,1.0,1.0));
    window.add(sphere_4, Transform3(
		Vec3(-2.0,0.0,0.0),
		Vec3(0.0,0.0,0.0)
	));
	
	
	
	auto overlay_sprite_tex = window.getTextureFromImage(*generateCheckerboardImage(Vec2i(256,256), Vec2i(16,16), Vec3(1.0,0.0,0.0), Vec3(0.0,0.0,1.0)));
	auto overlay_sprite = make_shared<Sprite>(overlay_sprite_tex);
	overlay_sprite->setDiffuse(Vec3::Zero());
	overlay_sprite->setAmbient(Vec3::Zero());
	overlay_sprite->setEmissive(Vec3(1,1,1));
	auto overlay_sprite_node = window.add(overlay_sprite, Vec3(200,200,0), true);
	overlay_sprite_node->setScale(200);
	
	
	
	MeshData overlay_mesh_data(6);
	overlay_mesh_data.addQuad2D(Vec2(0,0), Vec2(200,200), Vec2(0,0), Vec2(1,1));
	auto overlay_mesh = make_shared<Mesh>(overlay_mesh_data, Vec3(0.5,0.5,0.5));
	overlay_mesh->setCull(false);
	auto overlay_mesh_node = window.add(overlay_mesh, Vec3(400, 400, 0), true);
	
	


	Vec3 light_dir(0,0,1);
    light_dir /= light_dir.norm();
    auto dir_light = make_shared<DirLight>(light_dir, 0.4f);
    window.add(dir_light, Transform3());
	
    Mat4 proj = makePerspectiveProj(45, window.getSize(), 0.1f, 100.0f);
    window.setProjectionMatrix(proj);

    window.setCamFromWorld(makeLookAtTransform(
        Vec3(0,0,0),
        Vec3(0,0,-10),
        Vec3(1,0,0)
    ));

    float t = 0;
    while (window.render())
    {
		Vec3 light_dir(sin(t*0.3),sin(t*0.3),cos(t*0.3));
		dir_light->setDir(light_dir / light_dir.norm());
		
		Vec3 cam_focal_pt(0,0,0);
        float cam_angle = t*0.6;
        float cam_dist = 10.0;
        float cam_height = cos(t*0.5)*5.0;
        Vec3 cam_pos = cam_focal_pt+Vec3(cam_height,-sin(cam_angle)*cam_dist,-cos(cam_angle)*cam_dist);
            
        window.setCamFromWorld(makeLookAtTransform(
            cam_focal_pt,
            cam_pos,
            Vec3(1,0,0)
        ));
		
        t += 0.015;
    }

    return 0;
}