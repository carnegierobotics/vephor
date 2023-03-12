#include "vephor_ext.h"

using namespace vephor;

int main()
{
	Plot plt;
	
	plt.title("Squared");
	
	plt.xlabel("X Axis");
	plt.ylabel("Y Axis");
	
	{
		vector<float> x;
		vector<float> y;
		for (int i = 0; i < 100; i++)
		{
			x.push_back(i);
			y.push_back(i*i);
		}
		PlotLineOptions opts;
		opts.label = "Thick Line";
		opts.thickness = 0.025;
		plt.plot(x,y,opts);
	}
	
	{
		vector<float> x;
		vector<float> y;
		for (int i = 0; i < 100; i++)
		{
			x.push_back(i);
			y.push_back(i*i);
		}
		PlotLineOptions opts;
		opts.label = "Less Thick Line";
		opts.thickness = 0.01;
		plt.plot(x,y,opts);
	}

	{
		vector<float> x;
		vector<float> y;
		for (int i = 0; i < 100; i++)
		{
			x.push_back(i);
			y.push_back(i*i);
		}
		PlotLineOptions opts;
		opts.label = "Thin Line";
		plt.plot(x,y,opts);
	}
	
	{
		vector<float> x;
		vector<float> y;
		for (int i = 0; i < 100; i++)
		{
			x.push_back(i);
			y.push_back(i*i);
		}
		PlotScatterOptions opts;
		opts.marker = PlotScatterMarker::PLUS;
		opts.color = Vec3(0,0,1);
		opts.label = "Scatter";
		plt.scatter(x,y,opts);
	}

	plt.show();
	
	return 0;
}