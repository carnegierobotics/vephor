/**
 * Copyright 2025
 * Carnegie Robotics, LLC
 * 4501 Hatfield Street, Pittsburgh, PA 15201
 * https://www.carnegierobotics.com
 *
 * This source code is licensed under the Apache License, Version 2.0
 * found in the LICENSE file in the root directory of this source tree.
**/

#include "vephor_ext.h"

using namespace vephor;

int main()
{
	{
		auto sphere_image = generateSimpleImage(
			Vec2i(128,128),
			Vec4(0,0,0,0));
		
		auto sphere_normal_image = generateSimpleImage(
			Vec2i(128,128),
			Vec3(0.5,0.5,1));
		
		Vec2 center(64,64);
		
		auto iter = sphere_image->getIter();
		while(!iter.atEnd())
		{
			const auto ind = iter.getIndex();
			
			Vec2 off = ind.cast<float>() + Vec2(0.5,0.5) - center;
			float dist = off.norm();
			
			if (dist < 64)
			{
				(*sphere_image)(ind) = Vec4u(255,255,255,255);
				
				Vec3 normal(off[0]/64, -off[1]/64, 1.0);
				(*sphere_normal_image)(ind) = ((normal*0.5 + Vec3(0.5,0.5,0.5))*255).cast<uint8_t>();
			}
			
			iter++;
		}
		
		saveImage(getTempDir()+"/sphere_sprite.png", *sphere_image);
		saveImage(getTempDir()+"/sphere_sprite_normal.png", *sphere_normal_image);
	}
	
	{
		auto diamond_image = generateSimpleImage(
			Vec2i(128,128),
			Vec4(0,0,0,0));
		
		auto diamond_normal_image = generateSimpleImage(
			Vec2i(128,128),
			Vec3(0.5,0.5,1));
			
		Vec2 center(64,64);
		
		auto iter = diamond_image->getIter();
		while(!iter.atEnd())
		{
			const auto ind = iter.getIndex();
			
			Vec2 off = ind.cast<float>() + Vec2(0.5,0.5) - center;
			
			
			if (off[0] > 0 && off[1] > 0)
			{
				if (off[1] + off[0] < 64)
				{
					(*diamond_image)(ind) = Vec4u(255,255,255,255);
					
					Vec3 normal(1.0, -1.0, 1.0);
					normal /= normal.norm();
					(*diamond_normal_image)(ind) = ((normal*0.5 + Vec3(0.5,0.5,0.5))*255).cast<uint8_t>();
				}
			}
			else if (off[0] < 0 && off[1] < 0)
			{
				if (-off[1] + -off[0] < 64)
				{
					(*diamond_image)(ind) = Vec4u(255,255,255,255);
					
					Vec3 normal(-1.0, 1.0, 1.0);
					normal /= normal.norm();
					(*diamond_normal_image)(ind) = ((normal*0.5 + Vec3(0.5,0.5,0.5))*255).cast<uint8_t>();
				}
			}
			else if (off[0] > 0 && off[1] < 0)
			{
				if (-off[1] + off[0] < 64)
				{
					(*diamond_image)(ind) = Vec4u(255,255,255,255);
					
					Vec3 normal(1.0, 1.0, 1.0);
					normal /= normal.norm();
					(*diamond_normal_image)(ind) = ((normal*0.5 + Vec3(0.5,0.5,0.5))*255).cast<uint8_t>();
				}
			}
			else if (off[0] < 0 && off[1] > 0)
			{
				if (off[1] + -off[0] < 64)
				{
					(*diamond_image)(ind) = Vec4u(255,255,255,255);
					
					Vec3 normal(-1.0, -1.0, 1.0);
					normal /= normal.norm();
					(*diamond_normal_image)(ind) = ((normal*0.5 + Vec3(0.5,0.5,0.5))*255).cast<uint8_t>();
				}
			}
			
			iter++;
		}
		
		saveImage(getTempDir()+"/diamond_sprite.png", *diamond_image);
		saveImage(getTempDir()+"/diamond_sprite_normal.png", *diamond_normal_image);
	}
	
	return 0;
}