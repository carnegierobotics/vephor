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

#include <random>

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

    {
        //
        // Plot a square root function with random colors assigned to each point
        //

        std::random_device rd;
        std::mt19937 rng(rd());

        std::uniform_real_distribution<float> y_dist(0, 10000);
        std::uniform_int_distribution<> c_dist(0, 255);

        vector<float> x;
        vector<float> y;
        vector<Color> c;

        for (int i = 0; i < 100; i++)
        {
            x.push_back(i);
            y.push_back(std::sqrt(static_cast<float>(100000 * i)));
            c.emplace_back(c_dist(rng), c_dist(rng), c_dist(rng));
        }

        PlotScatterOptions opts;
        opts.marker = PlotScatterMarker::CIRCLE;
        opts.label = "Scatter with Random Colors";
        plt.scatter_c(x, y, c, opts);
    }

    {
        //
        // Plot a square function with colors assigned via a y-value color map
        //

        vector<float> x;
        vector<float> y;
        for (int i = 0; i < 100; i++)
        {
            x.push_back(i);
            y.push_back(i * i);
        }
        PlotScatterOptions opts;
        opts.marker = PlotScatterMarker::DIAMOND;
        opts.colormap = ColorMap(ColorMap::Style::VIRIDIS);
        opts.label = "Scatter Colored by y-Value";
        plt.scatter_colormap(x, y, opts);
    }

    {
        //
        // Plot a square function with colors assigned via an x-value color map
        //

        vector<float> x;
        vector<float> y;
        vector<float> c;
        for (int i = 0; i < 100; i++)
        {
            x.push_back(i);
            y.push_back(5000.0f * std::sin(M_PI * i / 180.0f * 4.0f) + 5000.0f);
            c.push_back(i);
        }
        PlotScatterOptions opts;
        opts.marker = PlotScatterMarker::SQUARE;
        opts.colormap = ColorMap(ColorMap::Style::JET);
        opts.label = "Scatter Colored by x-Value";
        plt.scatter_colormap(x, y, c, opts);
    }

	plt.show();
	
	return 0;
}