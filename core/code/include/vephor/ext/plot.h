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

#include "vephor/base/string.h"
#include "vephor/base/thirdparty/Neargye/magic_enum/magic_enum.hpp"

namespace vephor
{

// string should be in #rrggbb format
inline Vec3 convertStringToColor(const string& str)
{
	return Vec3(
		strtol(str.substr(1,2).c_str(), NULL, 16)/255.0f,
		strtol(str.substr(3,2).c_str(), NULL, 16)/255.0f,
		strtol(str.substr(5,2).c_str(), NULL, 16)/255.0f
	);
}

struct PlotLineOptions
{
	string label = "";
	Color color = Color(Vec3(-1,-1,-1));
	string linestyle = "";
	float thickness = 0;
};

enum PlotScatterMarker
{
	POINT,
	CIRCLE,
	TRIANGLE_DOWN,
	TRIANGLE_UP,
	TRIANGLE_LEFT,
	TRIANGLE_RIGHT,
	SQUARE,
	DIAMOND,
	PENTAGON,
	HEXAGON,
	OCTAGON,
	STAR,
	PLUS,
	PLUS_THIN,
	CROSS,
	CROSS_THIN,
	ARROW_DOWN,
	ARROW_UP,
	ARROW_LEFT,
	ARROW_RIGHT,
	LINE_VERTICAL,
	LINE_HORIZONTAL,
	SLASH_FORWARD,
	SLASH_BACKWARD
};

struct PlotScatterOptions
{
	string label = "";
	Color color = Color(Vec3(-1,-1,-1));
	float size = 0.01;
	PlotScatterMarker marker = PlotScatterMarker::CIRCLE;
};

class Plot
{
public:
	Plot(const string& title = "plot", int width=800, int height=800)
	: inner_window(width,height,title)
	{
        std::cout << "[" << __FILE__ << "][" << __LINE__ << "][Plot::" << __func__ << "]\n"; 
		inner_window.setPlotMode(false /*equal*/);
		base_asset_dir = getBaseAssetDir();

        std::cout << "[" << __FILE__ << "][" << __LINE__ << "][Plot::" << __func__ << "]\n"; 
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
        std::cout << "[" << __FILE__ << "][" << __LINE__ << "][Plot::" << __func__ << "]\n"; 
	}
	void title(const string& title_text)
	{
        std::cout << "[" << __FILE__ << "][" << __LINE__ << "][Plot::" << __func__ << "]\n"; 
		inner_window.setTitle(title_text);
        std::cout << "[" << __FILE__ << "][" << __LINE__ << "][Plot::" << __func__ << "]\n"; 
	}
	void back_color(const Color& color)
	{
        std::cout << "[" << __FILE__ << "][" << __LINE__ << "][Plot::" << __func__ << "]\n"; 
		inner_window.getCameraControlInfo()["back_color"] = color.getRGB();
        std::cout << "[" << __FILE__ << "][" << __LINE__ << "][Plot::" << __func__ << "]\n"; 
	}
	void fore_color(const Color& color)
	{
        std::cout << "[" << __FILE__ << "][" << __LINE__ << "][Plot::" << __func__ << "]\n"; 
		inner_window.getCameraControlInfo()["fore_color"] = color.getRGB();
        std::cout << "[" << __FILE__ << "][" << __LINE__ << "][Plot::" << __func__ << "]\n"; 
	}
	void grid_color(const Color& color)
	{
        std::cout << "[" << __FILE__ << "][" << __LINE__ << "][Plot::" << __func__ << "]\n"; 
		inner_window.getCameraControlInfo()["grid_color"] = color.getRGB();
        std::cout << "[" << __FILE__ << "][" << __LINE__ << "][Plot::" << __func__ << "]\n"; 
	}
	void xlabel(const string& text)
	{
        std::cout << "[" << __FILE__ << "][" << __LINE__ << "][Plot::" << __func__ << "]\n"; 
		inner_window.getCameraControlInfo()["x_axis"] = text;
        std::cout << "[" << __FILE__ << "][" << __LINE__ << "][Plot::" << __func__ << "]\n"; 
	}
	void ylabel(const string& text)
	{
        std::cout << "[" << __FILE__ << "][" << __LINE__ << "][Plot::" << __func__ << "]\n"; 
		inner_window.getCameraControlInfo()["y_axis"] = text;
        std::cout << "[" << __FILE__ << "][" << __LINE__ << "][Plot::" << __func__ << "]\n"; 
	}
	void yflip(bool flip = true)
	{
        std::cout << "[" << __FILE__ << "][" << __LINE__ << "][Plot::" << __func__ << "]\n"; 
		inner_window.getCameraControlInfo()["y_flip"] = flip;
        std::cout << "[" << __FILE__ << "][" << __LINE__ << "][Plot::" << __func__ << "]\n"; 
	}
	void equal(bool is_equal = true)
	{
        std::cout << "[" << __FILE__ << "][" << __LINE__ << "][Plot::" << __func__ << "]\n"; 
		inner_window.getCameraControlInfo()["equal"] = is_equal;
        std::cout << "[" << __FILE__ << "][" << __LINE__ << "][Plot::" << __func__ << "]\n"; 
	}
	void limits(float min_x, float max_x, float min_y, float max_y)
	{
        std::cout << "[" << __FILE__ << "][" << __LINE__ << "][Plot::" << __func__ << "]\n"; 
		inner_window.getCameraControlInfo()["auto_fit"] = false;
		inner_window.getCameraControlInfo()["content_min"] = toJson(Vec3(min_x, min_y, 0));
		inner_window.getCameraControlInfo()["content_max"] = toJson(Vec3(max_x, max_y, 0));
		inner_window.getCameraControlInfo()["orig_content_min"] = toJson(Vec3(min_x, min_y, 0));
		inner_window.getCameraControlInfo()["orig_content_max"] = toJson(Vec3(max_x, max_y, 0));
        std::cout << "[" << __FILE__ << "][" << __LINE__ << "][Plot::" << __func__ << "]\n"; 
	}

