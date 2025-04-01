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

// header: void handlePlotKeyPress(int key_code, const Vec2& plot_pos)
using PlotKeyActionCallback = std::function<void(int, const Vec2&)>;

// header: void handlePlotMouseClick(bool left, bool down, const Vec2& pos, const Vec2& window_size, const Vec2& plot_pos)
using PlotMouseClickActionCallback = std::function<void(bool, bool, const Vec2&, const Vec2&, const Vec2&)>;

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
	float thickness_in_screen_perc = 0;
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
	float size_in_screen_perc = 1.0;
	PlotScatterMarker marker = PlotScatterMarker::CIRCLE;
};

class Plot
{
public:
    Plot(const string &title = "plot", int width = 800, int height = 800, int x_position = -1, int y_position = -1)
            : inner_window(/* width */ width,
                           /* height */ height,
                           /* x_position */ x_position,
                           /* y_position */ y_position,
                           /* title */ title)
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
	void backColor(const Color& color)
	{
		inner_window.getCameraControlInfo()["back_color"] = toJson(color.getRGB());
	}
	void foreColor(const Color& color)
	{
		inner_window.getCameraControlInfo()["fore_color"] = toJson(color.getRGB());
	}
	void gridColor(const Color& color)
	{
		inner_window.getCameraControlInfo()["grid_color"] = toJson(color.getRGB());
	}
	void darkMode()
	{
		backColor(Vec3(0,0,0));
		foreColor(Vec3(1,1,1));
		gridColor(Vec3(0.75,0.75,0.75));
	}
	Vec3 colorCycle(int index)
	{
		return color_cycle[index % color_cycle.size()];
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
	void cursorCallout(bool enabled)
	{
		inner_window.getCameraControlInfo()["cursor_callout"] = enabled;
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
			
			if (opts.thickness_in_screen_perc <= 0)
			{
				auto lines = make_shared<Lines>(pts);
				lines->setColor(curr_color);
				inner_window.add(lines, Vec3(0,0,plot_index + 1));
			}
			else
			{
				auto lines = make_shared<ThickLines>(pts);
				lines->setColor(curr_color);
				lines->setLineWidth(opts.thickness_in_screen_perc*0.01);
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
	void plot_d(
		const vector<Vec2d>& xy,
		const PlotLineOptions& opts = PlotLineOptions())
	{
		vector<float> x(xy.size());
		vector<float> y(xy.size());

		for (int i = 0; i < xy.size(); i++)
		{
			x[i] = (float)xy[i][0];
			y[i] = (float)xy[i][1];
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
			particle->setSize(opts.size_in_screen_perc*0.01);
			particle->setScreenSpaceMode(true);
			particle->setColor(curr_color);
			
            std::string marker_name{magic_enum::enum_name(opts.marker)};
            marker_name = to_lower(marker_name);
            particle->setTexture(base_asset_dir + "/assets/" + marker_name + ".png", false);
            inner_window.add(particle, Vec3(0, 0, plot_index + 1));
			
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
	void scatter_d(
		const vector<Vec2d>& xy,
		const PlotScatterOptions& opts = PlotScatterOptions())
	{
		vector<float> x(xy.size());
		vector<float> y(xy.size());

		for (int i = 0; i < xy.size(); i++)
		{
			x[i] = (float)xy[i][0];
			y[i] = (float)xy[i][1];
		}

		scatter(
			VecXMap(x.data(), x.size()),
			MatXMap(y.data(), y.size(), 1),
			opts
		);
	}
    void label(const string &text, const Color &color, const PlotScatterMarker marker)
    {
        const std::string marker_name{to_lower(magic_enum::enum_name(marker))};
        inner_window.getCameraControlInfo()["labels"].push_back({
                                                                        {"text",  text},
                                                                        {"type",  marker_name},
                                                                        {"color", toJson(color.getRGB())}
                                                                });
        inner_window.invalidateCameraControlInfo();
    }
    void label(const string &text, const Color &color, const string &marker_name = "circle")
    {
        const auto marker = magic_enum::enum_cast<PlotScatterMarker>(marker_name,
                                                                     magic_enum::case_insensitive);
        if (!marker.has_value())
        {
			throw std::runtime_error("[vephor::Plot::label()] No marker with name " + marker_name);
        }

        label(text, color, marker.value());
    }

    void text(const string &raw_text, float size, const Vec2 &offset, const Color &color)
    {
		auto text = make_shared<Text>(raw_text);
		text->setAnchorCentered();
		text->setColor(color);
		text->setYFlip(inner_window.getCameraControlInfo()["y_flip"]);
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
				mesh->setDiffuseStrength(0.0f);
				mesh->setAmbientStrength(0.0f);
				mesh->setEmissiveStrength(1.0f);
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
			mesh->setDiffuseStrength(0.0f);
			mesh->setAmbientStrength(0.0f);
			mesh->setEmissiveStrength(1.0f);
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
			MatX pts(verts.size(), 3);

			for (int p = 0; p < verts.size(); p++)
			{
				pts.row(p) = Vec3(verts[p][0], verts[p][1], 0);
			}

			auto lines = make_shared<Lines>(pts);
			lines->setLineStrip(true);
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
		Vec2 vec = end - start;

		float vec_mag = vec.norm();

		if (vec_mag < 1e-3)
			return;

		vec /= vec_mag;


		float head_radius = radius * 0.5;
		float shaft_radius = radius * 0.2;


		Vec2 cross(-vec[1], vec[0]);

		vector<Vec2> verts;
		verts.push_back(start - cross * shaft_radius);
		verts.push_back(end - vec * head_radius - cross * shaft_radius);
		verts.push_back(end - vec * head_radius - cross * head_radius);
		verts.push_back(end);
		verts.push_back(end - vec * head_radius + cross * head_radius);
		verts.push_back(end - vec * head_radius + cross * shaft_radius);
		verts.push_back(start + cross * shaft_radius);

		polygon(verts, color, -1);
		

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
	void imshow(const Image<uint8_t>& p_image, bool p_filter_nearest = false, const Vec2& offset = Vec2(0,0), float scale=1.0)
	{
		shared_ptr<Sprite> sprite;
		sprite = make_shared<Sprite>(p_image, p_filter_nearest);
		sprite->setFlip(false, true);
		auto node = inner_window.add(sprite, Vec3(
			p_image.getSize()[0]/2.0f*scale+offset[0],
			p_image.getSize()[1]/2.0f*scale+offset[1],
			0.5), false, 0);
		node->setScale(p_image.getSize()[1]*scale);
		
		inner_window.getCameraControlInfo()["equal"] = true;
		inner_window.getCameraControlInfo()["y_flip"] = true;
	}
	Window& window()
	{
		return inner_window;
	}
	bool show(bool wait_close = true, bool wait_key = false, const std::function<void()>& wait_callback = NULL)
	{
		return inner_window.render(wait_close, wait_key, 0.0, wait_callback);
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
	int takeNextPlotIndex()
	{
		plot_index++;
		return plot_index;
	}

	void setKeyPressCallback(PlotKeyActionCallback p_callback)
    {
		inner_window.setKeyPressWithMessageCallback([p_callback](int key_code, const json& msg){
			p_callback(key_code, readVec2(msg["plot_pos"]));
		});
    }

	void setMouseClickCallback(PlotMouseClickActionCallback p_callback)
    {
		inner_window.setMouseClickWithMessageCallback([p_callback](
			bool left, bool down, const Vec2& pos, const Vec2& window_size, const json& msg){
			p_callback(left, down, pos, window_size, readVec2(msg["plot_pos"]));
		});
    }
private:
	Window inner_window;
	string base_asset_dir;
	vector<Vec3> color_cycle;
	int color_index = 0;
	int plot_index = 0;
};

}