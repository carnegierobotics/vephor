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