	void plot(
		const VecXRef& x, 
		const MatXRef& y,
		const PlotLineOptions& opts = PlotLineOptions())//cmap
	{
        std::cout << "[" << __FILE__ << "][" << __LINE__ << "][Plot::" << __func__ << "]\n"; 
		if (x.rows() == 0)
        {
            std::cout << "[" << __FILE__ << "][" << __LINE__ << "][Plot::" << __func__ << "]\n";
            return;
        }

        std::cout << "[" << __FILE__ << "][" << __LINE__ << "][Plot::" << __func__ << "]\n"; 
		if (x.rows() != y.rows())
        {
            std::cout << "[" << __FILE__ << "][" << __LINE__ << "][Plot::" << __func__ << "]\n";
            throw std::runtime_error("X and Y size must match in plot.");
        }
		
        std::cout << "[" << __FILE__ << "][" << __LINE__ << "][Plot::" << __func__ << "]\n"; 
		for (int col = 0; col < y.cols(); col++)
		{
            std::cout << "[" << __FILE__ << "][" << __LINE__ << "][Plot::" << __func__ << "]\n"; 
			Vec3 curr_color = opts.color.getRGB();
            std::cout << "[" << __FILE__ << "][" << __LINE__ << "][Plot::" << __func__ << "]\n"; 
			if (curr_color[0] < 0)
			{
                std::cout << "[" << __FILE__ << "][" << __LINE__ << "][Plot::" << __func__ << "]\n"; 
				curr_color = color_cycle[color_index % color_cycle.size()];
                std::cout << "[" << __FILE__ << "][" << __LINE__ << "][Plot::" << __func__ << "]\n"; 
				color_index++;
			}
            std::cout << "[" << __FILE__ << "][" << __LINE__ << "][Plot::" << __func__ << "]\n"; 
			
			MatX pts(x.rows(), 3);
            std::cout << "[" << __FILE__ << "][" << __LINE__ << "][Plot::" << __func__ << "]\n"; 
			pts.setZero();
            std::cout << "[" << __FILE__ << "][" << __LINE__ << "][Plot::" << __func__ << "]\n"; 
			
			pts.col(0) = x;
            std::cout << "[" << __FILE__ << "][" << __LINE__ << "][Plot::" << __func__ << "]\n"; 
			pts.col(1) = y.col(col);
            std::cout << "[" << __FILE__ << "][" << __LINE__ << "][Plot::" << __func__ << "]\n"; 
			
			if (opts.thickness <= 0)
			{
                std::cout << "[" << __FILE__ << "][" << __LINE__ << "][Plot::" << __func__ << "]\n"; 
				auto lines = make_shared<Lines>(pts);
                std::cout << "[" << __FILE__ << "][" << __LINE__ << "][Plot::" << __func__ << "]\n"; 
				lines->setColor(curr_color);
                std::cout << "[" << __FILE__ << "][" << __LINE__ << "][Plot::" << __func__ << "]\n"; 
				inner_window.add(lines, Vec3(0,0,plot_index + 1));
			}
			else
			{
                std::cout << "[" << __FILE__ << "][" << __LINE__ << "][Plot::" << __func__ << "]\n"; 
				auto lines = make_shared<ThickLines>(pts);
                std::cout << "[" << __FILE__ << "][" << __LINE__ << "][Plot::" << __func__ << "]\n"; 
				lines->setColor(curr_color);
                std::cout << "[" << __FILE__ << "][" << __LINE__ << "][Plot::" << __func__ << "]\n"; 
				lines->setLineWidth(opts.thickness);
                std::cout << "[" << __FILE__ << "][" << __LINE__ << "][Plot::" << __func__ << "]\n"; 
				inner_window.add(lines, Vec3(0,0,plot_index + 1));
                std::cout << "[" << __FILE__ << "][" << __LINE__ << "][Plot::" << __func__ << "]\n"; 
			}
            std::cout << "[" << __FILE__ << "][" << __LINE__ << "][Plot::" << __func__ << "]\n";

            if (!opts.label.empty())
            {
                std::cout << "[" << __FILE__ << "][" << __LINE__ << "][Plot::" << __func__ << "]\n";
                inner_window.getCameraControlInfo()["labels"].push_back({
                                                                                {"text",  opts.label},
                                                                                {"type",  "line"},
                                                                                {"color", toJson(curr_color)}
                                                                        });
            }
            std::cout << "[" << __FILE__ << "][" << __LINE__ << "][Plot::" << __func__ << "]\n";

			plot_index++;
            std::cout << "[" << __FILE__ << "][" << __LINE__ << "][Plot::" << __func__ << "]\n";
		}
        std::cout << "[" << __FILE__ << "][" << __LINE__ << "][Plot::" << __func__ << "]\n";
	}
	void plot(
		const vector<float>& x, 
		const vector<float>& y,
		const PlotLineOptions& opts = PlotLineOptions())
	{
        std::cout << "[" << __FILE__ << "][" << __LINE__ << "][Plot::" << __func__ << "]\n";
		plot(
			VecXMap(x.data(), x.size()),
			MatXMap(y.data(), y.size(), 1),
			opts
		);
        std::cout << "[" << __FILE__ << "][" << __LINE__ << "][Plot::" << __func__ << "]\n";
	}
	void plot(
		const MatXRef& y,
		const PlotLineOptions& opts = PlotLineOptions())
	{
        std::cout << "[" << __FILE__ << "][" << __LINE__ << "][Plot::" << __func__ << "]\n";
		VecX x(y.size());
        std::cout << "[" << __FILE__ << "][" << __LINE__ << "][Plot::" << __func__ << "]\n";
		for (size_t i = 0; i < y.size(); i++)
		{
            std::cout << "[" << __FILE__ << "][" << __LINE__ << "][Plot::" << __func__ << "]\n";
			x[i] = i;
		}
        std::cout << "[" << __FILE__ << "][" << __LINE__ << "][Plot::" << __func__ << "]\n";

		plot(x,y,opts);
        std::cout << "[" << __FILE__ << "][" << __LINE__ << "][Plot::" << __func__ << "]\n";
	}
	void plot(
		const vector<Vec2>& xy,
		const PlotLineOptions& opts = PlotLineOptions())
	{
        std::cout << "[" << __FILE__ << "][" << __LINE__ << "][Plot::" << __func__ << "]\n";
		vector<float> x(xy.size());
		vector<float> y(xy.size());

        std::cout << "[" << __FILE__ << "][" << __LINE__ << "][Plot::" << __func__ << "]\n";
		for (int i = 0; i < xy.size(); i++)
		{
            std::cout << "[" << __FILE__ << "][" << __LINE__ << "][Plot::" << __func__ << "]\n";
			x[i] = xy[i][0];
			y[i] = xy[i][1];
		}
        std::cout << "[" << __FILE__ << "][" << __LINE__ << "][Plot::" << __func__ << "]\n";

		plot(
			VecXMap(x.data(), x.size()),
			MatXMap(y.data(), y.size(), 1),
			opts
		);
        std::cout << "[" << __FILE__ << "][" << __LINE__ << "][Plot::" << __func__ << "]\n";
	}
	void scatter(
		const VecXRef& x, 
		const MatXRef& y, 
		const PlotScatterOptions& opts = PlotScatterOptions())//TODO: color map
	{
        std::cout << "[" << __FILE__ << "][" << __LINE__ << "][Plot::" << __func__ << "]\n";
		if (x.rows() == 0)
        {
            std::cout << "[" << __FILE__ << "][" << __LINE__ << "][Plot::" << __func__ << "]\n";
            return;
        }

        std::cout << "[" << __FILE__ << "][" << __LINE__ << "][Plot::" << __func__ << "]\n";
		if (x.rows() > 1 && y.rows() == 1 && y.cols() == 1)
		{
            std::cout << "[" << __FILE__ << "][" << __LINE__ << "][Plot::" << __func__ << "]\n";
			v4print "Fill value detected.";
            std::cout << "[" << __FILE__ << "][" << __LINE__ << "][Plot::" << __func__ << "]\n";
			MatX expanded_y(x.rows(), 1);
            std::cout << "[" << __FILE__ << "][" << __LINE__ << "][Plot::" << __func__ << "]\n";
			expanded_y.fill(y(0,0));
            std::cout << "[" << __FILE__ << "][" << __LINE__ << "][Plot::" << __func__ << "]\n";
			scatter(x, expanded_y, opts);
            std::cout << "[" << __FILE__ << "][" << __LINE__ << "][Plot::" << __func__ << "]\n";
			return;
		}
        std::cout << "[" << __FILE__ << "][" << __LINE__ << "][Plot::" << __func__ << "]\n";

		if (x.rows() != y.rows())
        {
            std::cout << "[" << __FILE__ << "][" << __LINE__ << "][Plot::" << __func__ << "]\n";
            throw std::runtime_error("X and Y size must match in scatter.");
        }
		
        std::cout << "[" << __FILE__ << "][" << __LINE__ << "][Plot::" << __func__ << "]\n";
		MatX pts(x.rows(), 3);
        std::cout << "[" << __FILE__ << "][" << __LINE__ << "][Plot::" << __func__ << "]\n";
		pts.setZero();
        std::cout << "[" << __FILE__ << "][" << __LINE__ << "][Plot::" << __func__ << "]\n";
		
		for (int col = 0; col < y.cols(); col++)
		{
            std::cout << "[" << __FILE__ << "][" << __LINE__ << "][Plot::" << __func__ << "]\n";
			pts.col(0) = x;
			pts.col(1) = y.col(col);

            std::cout << "[" << __FILE__ << "][" << __LINE__ << "][Plot::" << __func__ << "]\n";
			Vec3 curr_color = opts.color.getRGB();
			if (curr_color[0] < 0)
			{
                std::cout << "[" << __FILE__ << "][" << __LINE__ << "][Plot::" << __func__ << "]\n";
				curr_color = color_cycle[color_index % color_cycle.size()];
				color_index++;
			}
            std::cout << "[" << __FILE__ << "][" << __LINE__ << "][Plot::" << __func__ << "]\n";
			
			auto particle = make_shared<Particle>(pts);
            std::cout << "[" << __FILE__ << "][" << __LINE__ << "][Plot::" << __func__ << "]\n";
			particle->setSize(opts.size);
            std::cout << "[" << __FILE__ << "][" << __LINE__ << "][Plot::" << __func__ << "]\n";
			particle->setScreenSpaceMode(true);
            std::cout << "[" << __FILE__ << "][" << __LINE__ << "][Plot::" << __func__ << "]\n";
			particle->setColor(curr_color);
            std::cout << "[" << __FILE__ << "][" << __LINE__ << "][Plot::" << __func__ << "]\n";
			
            std::string marker_name{magic_enum::enum_name(opts.marker)};
            std::cout << "[" << __FILE__ << "][" << __LINE__ << "][Plot::" << __func__ << "]\n";
            marker_name = to_lower(marker_name);
            std::cout << "[" << __FILE__ << "][" << __LINE__ << "][Plot::" << __func__ << "]\n";
            particle->setTexture(base_asset_dir + "/assets/" + marker_name + ".png", false);
            std::cout << "[" << __FILE__ << "][" << __LINE__ << "][Plot::" << __func__ << "]\n";
            inner_window.add(particle, Vec3(0, 0, plot_index + 1));
            std::cout << "[" << __FILE__ << "][" << __LINE__ << "][Plot::" << __func__ << "]\n";
			
			if (!opts.label.empty())
            {
                std::cout << "[" << __FILE__ << "][" << __LINE__ << "][Plot::" << __func__ << "]\n";
                inner_window.getCameraControlInfo()["labels"].push_back({
                                                                                {"text",  opts.label},
                                                                                {"type",  marker_name},
                                                                                {"color", toJson(curr_color)}
                                                                        });
            }
            std::cout << "[" << __FILE__ << "][" << __LINE__ << "][Plot::" << __func__ << "]\n";

			plot_index++;
            std::cout << "[" << __FILE__ << "][" << __LINE__ << "][Plot::" << __func__ << "]\n";
		}
        std::cout << "[" << __FILE__ << "][" << __LINE__ << "][Plot::" << __func__ << "]\n";
	}
	void scatter(
		const vector<float>& x, 
		const vector<float>& y,
		const PlotScatterOptions& opts = PlotScatterOptions())
	{
        std::cout << "[" << __FILE__ << "][" << __LINE__ << "][Plot::" << __func__ << "]\n";
		scatter(
			VecXMap(x.data(), x.size()),
			MatXMap(y.data(), y.size(), 1),
			opts
		);
        std::cout << "[" << __FILE__ << "][" << __LINE__ << "][Plot::" << __func__ << "]\n";
	}
	void scatter(
		const MatXRef& y,
		const PlotScatterOptions& opts = PlotScatterOptions())
	{
        std::cout << "[" << __FILE__ << "][" << __LINE__ << "][Plot::" << __func__ << "]\n";
		VecX x(y.size());
		for (size_t i = 0; i < y.size(); i++)
		{
			x[i] = i;
		}
        std::cout << "[" << __FILE__ << "][" << __LINE__ << "][Plot::" << __func__ << "]\n";

		scatter(x,y,opts);
        std::cout << "[" << __FILE__ << "][" << __LINE__ << "][Plot::" << __func__ << "]\n";
	}
	void scatter(
		const vector<Vec2>& xy,
		const PlotScatterOptions& opts = PlotScatterOptions())
	{
        std::cout << "[" << __FILE__ << "][" << __LINE__ << "][Plot::" << __func__ << "]\n";
		vector<float> x(xy.size());
		vector<float> y(xy.size());

        std::cout << "[" << __FILE__ << "][" << __LINE__ << "][Plot::" << __func__ << "]\n";
		for (int i = 0; i < xy.size(); i++)
		{
			x[i] = xy[i][0];
			y[i] = xy[i][1];
		}

        std::cout << "[" << __FILE__ << "][" << __LINE__ << "][Plot::" << __func__ << "]\n";
		scatter(
			VecXMap(x.data(), x.size()),
			MatXMap(y.data(), y.size(), 1),
			opts
		);
        std::cout << "[" << __FILE__ << "][" << __LINE__ << "][Plot::" << __func__ << "]\n";
	}
    void label(const string &text, const Color &color, const PlotScatterMarker marker)
    {
        std::cout << "[" << __FILE__ << "][" << __LINE__ << "][Plot::" << __func__ << "]\n";
        const std::string marker_name{to_lower(magic_enum::enum_name(marker))};
        std::cout << "[" << __FILE__ << "][" << __LINE__ << "][Plot::" << __func__ << "]\n";
        inner_window.getCameraControlInfo()["labels"].push_back({
                                                                        {"text",  text},
                                                                        {"type",  marker_name},
                                                                        {"color", toJson(color.getRGB())}
                                                                });
        std::cout << "[" << __FILE__ << "][" << __LINE__ << "][Plot::" << __func__ << "]\n";
        inner_window.invalidateCameraControlInfo();
        std::cout << "[" << __FILE__ << "][" << __LINE__ << "][Plot::" << __func__ << "]\n";
    }
    void label(const string &text, const Color &color, const string &marker_name = "circle")
    {
        std::cout << "[" << __FILE__ << "][" << __LINE__ << "][Plot::" << __func__ << "]\n";
        const auto marker = magic_enum::enum_cast<PlotScatterMarker>(marker_name,
                                                                     magic_enum::case_insensitive);
        std::cout << "[" << __FILE__ << "][" << __LINE__ << "][Plot::" << __func__ << "]\n";
        if (!marker.has_value())
        {
			throw std::runtime_error("[vephor::Plot::label()] No marker with name " + marker_name);
        }

        std::cout << "[" << __FILE__ << "][" << __LINE__ << "][Plot::" << __func__ << "]\n";
        label(text, color, marker.value());
        std::cout << "[" << __FILE__ << "][" << __LINE__ << "][Plot::" << __func__ << "]\n";
    }

