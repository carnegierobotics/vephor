#include "vephor_ext.h"

using namespace vephor;

int main()
{
	Plot plt;
	
	plt.title("render 2d");
	
	plt.xlabel("X Axis");
	plt.ylabel("Y Axis");

	vector<Vec2> verts = {
        Vec2(0,0),
		Vec2(64,64),
		Vec2(64,-64),
        Vec2(-64,-64),
        Vec2(-64,64)
    };
	
    plt.polygon(verts, Vec4(1,1,0,0.75), -1);
	plt.polygon(verts, Vec3(1,0,1), 5);
	
	plt.circle(Vec2(0,64), 16, Vec3(0.25,0.25,0.75), -1, 32);
	plt.circle(Vec2(0,0), 32, Vec3(0.5,0.5,0.5), -1, 32);
	plt.circle(Vec2(48,0), 8, Vec3(0.25,0.75,0.25), -1, 32);
	plt.circle(Vec2(-48,0), 8, Vec3(0.75,0.25,0.25), -1, 32);
	plt.circle(Vec2(0,-48), 8, Vec3(0.75,0.75,0.75), -1, 32);
	
	plt.line({
			Vec2(-80,-104),
			Vec2(-64,-80),
			Vec2(0,-96),
			Vec2(64,-80),
			Vec2(80,-104)
		},
		Vec3(0.5,0.2,0.8),
		5
	);
	
	vector<Vec2> verts_2 = {
        Vec2(0,0),
		Vec2(-32,-32),
		Vec2(0,-16),
        Vec2(32,-32)
    };
	
    plt.polygon(verts_2, Vec4(0,1,1,0.25), -1);
	
	plt.equal();
	plt.show();
	
	return 0;
}