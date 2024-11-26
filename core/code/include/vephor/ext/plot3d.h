/**
 * Copyright 2023
 * Carnegie Robotics, LLC
 * 4501 Hatfield Street, Pittsburgh, PA 15201
 * https://www.carnegierobotics.com
 *
 * This code is provided under the terms of the Master Services Agreement (the Agreement).
 * This code constitutes CRL Background Intellectual Property, as defined in the Agreement.
**/

#pragma once

#include "window.h"
#include "plot.h"

namespace vephor
{

class Plot3D
{
public:
    Plot3D(const string &title = "plot", int width = 800, int height = 800, int x_position = -1, int y_position = -1)
            : inner_window(/* width */ width,
                           /* height */ height,
                           /* x_position */ x_position,
                           /* y_position */ y_position,
                           /* title */ title)
	{
		inner_window.setPlot3DMode();
		base_asset_dir = getBaseAssetDir();

		color_cycle = {
			convertStringToColor("#1f77b4"),
			convertStringToColor("#ff7f0e"),
			convertStringToColor("#2ca02c"),
			convertStringToColor("#d62728"),
			convertStringToColor("#9467bd"),
			convertStringToColor("#8c564b"),
			convertStringToColor("#e377c2"),
			convertStringToColor("#7f7f7f"),
			convertStringToColor("#bcbd22"),
			convertStringToColor("#17becf")
		};
	}
	void title(const string& title_text)
	{
		inner_window.setTitle(title_text);
	}
	void back_color(const Color& color)
	{
		inner_window.getCameraControlInfo()["back_color"] = toJson(color.getRGB());
	}
	void fore_color(const Color& color)
	{
		inner_window.getCameraControlInfo()["fore_color"] = toJson(color.getRGB());
	}
	void grid_color(const Color& color)
	{
		inner_window.getCameraControlInfo()["grid_color"] = toJson(color.getRGB());
	}
	void xlabel(const string& text)
	{
		inner_window.getCameraControlInfo()["x_axis"] = text;
	}
	void ylabel(const string& text)
	{
		inner_window.getCameraControlInfo()["y_axis"] = text;
	}
    void zlabel(const string& text)
	{
		inner_window.getCameraControlInfo()["z_axis"] = text;
	}

	void plot(
		const VecXRef& x, 
		const VecXRef& y,
        const VecXRef& z,
		const PlotLineOptions& opts = PlotLineOptions())//cmap
	{
		if (x.rows() == 0)
			return;

		if (x.rows() != y.rows())
			throw std::runtime_error("X and Y size must match in plot.");

        if (z.rows() != z.rows())
			throw std::runtime_error("X and Z size must match in plot.");
		
		{
			Vec3 curr_color = opts.color.getRGB();
			if (curr_color[0] < 0)
			{
				curr_color = color_cycle[color_index % color_cycle.size()];
				color_index++;
			}
			
			MatX pts(x.rows(), 3);
			pts.col(0) = x;
			pts.col(1) = y;
            pts.col(2) = z;
			
			if (opts.thickness <= 0)
			{
				auto lines = make_shared<Lines>(pts);
				lines->setColor(curr_color);
				inner_window.add(lines);
			}
			else
			{
				auto lines = make_shared<ThickLines>(pts);
				lines->setColor(curr_color);
				lines->setLineWidth(opts.thickness);
				inner_window.add(lines);
			}
			
			if (!opts.label.empty())
				inner_window.getCameraControlInfo()["labels"].push_back({
					{"text",opts.label},
					{"type","line"},
					{"color",toJson(curr_color)}
				});

			plot_index++;
		}
	}
	void plot(
		const vector<float>& x, 
		const vector<float>& y,
        const vector<float>& z,
		const PlotLineOptions& opts = PlotLineOptions())
	{
		plot(
			VecXMap(x.data(), x.size()),
			VecXMap(y.data(), y.size()),
            VecXMap(z.data(), z.size()),
			opts
		);
	}
	void plot(
		const vector<Vec3>& xyz,
		const PlotLineOptions& opts = PlotLineOptions())
	{
		vector<float> x(xyz.size());
		vector<float> y(xyz.size());
        vector<float> z(xyz.size());

		for (int i = 0; i < xyz.size(); i++)
		{
			x[i] = xyz[i][0];
			y[i] = xyz[i][1];
            z[i] = xyz[i][2];
		}

		plot(
			VecXMap(x.data(), x.size()),
			VecXMap(y.data(), y.size()),
            VecXMap(z.data(), z.size()),
			opts
		);
	}
	Window& window()
	{
		return inner_window;
	}
	bool show(bool wait_close = true, bool wait_key = false)
	{
		return inner_window.render(wait_close, wait_key);
	}
	void save(const string& path)
	{
		inner_window.save(path);
	}
	void clear()
	{
		inner_window.clear();
		inner_window.getCameraControlInfo()["labels"] = {};
		inner_window.invalidateCameraControlInfo();
		color_index = 0;
		plot_index = 0;
	}
	int take_next_plot_index()
	{
		plot_index++;
		return plot_index;
	}
private:
	Window inner_window;
	string base_asset_dir;
	vector<Vec3> color_cycle;
	int color_index = 0;
	int plot_index = 0;
};

}