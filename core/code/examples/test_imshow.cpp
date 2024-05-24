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