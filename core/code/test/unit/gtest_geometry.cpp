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

#include <gtest/gtest.h>
#include "vephor/base/geometry.h"

using namespace vephor;

TEST(geometry, in_tri)
{
    bool in_tri_ccw = isPointInTriCCW(
		Vec2(-1,-1),
		Vec2(1,-1),
		Vec2(0,1),
		Vec2(0,0)
	);
	
	ASSERT_TRUE(in_tri_ccw);
	
	bool in_tri_cw = isPointInTriCCW(
		Vec2(1,-1),
		Vec2(-1,-1),
		Vec2(0,1),
		Vec2(0,0)
	);
	
	ASSERT_FALSE(in_tri_cw);
}

TEST(geometry, out_tri)
{
    bool in_tri_ccw = isPointInTriCCW(
		Vec2(-1,-1),
		Vec2(1,-1),
		Vec2(0,1),
		Vec2(0,5)
	);
	
	ASSERT_FALSE(in_tri_ccw);
	
	bool in_tri_cw = isPointInTriCCW(
		Vec2(1,-1),
		Vec2(-1,-1),
		Vec2(0,1),
		Vec2(0,5)
	);
	
	ASSERT_FALSE(in_tri_cw);
}

TEST(geometry, is_poly_ccw)
{
	vector<Vec2> verts = {
		Vec2(-1,-1),
		Vec2(1,-1),
		Vec2(1,1),
		Vec2(-1,1)
	};
	
	bool is_ccw = isPolyCCW(verts);
	ASSERT_TRUE(is_ccw);
}

TEST(geometry, is_poly_cw)
{
	vector<Vec2> verts = {
		Vec2(-1,-1),
		Vec2(-1,1),
		Vec2(1,1),
		Vec2(1,-1)
	};
	
	bool is_cw = !isPolyCCW(verts);
	ASSERT_TRUE(is_cw);
}

TEST(geometry, is_concave_poly_ccw)
{
	vector<Vec2> verts = {
		Vec2(-1,-1),
		Vec2(1,-1),
		Vec2(1,1),
		Vec2(0,0),
		Vec2(-1,1)
	};
	
	bool is_ccw = isPolyCCW(verts);
	ASSERT_TRUE(is_ccw);
}

TEST(geometry, is_concave_poly_cw)
{
	vector<Vec2> verts = {
		Vec2(-1,-1),
		Vec2(-1,1),
		Vec2(0,0),
		Vec2(1,1),
		Vec2(1,-1)
	};
	
	bool is_cw = !isPolyCCW(verts);
	ASSERT_TRUE(is_cw);
}

TEST(geometry, find_ear_ccw)
{
	vector<Vec2> verts = {
		Vec2(-1,-1),
		Vec2(1,-1),
		Vec2(1,1),
		Vec2(-1,1)
	};
	
	bool is_ear = isEar(0, verts);
	ASSERT_TRUE(is_ear);
}

TEST(geometry, find_ear_cw)
{
	vector<Vec2> verts = {
		Vec2(-1,-1),
		Vec2(-1,1),
		Vec2(1,1),
		Vec2(1,-1)
	};
	
	bool is_ear = isEar(0, verts);
	ASSERT_FALSE(is_ear);
}


TEST(geometry, find_ear_concave_ccw)
{
	vector<Vec2> verts = {
		Vec2(-1,-1),
		Vec2(1,-1),
		Vec2(1,1),
		Vec2(0,0),
		Vec2(-1,1)
	};
	
	bool is_ear = isEar(3, verts);
	ASSERT_FALSE(is_ear);
}

TEST(geometry, find_ear_concave_cw)
{
	vector<Vec2> verts = {
		Vec2(-1,-1),
		Vec2(-1,1),
		Vec2(0,0),
		Vec2(1,1),
		Vec2(1,-1)
	};
	
	bool is_ear = isEar(2, verts);
	ASSERT_TRUE(is_ear);
}

TEST(geometry, find_ear_inner_point)
{
	vector<Vec2> verts = {
		Vec2(0,0),
		Vec2(-1,-1),
		Vec2(0,-0.25),
		Vec2(1,-1)
	};
	
	bool is_ear = isEar(0, verts);
	ASSERT_FALSE(is_ear);

	is_ear = isEar(1, verts);
	ASSERT_TRUE(is_ear);
}