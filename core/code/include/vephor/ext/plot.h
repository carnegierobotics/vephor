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
	CIRCLE,
	DIAMOND,
	PLUS,
	SQUARE
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
		inner_window.setPlotMode(false /*equal*/);
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
	void xlabel(const string& text)
	{
		inner_window.getCameraControlInfo()["x_axis"] = text;
	}
	void ylabel(const string& text)
	{
		inner_window.getCameraControlInfo()["y_axis"] = text;
	}
	void yflip(bool flip = true)
	{
		inner_window.getCameraControlInfo()["y_flip"] = flip;
	}
	void equal(bool is_equal = true)
	{
		inner_window.getCameraControlInfo()["equal"] = is_equal;
	}
	void limits(float min_x, float max_x, float min_y, float max_y)
	{
		inner_window.getCameraControlInfo()["auto_fit"] = false;
		inner_window.getCameraControlInfo()["content_min"] = toJson(Vec3(min_x, min_y, 0));
		inner_window.getCameraControlInfo()["content_max"] = toJson(Vec3(max_x, max_y, 0));
		inner_window.getCameraControlInfo()["orig_content_min"] = toJson(Vec3(min_x, min_y, 0));
		inner_window.getCameraControlInfo()["orig_content_max"] = toJson(Vec3(max_x, max_y, 0));
	}

	void plot(
		const VecXRef& x, 
		const MatXRef& y,
		const PlotLineOptions& opts = PlotLineOptions())//cmap
	{
		if (x.rows() == 0)
			return;

		if (x.rows() != y.rows())
			throw std::runtime_error("X and Y size must match in plot.");
		
		for (int col = 0; col < y.cols(); col++)
		{
			Vec3 curr_color = opts.color.getRGB();
			if (curr_color[0] < 0)
			{
				curr_color = color_cycle[color_index % color_cycle.size()];
				color_index++;
			}
			
			MatX pts(x.rows(), 3);
			pts.setZero();
			
			pts.col(0) = x;
			pts.col(1) = y.col(col);
			
			if (opts.thickness <= 0)
			{
				auto lines = make_shared<Lines>(pts);
				lines->setColor(curr_color);
				inner_window.add(lines, Vec3(0,0,plot_index + 1));
			}
			else
			{
				auto lines = make_shared<ThickLines>(pts);
				lines->setColor(curr_color);
				lines->setLineWidth(opts.thickness);
				inner_window.add(lines, Vec3(0,0,plot_index + 1));
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
		const PlotLineOptions& opts = PlotLineOptions())
	{
		plot(
			VecXMap(x.data(), x.size()),
			MatXMap(y.data(), y.size(), 1),
			opts
		);
	}
	void plot(
		const MatXRef& y,
		const PlotLineOptions& opts = PlotLineOptions())
	{
		VecX x(y.size());
		for (size_t i = 0; i < y.size(); i++)
		{
			x[i] = i;
		}

		plot(x,y,opts);
	}
	void plot(
		const vector<Vec2>& xy,
		const PlotLineOptions& opts = PlotLineOptions())
	{
		vector<float> x(xy.size());
		vector<float> y(xy.size());

		for (int i = 0; i < xy.size(); i++)
		{
			x[i] = xy[i][0];
			y[i] = xy[i][1];
		}

		plot(
			VecXMap(x.data(), x.size()),
			MatXMap(y.data(), y.size(), 1),
			opts
		);
	}
	void scatter(
		const VecXRef& x, 
		const MatXRef& y, 
		const PlotScatterOptions& opts = PlotScatterOptions())//TODO: color map
	{
		if (x.rows() == 0)
			return;

		if (x.rows() > 1 && y.rows() == 1 && y.cols() == 1)
		{
			v4print "Fill value detected.";
			MatX expanded_y(x.rows(), 1);
			expanded_y.fill(y(0,0));
			scatter(x, expanded_y, opts);
			return;
		}

		if (x.rows() != y.rows())
			throw std::runtime_error("X and Y size must match in scatter.");
		
		MatX pts(x.rows(), 3);
		pts.setZero();
		
		for (int col = 0; col < y.cols(); col++)
		{
			pts.col(0) = x;
			pts.col(1) = y.col(col);

			Vec3 curr_color = opts.color.getRGB();
			if (curr_color[0] < 0)
			{
				curr_color = color_cycle[color_index % color_cycle.size()];
				color_index++;
			}
			
			auto particle = make_shared<Particle>(pts);
			particle->setSize(opts.size);
			particle->setScreenSpaceMode(true);
			particle->setColor(curr_color);
			
			string marker_name = "square";			
			if (opts.marker == PlotScatterMarker::CIRCLE)
			{
				particle->setTexture(base_asset_dir+"/assets/circle.png", false);
				marker_name = "circle";
			}
			else if (opts.marker == PlotScatterMarker::DIAMOND)
			{
				particle->setTexture(base_asset_dir+"/assets/diamond.png", false);
				marker_name = "diamond";
			}
			else if (opts.marker == PlotScatterMarker::PLUS)
			{
				particle->setTexture(base_asset_dir+"/assets/plus.png", false);
				marker_name = "plus";
			}
			inner_window.add(particle, Vec3(0,0,plot_index + 1));
			
			if (!opts.label.empty())
				inner_window.getCameraControlInfo()["labels"].push_back({
					{"text",opts.label},
					{"type",marker_name},
					{"color",toJson(curr_color)}
				});

			plot_index++;
		}
	}
	void scatter(
		const vector<float>& x, 
		const vector<float>& y,
		const PlotScatterOptions& opts = PlotScatterOptions())
	{
		scatter(
			VecXMap(x.data(), x.size()),
			MatXMap(y.data(), y.size(), 1),
			opts
		);
	}
	void scatter(
		const MatXRef& y,
		const PlotScatterOptions& opts = PlotScatterOptions())
	{
		VecX x(y.size());
		for (size_t i = 0; i < y.size(); i++)
		{
			x[i] = i;
		}

		scatter(x,y,opts);
	}
	void scatter(
		const vector<Vec2>& xy,
		const PlotScatterOptions& opts = PlotScatterOptions())
	{
		vector<float> x(xy.size());
		vector<float> y(xy.size());

		for (int i = 0; i < xy.size(); i++)
		{
			x[i] = xy[i][0];
			y[i] = xy[i][1];
		}

		scatter(
			VecXMap(x.data(), x.size()),
			MatXMap(y.data(), y.size(), 1),
			opts
		);
	}
	void text(const string& raw_text, float size, const Vec2& offset, const Color& color)
	{
		auto text = make_shared<Text>(raw_text);
		text->setAnchorCentered();
		text->setColor(color);
		auto node = inner_window.add(text, Vec3(offset[0],offset[1],plot_index + 1));
		node->setScale(size);
		plot_index++;
	}
	void polygon(const vector<Vec2>& verts, const Color& color, float thickness = 0)
	{
		if (verts.empty())
			return;

		// Non-negative border widths indicate border should be drawn
		if (thickness >= 0)
		{
			if (thickness == 0) // Border is a thin line
			{
				MatX pts(verts.size() + 1, 3);

				for (size_t i = 0; i < verts.size(); i++)
				{
					pts.row(i) = Vec3(verts[i][0], verts[i][1], 0);
				}
				pts.row(verts.size()) = Vec3(verts[0][0], verts[0][1], 0);

				auto lines = make_shared<Lines>(pts);
				lines->setColor(color);
				inner_window.add(lines, Vec3(0,0,plot_index + 1));
			}
			else
			{
				auto data = formLineLoop(verts, thickness / 2.0f);
				auto mesh = make_shared<Mesh>(data);
				mesh->setColor(color);
				mesh->setCull(false);
				mesh->setSpecular(0.0f);
				inner_window.add(mesh, Vec3(0,0,plot_index + 1));
			}
		}
		else
		{
			// Negative border width means this is a fill

			// Triangulate an arbitrary polygon
			auto data = formPolygon(verts);
			auto mesh = make_shared<Mesh>(data);
			mesh->setColor(color);
			mesh->setCull(false);
			mesh->setSpecular(0.0f);
			inner_window.add(mesh, Vec3(0,0,plot_index + 1));
		}

		plot_index++;
	}
	void polygon_d(const vector<Vec2d>& verts, const Color& color, float thickness = 0)
	{
		vector<Vec2> verts_f;
		for (const auto& v : verts)
		{
			verts_f.push_back(v.cast<float>());
		}
		polygon(verts_f, color, thickness);
	}
	void circle(const Vec2& center, float rad, const Color& color, float thickness = 0, int slices = 16)
	{
		vector<Vec2> verts;

		for (int slice = 0; slice < slices; slice++)
		{
			float angle = slice / (float)slices * (2 * M_PI);
			verts.push_back(Vec2(cos(angle),sin(angle)) * rad + center);
		}

		polygon(verts, color, thickness);
	}
	void circle_d(const Vec2d& center, float rad, const Color& color, float thickness = 0, int slices = 16)
	{
		circle((Vec2)center.cast<float>(), rad, color, thickness, slices);
	}
	void rect(const Vec2& center, float width, float height, const Color& color, float thickness = 0)
	{
		vector<Vec2> verts;
		verts.push_back(Vec2(-width/2.0f,-height/2.0f) + center);
		verts.push_back(Vec2(width/2.0f,-height/2.0f) + center);
		verts.push_back(Vec2(width/2.0f,height/2.0f) + center);
		verts.push_back(Vec2(-width/2.0f,height/2.0f) + center);

		polygon(verts, color, thickness);
	}
	void line(const vector<Vec2>& verts, const Color& color, float thickness = 0)
	{
		if (verts.empty())
			return;

		if (thickness == 0) // A thin line
		{
			MatX pts(2, 3);

			pts.row(0) = Vec3(verts[0][0], verts[0][1], 0);
			pts.row(1) = Vec3(verts[1][0], verts[1][1], 0);

			auto lines = make_shared<Lines>(pts);
			lines->setColor(color);
			inner_window.add(lines, Vec3(0,0,plot_index + 1));
		}
		else
		{
			auto data = formLine(verts, thickness / 2.0f);
			auto mesh = make_shared<Mesh>(data);
			mesh->setColor(color);
			mesh->setCull(false);
			mesh->setSpecular(0.0f);
			inner_window.add(mesh, Vec3(0,0,plot_index + 1));
		}

		plot_index++;
	}
	void line(const Vec2& start, const Vec2& end, const Color& color, float thickness = 0)
	{
		line({start, end}, color, thickness);
	}
	void line_d(const Vec2d& start, const Vec2d& end, const Color& color, float thickness = 0)
	{
		line({start.cast<float>(), end.cast<float>()}, color, thickness);
	}
    void arrow(const Vec2 &start, const Vec2 &end, const Color &color, const float radius = 1.0F)
    {
        Vec3 start_3d, end_3d;
        start_3d << start, 0;
        end_3d << end, 0;

        auto arrow = std::make_shared<Arrow>(/* start */ start_3d,
                                             /* end */ end_3d,
                                             /* radius */ radius,
                                             /* slices */ 4);
        arrow->setColor(color);
        inner_window.add(/* obj */ arrow, /* parent_from_node_t */ Vec3{0, 0, static_cast<float>(plot_index + 1)});

        plot_index++;
    }
    void arrowhead(const Vec2 &center, const float heading, const Color &color, const float radius = 1.0F)
    {
        const auto offset = radius / 2;
        const Vec2 unit{std::cos(heading), std::sin(heading)};
        arrow(/* start */ center - offset * unit,
              /* end */ center + offset * unit,
              /* color */ color,
              /* radius */ radius);
    }
	void imshow(const Image<uint8_t>& p_image, bool p_filter_nearest = false, const Vec2& offset = Vec2(0,0))
	{
		shared_ptr<Sprite> sprite;
		sprite = make_shared<Sprite>(p_image, p_filter_nearest);
		sprite->setFlip(false, true);
		auto node = inner_window.add(sprite, Vec3(
			p_image.getSize()[0]/2.0f+offset[0],
			p_image.getSize()[1]/2.0f+offset[1],
			0.5), false, 0);
		node->setScale(p_image.getSize()[1]);
		
		inner_window.getCameraControlInfo()["equal"] = true;
		inner_window.getCameraControlInfo()["y_flip"] = true;
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