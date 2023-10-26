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