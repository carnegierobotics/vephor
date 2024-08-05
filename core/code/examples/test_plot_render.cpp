/**
 * Copyright 2023
 * Carnegie Robotics, LLC
 * 4501 Hatfield Street, Pittsburgh, PA 15201
 * https://www.carnegierobotics.com
 *
 * This code is provided under the terms of the Master Services Agreement (the Agreement).
 * This code constitutes CRL Background Intellectual Property, as defined in the Agreement.
**/

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

    plt.arrow(/* start */ {-100, -100},
              /* end */ {100, 100},
              /* color */ {1.0F, 0.0F, 0.0F, 0.5F},
              /* radius */ 10);

    plt.arrowhead(/* center */ {0, 100},
                  /* heading */ M_PI_2,
                  /* color */ {0.0F, 1.0F, 0.0F},
                  /* radius */ 10);
    plt.arrowhead(/* center */ {-10, 100},
                  /* heading */ 5 * M_PI / 4,
                  /* color */ {0.0F, 0.0F, 1.0F},
                  /* radius */ 2);
    plt.arrowhead(/* center */ {10, 100},
                  /* heading */ 7 * M_PI / 4,
                  /* color */ {0.0F, 0.0F, 1.0F},
                  /* radius */ 2);

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