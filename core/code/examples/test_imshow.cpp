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
	
	plt.title("imshow");
	
	plt.xlabel("X Axis");
	plt.ylabel("Y Axis");

	string base_asset_dir = getBaseAssetDir();

	auto image = generateCheckerboardImage(
		Vec2i(128,256), 
		Vec2i(4,8),
		Vec3(1,1,0),
		Vec3(1,0,1));
	plt.imshow(*image);

	auto image_2 = generateGradientImage(
		Vec2i(256,116),
		Vec3(1,1,0),
		Vec3(1,0,1));
	plt.imshow(*image_2, false, Vec2(140,0));

	auto image_3 = generateGradientImage(
		Vec2i(256,116),
		Vec4(1,0,1,0),
		Vec4(1,1,0,1));
	plt.imshow(*image_3, false, Vec2(140,140));

	auto image_4 = generateCheckerboardImage(
		Vec2i(128,256), 
		Vec2i(4,8),
		Vec4(1,1,0,0),
		Vec4(1,0,1,1));
	plt.imshow(*image_4, false, Vec2(408,0));
	
	saveImage(getTempDir()+"/write_test.png", *image);
	
	{
		vector<float> x;
		vector<float> y;
		for (int i = 0; i < 128; i++)
		{
			x.push_back(i);
			y.push_back(i*2);
		}
		PlotLineOptions opts;
		opts.label = "Lines 1";
		plt.plot(x,y,opts);
	}
	
	{
		vector<float> x;
		vector<float> y;
		for (int i = 0; i < 128; i++)
		{
			x.push_back(i);
			y.push_back(i*1.5);
		}
		PlotLineOptions opts;
		opts.label = "Lines 2";
		plt.plot(x,y,opts);
	}
	
	{
		vector<float> x;
		vector<float> y;
		for (int i = 0; i < 128; i++)
		{
			x.push_back(i);
			y.push_back(i);
		}
		PlotLineOptions opts;
		opts.label = "Lines 3";
		plt.plot(x,y,opts);
	}
	
	{
		vector<float> x;
		vector<float> y;
		for (int i = 0; i < 128; i++)
		{
			x.push_back(128-i);
			y.push_back(i*2);
		}
		PlotLineOptions opts;
		opts.label = "Lines 4";
		plt.plot(x,y,opts);
	}
	
	{
		vector<float> x;
		vector<float> y;
		for (int i = 0; i < 128; i++)
		{
			x.push_back(128-i);
			y.push_back(i*1.5);
		}
		PlotLineOptions opts;
		opts.label = "Lines 5";
		plt.plot(x,y,opts);
	}
	
	{
		vector<float> x;
		vector<float> y;
		for (int i = 0; i < 128; i++)
		{
			x.push_back(128-i);
			y.push_back(i);
		}
		PlotLineOptions opts;
		opts.label = "Lines 6";
		plt.plot(x,y,opts);
	}
	
	plt.rect(Vec2(268,128), 536, 256, Vec3(0,0.75,0.75), 3);
	plt.circle(Vec2(64,128), 64, Vec3(0.5,0.5,0.5), 8, 32);
	
	plt.show();
	
	return 0;
}