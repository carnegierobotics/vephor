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