    void text(const string &raw_text, float size, const Vec2 &offset, const Color &color)
    {
        std::cout << "[" << __FILE__ << "][" << __LINE__ << "][Plot::" << __func__ << "]\n";
		auto text = make_shared<Text>(raw_text);
        std::cout << "[" << __FILE__ << "][" << __LINE__ << "][Plot::" << __func__ << "]\n";
		text->setAnchorCentered();
        std::cout << "[" << __FILE__ << "][" << __LINE__ << "][Plot::" << __func__ << "]\n";
		text->setColor(color);
        std::cout << "[" << __FILE__ << "][" << __LINE__ << "][Plot::" << __func__ << "]\n";
		text->setYFlip(inner_window.getCameraControlInfo()["y_flip"]);
        std::cout << "[" << __FILE__ << "][" << __LINE__ << "][Plot::" << __func__ << "]\n";
		auto node = inner_window.add(text, Vec3(offset[0],offset[1],plot_index + 1));
        std::cout << "[" << __FILE__ << "][" << __LINE__ << "][Plot::" << __func__ << "]\n";
		node->setScale(size);
        std::cout << "[" << __FILE__ << "][" << __LINE__ << "][Plot::" << __func__ << "]\n";
		plot_index++;
        std::cout << "[" << __FILE__ << "][" << __LINE__ << "][Plot::" << __func__ << "]\n";
	}
	void polygon(const vector<Vec2>& verts, const Color& color, float thickness = 0)
	{
        std::cout << "[" << __FILE__ << "][" << __LINE__ << "][Plot::" << __func__ << "]\n";
		if (verts.empty())
			return;

        std::cout << "[" << __FILE__ << "][" << __LINE__ << "][Plot::" << __func__ << "]\n";
		// Non-negative border widths indicate border should be drawn
		if (thickness >= 0)
		{
            std::cout << "[" << __FILE__ << "][" << __LINE__ << "][Plot::" << __func__ << "]\n";
			if (thickness == 0) // Border is a thin line
			{
                std::cout << "[" << __FILE__ << "][" << __LINE__ << "][Plot::" << __func__ << "]\n";
				MatX pts(verts.size() + 1, 3);

                std::cout << "[" << __FILE__ << "][" << __LINE__ << "][Plot::" << __func__ << "]\n";
				for (size_t i = 0; i < verts.size(); i++)
				{
					pts.row(i) = Vec3(verts[i][0], verts[i][1], 0);
				}
                std::cout << "[" << __FILE__ << "][" << __LINE__ << "][Plot::" << __func__ << "]\n";
				pts.row(verts.size()) = Vec3(verts[0][0], verts[0][1], 0);
                std::cout << "[" << __FILE__ << "][" << __LINE__ << "][Plot::" << __func__ << "]\n";

                std::cout << "[" << __FILE__ << "][" << __LINE__ << "][Plot::" << __func__ << "]\n";
				auto lines = make_shared<Lines>(pts);
                std::cout << "[" << __FILE__ << "][" << __LINE__ << "][Plot::" << __func__ << "]\n";
				lines->setColor(color);
                std::cout << "[" << __FILE__ << "][" << __LINE__ << "][Plot::" << __func__ << "]\n";
				inner_window.add(lines, Vec3(0,0,plot_index + 1));
                std::cout << "[" << __FILE__ << "][" << __LINE__ << "][Plot::" << __func__ << "]\n";
			}
			else
			{
                std::cout << "[" << __FILE__ << "][" << __LINE__ << "][Plot::" << __func__ << "]\n";
				auto data = formLineLoop(verts, thickness / 2.0f);
                std::cout << "[" << __FILE__ << "][" << __LINE__ << "][Plot::" << __func__ << "]\n";
				auto mesh = make_shared<Mesh>(data);
                std::cout << "[" << __FILE__ << "][" << __LINE__ << "][Plot::" << __func__ << "]\n";
				mesh->setColor(color);
                std::cout << "[" << __FILE__ << "][" << __LINE__ << "][Plot::" << __func__ << "]\n";
				mesh->setCull(false);
                std::cout << "[" << __FILE__ << "][" << __LINE__ << "][Plot::" << __func__ << "]\n";
				mesh->setSpecular(0.0f);
                std::cout << "[" << __FILE__ << "][" << __LINE__ << "][Plot::" << __func__ << "]\n";
				mesh->setDiffuseStrength(0.0f);
                std::cout << "[" << __FILE__ << "][" << __LINE__ << "][Plot::" << __func__ << "]\n";
				mesh->setAmbientStrength(0.0f);
                std::cout << "[" << __FILE__ << "][" << __LINE__ << "][Plot::" << __func__ << "]\n";
				mesh->setEmissiveStrength(1.0f);
                std::cout << "[" << __FILE__ << "][" << __LINE__ << "][Plot::" << __func__ << "]\n";
				inner_window.add(mesh, Vec3(0,0,plot_index + 1));
                std::cout << "[" << __FILE__ << "][" << __LINE__ << "][Plot::" << __func__ << "]\n";
			}
            std::cout << "[" << __FILE__ << "][" << __LINE__ << "][Plot::" << __func__ << "]\n";
		}
		else
		{
            std::cout << "[" << __FILE__ << "][" << __LINE__ << "][Plot::" << __func__ << "]\n";
			// Negative border width means this is a fill

			// Triangulate an arbitrary polygon
			auto data = formPolygon(verts);
            std::cout << "[" << __FILE__ << "][" << __LINE__ << "][Plot::" << __func__ << "]\n";
			auto mesh = make_shared<Mesh>(data);
            std::cout << "[" << __FILE__ << "][" << __LINE__ << "][Plot::" << __func__ << "]\n";
			mesh->setColor(color);
            std::cout << "[" << __FILE__ << "][" << __LINE__ << "][Plot::" << __func__ << "]\n";
			mesh->setCull(false);
            std::cout << "[" << __FILE__ << "][" << __LINE__ << "][Plot::" << __func__ << "]\n";
			mesh->setSpecular(0.0f);
            std::cout << "[" << __FILE__ << "][" << __LINE__ << "][Plot::" << __func__ << "]\n";
			mesh->setDiffuseStrength(0.0f);
            std::cout << "[" << __FILE__ << "][" << __LINE__ << "][Plot::" << __func__ << "]\n";
			mesh->setAmbientStrength(0.0f);
            std::cout << "[" << __FILE__ << "][" << __LINE__ << "][Plot::" << __func__ << "]\n";
			mesh->setEmissiveStrength(1.0f);
            std::cout << "[" << __FILE__ << "][" << __LINE__ << "][Plot::" << __func__ << "]\n";
			inner_window.add(mesh, Vec3(0,0,plot_index + 1));
            std::cout << "[" << __FILE__ << "][" << __LINE__ << "][Plot::" << __func__ << "]\n";
		}
        std::cout << "[" << __FILE__ << "][" << __LINE__ << "][Plot::" << __func__ << "]\n";

		plot_index++;
        std::cout << "[" << __FILE__ << "][" << __LINE__ << "][Plot::" << __func__ << "]\n";
	}
	void polygon_d(const vector<Vec2d>& verts, const Color& color, float thickness = 0)
	{
        std::cout << "[" << __FILE__ << "][" << __LINE__ << "][Plot::" << __func__ << "]\n";
		vector<Vec2> verts_f;
		for (const auto& v : verts)
		{
			verts_f.push_back(v.cast<float>());
		}
        std::cout << "[" << __FILE__ << "][" << __LINE__ << "][Plot::" << __func__ << "]\n";
		polygon(verts_f, color, thickness);
        std::cout << "[" << __FILE__ << "][" << __LINE__ << "][Plot::" << __func__ << "]\n";
	}
	void circle(const Vec2& center, float rad, const Color& color, float thickness = 0, int slices = 16)
	{
        std::cout << "[" << __FILE__ << "][" << __LINE__ << "][Plot::" << __func__ << "]\n";
		vector<Vec2> verts;

        std::cout << "[" << __FILE__ << "][" << __LINE__ << "][Plot::" << __func__ << "]\n";
		for (int slice = 0; slice < slices; slice++)
		{
			float angle = slice / (float)slices * (2 * M_PI);
			verts.push_back(Vec2(cos(angle),sin(angle)) * rad + center);
		}
        std::cout << "[" << __FILE__ << "][" << __LINE__ << "][Plot::" << __func__ << "]\n";

		polygon(verts, color, thickness);
        std::cout << "[" << __FILE__ << "][" << __LINE__ << "][Plot::" << __func__ << "]\n";
	}
	void circle_d(const Vec2d& center, float rad, const Color& color, float thickness = 0, int slices = 16)
	{
        std::cout << "[" << __FILE__ << "][" << __LINE__ << "][Plot::" << __func__ << "]\n";
		circle((Vec2)center.cast<float>(), rad, color, thickness, slices);
        std::cout << "[" << __FILE__ << "][" << __LINE__ << "][Plot::" << __func__ << "]\n";
	}
	void rect(const Vec2& center, float width, float height, const Color& color, float thickness = 0)
	{
        std::cout << "[" << __FILE__ << "][" << __LINE__ << "][Plot::" << __func__ << "]\n";
		vector<Vec2> verts;
        std::cout << "[" << __FILE__ << "][" << __LINE__ << "][Plot::" << __func__ << "]\n";
		verts.push_back(Vec2(-width/2.0f,-height/2.0f) + center);
        std::cout << "[" << __FILE__ << "][" << __LINE__ << "][Plot::" << __func__ << "]\n";
		verts.push_back(Vec2(width/2.0f,-height/2.0f) + center);
        std::cout << "[" << __FILE__ << "][" << __LINE__ << "][Plot::" << __func__ << "]\n";
		verts.push_back(Vec2(width/2.0f,height/2.0f) + center);
        std::cout << "[" << __FILE__ << "][" << __LINE__ << "][Plot::" << __func__ << "]\n";
		verts.push_back(Vec2(-width/2.0f,height/2.0f) + center);
        std::cout << "[" << __FILE__ << "][" << __LINE__ << "][Plot::" << __func__ << "]\n";

		polygon(verts, color, thickness);
        std::cout << "[" << __FILE__ << "][" << __LINE__ << "][Plot::" << __func__ << "]\n";
	}
	void line(const vector<Vec2>& verts, const Color& color, float thickness = 0)
	{
        std::cout << "[" << __FILE__ << "][" << __LINE__ << "][Plot::" << __func__ << "]\n";
		if (verts.empty())
			return;

        std::cout << "[" << __FILE__ << "][" << __LINE__ << "][Plot::" << __func__ << "]\n";
		if (thickness == 0) // A thin line
		{
            std::cout << "[" << __FILE__ << "][" << __LINE__ << "][Plot::" << __func__ << "]\n";
			MatX pts(2, 3);

            std::cout << "[" << __FILE__ << "][" << __LINE__ << "][Plot::" << __func__ << "]\n";
			pts.row(0) = Vec3(verts[0][0], verts[0][1], 0);
			pts.row(1) = Vec3(verts[1][0], verts[1][1], 0);

            std::cout << "[" << __FILE__ << "][" << __LINE__ << "][Plot::" << __func__ << "]\n";
			auto lines = make_shared<Lines>(pts);
            std::cout << "[" << __FILE__ << "][" << __LINE__ << "][Plot::" << __func__ << "]\n";
			lines->setColor(color);
            std::cout << "[" << __FILE__ << "][" << __LINE__ << "][Plot::" << __func__ << "]\n";
			inner_window.add(lines, Vec3(0,0,plot_index + 1));
            std::cout << "[" << __FILE__ << "][" << __LINE__ << "][Plot::" << __func__ << "]\n";
		}
		else
		{
            std::cout << "[" << __FILE__ << "][" << __LINE__ << "][Plot::" << __func__ << "]\n";
			auto data = formLine(verts, thickness / 2.0f);
            std::cout << "[" << __FILE__ << "][" << __LINE__ << "][Plot::" << __func__ << "]\n";
			auto mesh = make_shared<Mesh>(data);
            std::cout << "[" << __FILE__ << "][" << __LINE__ << "][Plot::" << __func__ << "]\n";
			mesh->setColor(color);
            std::cout << "[" << __FILE__ << "][" << __LINE__ << "][Plot::" << __func__ << "]\n";
			mesh->setCull(false);
            std::cout << "[" << __FILE__ << "][" << __LINE__ << "][Plot::" << __func__ << "]\n";
			mesh->setSpecular(0.0f);
            std::cout << "[" << __FILE__ << "][" << __LINE__ << "][Plot::" << __func__ << "]\n";
			inner_window.add(mesh, Vec3(0,0,plot_index + 1));
            std::cout << "[" << __FILE__ << "][" << __LINE__ << "][Plot::" << __func__ << "]\n";
		}

        std::cout << "[" << __FILE__ << "][" << __LINE__ << "][Plot::" << __func__ << "]\n";
		plot_index++;
        std::cout << "[" << __FILE__ << "][" << __LINE__ << "][Plot::" << __func__ << "]\n";
	}
	void line(const Vec2& start, const Vec2& end, const Color& color, float thickness = 0)
	{
        std::cout << "[" << __FILE__ << "][" << __LINE__ << "][Plot::" << __func__ << "]\n";
		line({start, end}, color, thickness);
        std::cout << "[" << __FILE__ << "][" << __LINE__ << "][Plot::" << __func__ << "]\n";
	}
	void line_d(const Vec2d& start, const Vec2d& end, const Color& color, float thickness = 0)
	{
        std::cout << "[" << __FILE__ << "][" << __LINE__ << "][Plot::" << __func__ << "]\n";
		line({start.cast<float>(), end.cast<float>()}, color, thickness);
        std::cout << "[" << __FILE__ << "][" << __LINE__ << "][Plot::" << __func__ << "]\n";
	}
    void arrow(const Vec2 &start, const Vec2 &end, const Color &color, const float radius = 1.0F)
    {
        std::cout << "[" << __FILE__ << "][" << __LINE__ << "][Plot::" << __func__ << "]\n";
		Vec2 vec = end - start;

        std::cout << "[" << __FILE__ << "][" << __LINE__ << "][Plot::" << __func__ << "]\n";
		float vec_mag = vec.norm();

        std::cout << "[" << __FILE__ << "][" << __LINE__ << "][Plot::" << __func__ << "]\n";
		if (vec_mag < 1e-3)
			return;

        std::cout << "[" << __FILE__ << "][" << __LINE__ << "][Plot::" << __func__ << "]\n";
		vec /= vec_mag;


        std::cout << "[" << __FILE__ << "][" << __LINE__ << "][Plot::" << __func__ << "]\n";
		float head_radius = radius * 0.5;
		float shaft_radius = radius * 0.2;


        std::cout << "[" << __FILE__ << "][" << __LINE__ << "][Plot::" << __func__ << "]\n";
		Vec2 cross(-vec[1], vec[0]);

        std::cout << "[" << __FILE__ << "][" << __LINE__ << "][Plot::" << __func__ << "]\n";
		vector<Vec2> verts;
		verts.push_back(start - cross * shaft_radius);
		verts.push_back(end - vec * head_radius - cross * shaft_radius);
		verts.push_back(end - vec * head_radius - cross * head_radius);
		verts.push_back(end);
		verts.push_back(end - vec * head_radius + cross * head_radius);
		verts.push_back(end - vec * head_radius + cross * shaft_radius);
		verts.push_back(start + cross * shaft_radius);

        std::cout << "[" << __FILE__ << "][" << __LINE__ << "][Plot::" << __func__ << "]\n";
		polygon(verts, color, -1);
		
        std::cout << "[" << __FILE__ << "][" << __LINE__ << "][Plot::" << __func__ << "]\n";

        plot_index++;
        std::cout << "[" << __FILE__ << "][" << __LINE__ << "][Plot::" << __func__ << "]\n";
    }
    void arrowhead(const Vec2 &center, const float heading, const Color &color, const float radius = 1.0F)
    {
        std::cout << "[" << __FILE__ << "][" << __LINE__ << "][Plot::" << __func__ << "]\n";
        const auto offset = radius / 2;
        std::cout << "[" << __FILE__ << "][" << __LINE__ << "][Plot::" << __func__ << "]\n";
        const Vec2 unit{std::cos(heading), std::sin(heading)};
        std::cout << "[" << __FILE__ << "][" << __LINE__ << "][Plot::" << __func__ << "]\n";
        arrow(/* start */ center - offset * unit,
              /* end */ center + offset * unit,
              /* color */ color,
              /* radius */ radius);
        std::cout << "[" << __FILE__ << "][" << __LINE__ << "][Plot::" << __func__ << "]\n";
    }
	void imshow(const Image<uint8_t>& p_image, bool p_filter_nearest = false, const Vec2& offset = Vec2(0,0))
	{
        std::cout << "[" << __FILE__ << "][" << __LINE__ << "][Plot::" << __func__ << "]\n";
		shared_ptr<Sprite> sprite;
        std::cout << "[" << __FILE__ << "][" << __LINE__ << "][Plot::" << __func__ << "]\n";
		sprite = make_shared<Sprite>(p_image, p_filter_nearest);
        std::cout << "[" << __FILE__ << "][" << __LINE__ << "][Plot::" << __func__ << "]\n";
		sprite->setFlip(false, true);
        std::cout << "[" << __FILE__ << "][" << __LINE__ << "][Plot::" << __func__ << "]\n";
		auto node = inner_window.add(sprite, Vec3(
			p_image.getSize()[0]/2.0f+offset[0],
			p_image.getSize()[1]/2.0f+offset[1],
			0.5), false, 0);
        std::cout << "[" << __FILE__ << "][" << __LINE__ << "][Plot::" << __func__ << "]\n";
		node->setScale(p_image.getSize()[1]);
        std::cout << "[" << __FILE__ << "][" << __LINE__ << "][Plot::" << __func__ << "]\n";
		
		inner_window.getCameraControlInfo()["equal"] = true;
        std::cout << "[" << __FILE__ << "][" << __LINE__ << "][Plot::" << __func__ << "]\n";
		inner_window.getCameraControlInfo()["y_flip"] = true;
        std::cout << "[" << __FILE__ << "][" << __LINE__ << "][Plot::" << __func__ << "]\n";
	}
	Window& window()
	{
        std::cout << "[" << __FILE__ << "][" << __LINE__ << "][Plot::" << __func__ << "]\n";
		return inner_window;
	}
	bool show(bool wait_close = true, bool wait_key = false)
	{
        std::cout << "[" << __FILE__ << "][" << __LINE__ << "][Plot::" << __func__ << "]\n";
		return inner_window.render(wait_close, wait_key);
	}
	void save(const string& path)
	{
        std::cout << "[" << __FILE__ << "][" << __LINE__ << "][Plot::Plot::" << __func__ << "]\n";
		inner_window.save(path);
	}
	void clear()
	{
        std::cout << "[" << __FILE__ << "][" << __LINE__ << "][Plot::Plot::" << __func__ << "]\n";
		inner_window.clear();
        std::cout << "[" << __FILE__ << "][" << __LINE__ << "][Plot::" << __func__ << "]\n";
		inner_window.getCameraControlInfo()["labels"] = {};
        std::cout << "[" << __FILE__ << "][" << __LINE__ << "][Plot::" << __func__ << "]\n";
		inner_window.invalidateCameraControlInfo();
        std::cout << "[" << __FILE__ << "][" << __LINE__ << "][Plot::" << __func__ << "]\n";
		color_index = 0;
		plot_index = 0;
        std::cout << "[" << __FILE__ << "][" << __LINE__ << "][Plot::" << __func__ << "]\n";
	}
	int take_next_plot_index()
	{
        std::cout << "[" << __FILE__ << "][" << __LINE__ << "][Plot::" << __func__ << "]\n";
		plot_index++;
        std::cout << "[" << __FILE__ << "][" << __LINE__ << "][Plot::" << __func__ << "]\n";
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