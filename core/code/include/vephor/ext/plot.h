/**
 * Copyright 2025
 * Carnegie Robotics, LLC
 * 4501 Hatfield Street, Pittsburgh, PA 15201
 * https://www.carnegierobotics.com
 *
 * This source code is licensed under the Apache License, Version 2.0
 * found in the LICENSE file in the root directory of this source tree.
**/

#pragma once

#include "window.h"

#include "vephor/base/string.h"
#include "vephor/base/thirdparty/Neargye/magic_enum/magic_enum.hpp"

#include <cmath>
#include <limits>
#include <stdexcept>
#include <string>

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

enum class PlotScatterMarker
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
    Color color = Color(Vec3(-1, -1, -1));
    float size_in_screen_perc = 1.0;
    PlotScatterMarker marker = PlotScatterMarker::CIRCLE;
    Color icon_color = Color(Vec3(-1, -1, -1));

    ColorMap::Style colormap_style = ColorMap::Style::VIRIDIS;
    Vec2 color_axis_limits{std::numeric_limits<float>::quiet_NaN(), std::numeric_limits<float>::quiet_NaN()};

    Vec2 size_axis_limits{std::numeric_limits<float>::quiet_NaN(), std::numeric_limits<float>::quiet_NaN()};
    Vec2 point_size_limits{std::numeric_limits<float>::quiet_NaN(), std::numeric_limits<float>::quiet_NaN()};
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
    void xyswap(bool swap = true)
	{
		inner_window.getCameraControlInfo()["xy_swap"] = swap;
	}
	void equal(bool is_equal = true)
	{
		inner_window.getCameraControlInfo()["equal"] = is_equal;
	}
	void textScale(float text_scale)
	{
		inner_window.getCameraControlInfo()["text_scale"] = text_scale;
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
	void setLegendTopRight()
	{
		inner_window.getCameraControlInfo()["legend_right"] = true;
		inner_window.getCameraControlInfo()["legend_top"] = true;
	}
	void setLegendTopLeft()
	{
		inner_window.getCameraControlInfo()["legend_right"] = false;
		inner_window.getCameraControlInfo()["legend_top"] = true;
	}
	void setLegendBottomRight()
	{
		inner_window.getCameraControlInfo()["legend_right"] = true;
		inner_window.getCameraControlInfo()["legend_top"] = false;
	}
	void setLegendBottomLeft()
	{
		inner_window.getCameraControlInfo()["legend_right"] = false;
		inner_window.getCameraControlInfo()["legend_top"] = false;
	}

    //
    // Plot
    //

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

    //
    // Scatter
    //

    ///
    /// Create a scatter plot with the specified point coordinates, colors, and sizes.
    ///
    /// @todo Add opacity/alpha specification.
    /// @todo Smarter legend icons for cases where color is not uniform.
    /// @todo Add back in c/s matrix support for sets if desired
    ///
    /// @param[in] x An (N, 1) vector of x-values.
    /// @param[in] y Either a (1, 1) singular y-value or an (N, M) matrix of y-values organized by set.
    /// @param[in] c Either a (0, 0) matrix indicating that color should be assigned automatically or based on opts, 
    //             an (N, 3) matrix specifying RGB colors for each point index, or an (N, 3 * M) matrix specifying RGB 
    ///            colors for each individual point in each indivual set.
    /// @param[in] s Either a (0, 0) matrix indicating that point size should be assigned based on @c opts, 
    //             an (N, 1) matrix of sizes for each individual point, or an
    ///            (N, M) matrix specifying sizes for each individual point in each individual set.
    /// @param[in] opts Plot configuration options.
    ///
    void scatter(const VecXRef& x,
                 const MatXRef& y,
                 const MatXRef& c,
                 const MatXRef& s,
                 const PlotScatterOptions& opts = PlotScatterOptions())
    {
        const auto n_points = x.rows();

        // Empty plots can shortcut the downstream logic
        if (n_points == 0)
        {
            return;
        }

        // Fill Y values if only one is provided
        {
            if (n_points > 1 && y.rows() == 1 && y.cols() == 1)
            {
                MatX expanded_y(n_points, 1);
                expanded_y.fill(y(0, 0));

                scatter(x, expanded_y, c, s, opts);

                return;
            }

            // All fill-in possibilities exhausted
            if (y.rows() != n_points)
            {
                throw std::runtime_error("Invalid Y specified in scatter.");
            }
        }

        // Check validity of C values
        {
            if (c.cols() % 3 != 0)
            {
                throw std::runtime_error("Invalid C specified in scatter.");
            }

            // All fill-in possibilities exhausted
            if (c.rows() > 0 && (c.rows() != n_points || (c.cols() != 3 * y.cols() && c.cols() != 3)))
            {
                throw std::runtime_error("Invalid C specified in scatter.");
            }
        }

        // Check validity of S values
        {
            if (s.rows() > 0 && (s.rows() != n_points || (s.cols() != y.cols() && s.cols() != 1)))
            {
                throw std::runtime_error("Invalid S specified in scatter.");
            }
        }

        // At this point we have x ∈ R(N, 1), y ∈ R(N, M), 
        //  and
        // c ∈ R(N, 3 * M) or c ∈ R(0, 3 * ?)
        //  and
        // s ∈ R(N, M) or s ∈ R(0, ?)
        MatX points = MatX::Zero(x.rows(), 3);
        points.col(0) = x;
        for (int set = 0; set < y.cols(); set++)
        {
            const VecX& set_y = y.col(set);
            points.col(1) = set_y;

            shared_ptr<Particle> particle;

            Vec3 icon_color;

            if (c.rows() > 0)
            {
                MatX set_colors;

                if (c.cols() == 3)
                {
                    set_colors = c;
                }
                else
                {
                    set_colors = c.middleCols(3 * set, 3);
                }
                
                icon_color = set_colors.row(0).transpose();

                particle = make_shared<Particle>(points, set_colors);
                if (s.rows() > 0)
                {
                    if (s.cols() == 1)
                    {
                        particle->setSizes(s);
                    }
                    else
                    {
                        const VecX& set_sizes = s.col(set);
                        particle->setSizes(set_sizes);
                    }
                }
                else
                {
                    particle->setSize(opts.size_in_screen_perc * 0.01);
                }
            }
            else
            {
                Vec3 curr_color = opts.color.getRGB();
                if (curr_color[0] < 0)
                {
                    curr_color = color_cycle[color_index % color_cycle.size()];
                    color_index++;
                }

                icon_color = curr_color;

                particle = make_shared<Particle>(points);
                particle->setColor(curr_color);
                if (s.rows() > 0)
                {
                    if (s.cols() == 1)
                    {
                        particle->setSizes(s);
                    }
                    else
                    {
                        const VecX& set_sizes = s.col(set);
                        particle->setSizes(set_sizes);
                    }
                }
                else
                {
                    particle->setSize(opts.size_in_screen_perc * 0.01);
                }
            }


            particle->setScreenSpaceMode(true);

            std::string marker_name{magic_enum::enum_name(opts.marker)};
            marker_name = to_lower(marker_name);
            particle->setTexture(base_asset_dir + "/assets/" + marker_name + ".png", false);
            inner_window.add(particle, Vec3(0, 0, plot_index + 1));

            if (!opts.label.empty())
            {
                // TODO: Add the ability to specify a color gradient icon or a discrete color set icon.
                inner_window.getCameraControlInfo()["labels"].push_back(
                    {{"text", opts.label}, {"type", marker_name}, {"color", toJson(icon_color)}});
            }

            plot_index++;
        }
    }

    ///
    /// @overload
    ///
    void scatter(const VecXRef& x,
                 const MatXRef& y,
                 const MatXRef& c,
                 const PlotScatterOptions& opts = PlotScatterOptions())
    {
        scatter(x, y, c, MatX{}, opts);
    }

    ///
    /// @overload
    ///
    void scatter(const VecXRef& x, const MatXRef& y, const PlotScatterOptions& opts = PlotScatterOptions())
    {
        scatter(x, y, MatX{}, MatX{}, opts);
    }

    ///
    /// @overload
    ///
    void scatter(const MatXRef& y, const PlotScatterOptions& opts = PlotScatterOptions())
    {
        VecX x(y.size());
        for (size_t i = 0; i < y.size(); i++)
        {
            x[i] = i;
        }

        scatter(x, y, MatX{}, MatX{}, opts);
    }

    ///
    /// @overload
    ///
    void scatter(const vector<float>& x,
                 const vector<float>& y,
                 const vector<Vec3>& c,
                 const vector<float>& s,
                 const PlotScatterOptions& opts = PlotScatterOptions())
    {
        MatX c_mat = MatX::Zero(c.size(), 3);
        for (size_t i = 0; i < c.size(); i++)
        {
            c_mat.row(i) = c[i].transpose();
        }

        scatter(
            VecXMap(x.data(), x.size()), MatXMap(y.data(), y.size(), 1), c_mat, MatXMap(s.data(), s.size(), 1), opts);
    }

    ///
    /// @overload
    ///
    void scatter(const vector<float>& x,
                 const vector<float>& y,
                 const vector<Color>& c,
                 const vector<float>& s,
                 const PlotScatterOptions& opts = PlotScatterOptions())
    {
        MatX c_mat = MatX::Zero(c.size(), 3);
        for (size_t i = 0; i < c.size(); i++)
        {
            c_mat.row(i) = c[i].getRGB().transpose();
        }

        scatter(
            VecXMap(x.data(), x.size()), MatXMap(y.data(), y.size(), 1), c_mat, MatXMap(s.data(), s.size(), 1), opts);
    }

    ///
    /// @overload
    ///
    void scatter(const vector<float>& x,
                 const vector<float>& y,
                 const vector<Vec3>& c,
                 const PlotScatterOptions& opts = PlotScatterOptions())
    {
        scatter(x, y, c, std::vector<float>{}, opts);
    }

    ///
    /// @overload
    ///
    void scatter(const vector<float>& x,
                 const vector<float>& y,
                 const vector<Color>& c,
                 const PlotScatterOptions& opts = PlotScatterOptions())
    {
        scatter(x, y, c, std::vector<float>{}, opts);
    }

    ///
    /// @overload
    ///
    void scatter(const vector<float>& x, const vector<float>& y, const PlotScatterOptions& opts = PlotScatterOptions())
    {
        scatter(x, y, std::vector<Vec3>{}, std::vector<float>{}, opts);
    }

    ///
    /// @overload
    ///
    void scatter(const vector<Vec2>& xy,
                 const vector<Vec3>& c,
                 const vector<float>& s,
                 const PlotScatterOptions& opts = PlotScatterOptions())
    {
        vector<float> x(xy.size());
        vector<float> y(xy.size());

        for (int i = 0; i < xy.size(); i++)
        {
            x[i] = xy[i][0];
            y[i] = xy[i][1];
        }

        scatter(x, y, c, s, opts);
    }

    ///
    /// @overload
    ///
    void scatter(const vector<Vec2>& xy,
                 const vector<Color>& c,
                 const vector<float>& s,
                 const PlotScatterOptions& opts = PlotScatterOptions())
    {
        vector<float> x(xy.size());
        vector<float> y(xy.size());

        for (int i = 0; i < xy.size(); i++)
        {
            x[i] = xy[i][0];
            y[i] = xy[i][1];
        }

        scatter(x, y, c, s, opts);
    }

    ///
    /// @overload
    ///
    void scatter(const vector<Vec2>& xy, const vector<Vec3>& c, const PlotScatterOptions& opts = PlotScatterOptions())
    {
        scatter(xy, c, vector<float>(), opts);
    }

    ///
    /// @overload
    ///
    void scatter(const vector<Vec2>& xy, const vector<Color>& c, const PlotScatterOptions& opts = PlotScatterOptions())
    {
        scatter(xy, c, vector<float>(), opts);
    }

    ///
    /// @overload
    ///
    void scatter(const vector<Vec2>& xy, const PlotScatterOptions& opts = PlotScatterOptions())
    {
        scatter(xy, vector<Vec3>(), vector<float>(), opts);
    }

    ///
    /// @overload
    ///
    void scatter_d(const vector<Vec2d>& xy, const PlotScatterOptions& opts = PlotScatterOptions())
    {
        vector<float> x(xy.size());
        vector<float> y(xy.size());

        for (int i = 0; i < xy.size(); i++)
        {
            x[i] = static_cast<float>(xy[i][0]);
            y[i] = static_cast<float>(xy[i][1]);
        }

        scatter(VecXMap(x.data(), x.size()), MatXMap(y.data(), y.size(), 1), opts);
    }

    //
    // Scatter with color and size
    //

    ///
    /// Create a scatter plot with point colors assigned from a color map and point sizes assigned from a size map.
    ///
    /// @todo Add support for an alpha axis.
    /// @todo Add support for plotting multiple y sets as in scatter().
    ///
    /// @param[in] x (N, 1) vector of x-values.
    /// @param[in] y (N, 1) matrix of y-values corresponding to @c x.
    /// @param[in] c (N, 1) matrix specifying value to use for prompting color map.  Note that limits can be optionally
    ///            set in @c opts.
    /// @param[in] s (N, 1) matrix specifying value to use for prompting size map.  Note that limits can be optionally
    ///            set in @c opts.
    /// @param[in] opts Scatter plot options.
    ///
    void scatter_cs(const VecXRef& x,
                    const VecXRef& y,
                    const VecXRef& c,
                    const VecXRef& s,
                    PlotScatterOptions opts = PlotScatterOptions())
    {
        const auto n_points = x.size();

        //
        // Color map
        //

        MatX colors;
        if (n_points > 0 && c.size() > 0)
        {
            if (std::isnan(opts.color_axis_limits(0)))
            {
                opts.color_axis_limits(0) = c.minCoeff();
            }
            if (std::isnan(opts.color_axis_limits(1)))
            {
                opts.color_axis_limits(1) = c.maxCoeff();
            }

            const ColorMap colormap{opts.colormap_style};

            colors.conservativeResize(n_points, 3);
            for (auto i = 0; i < n_points; ++i)
            {
                const float c_value = rescale(c(i), opts.color_axis_limits(0), opts.color_axis_limits(1), 0.0F, 1.0F);
                colors.row(i) = colormap(c_value).getRGB();
            }
        }

        //
        // Size map
        //

        VecX sizes;
        if (n_points > 0 && s.size() > 0)
        {
            if (std::isnan(opts.size_axis_limits(0)))
            {
                opts.size_axis_limits(0) = s.minCoeff();
            }
            if (std::isnan(opts.size_axis_limits(1)))
            {
                opts.size_axis_limits(1) = s.maxCoeff();
            }

            if (std::isnan(opts.point_size_limits(0)))
            {
                opts.point_size_limits(0) = std::max(0.0F, opts.size_in_screen_perc - 0.5F);
            }
            if (std::isnan(opts.point_size_limits(1)))
            {
                opts.point_size_limits(1) = opts.size_in_screen_perc + 0.5F;
            }

            sizes.conservativeResize(n_points);
            for (auto i = 0; i < n_points; ++i)
            {
                sizes(i) = rescale(s(i),
                                   opts.size_axis_limits(0),
                                   opts.size_axis_limits(1),
                                   0.01F * opts.point_size_limits(0),
                                   0.01F * opts.point_size_limits(1));
            }
        }

        //
        // Plot scatter
        //

        scatter(x, y, colors, sizes, opts);
    }

    ///
    /// @overload
    ///
    void scatter_cs(const vector<float>& x,
                    const vector<float>& y,
                    const vector<float>& c,
                    const vector<float>& s,
                    const PlotScatterOptions& opts = PlotScatterOptions())
    {
        scatter_cs(VecXMap(x.data(), x.size()),
                   MatXMap(y.data(), y.size(), 1),
                   VecXMap(c.data(), c.size()),
                   VecXMap(s.data(), s.size()),
                   opts);
    }

    ///
    /// Create a scatter plot with point colors assigned from a color map with scaling designated by values in @c c.
    ///
    /// @param[in] x (N, 1) vector of x-values.
    /// @param[in] y (N, 1) matrix of y-values corresponding to @c x.
    /// @param[in] c (N, 1) matrix specifying value to use for prompting color map.  Note that limits can be optionally
    ///            set in @c opts.
    /// @param[in] opts Scatter plot options.
    ///
    void scatter_colormap(const VecXRef& x,
                          const VecXRef& y,
                          const VecXRef& c,
                          PlotScatterOptions opts = PlotScatterOptions())
    {
        scatter_cs(x, y, c, VecX{}, opts);
    }

    ///
    /// @overload
    ///
    void scatter_colormap(const vector<float>& x,
                          const vector<float>& y,
                          const vector<float>& c,
                          const PlotScatterOptions& opts = PlotScatterOptions())
    {
        scatter_colormap(
            VecXMap(x.data(), x.size()), MatXMap(y.data(), y.size(), 1), VecXMap(c.data(), c.size()), opts);
    }

    ///
    /// @overload
    ///
    void scatter_colormap(const vector<Vec2>& xy,
                          const vector<float>& c,
                          const PlotScatterOptions& opts = PlotScatterOptions())
    {
        vector<float> x(xy.size());
        vector<float> y(xy.size());

        for (int i = 0; i < xy.size(); i++)
        {
            x[i] = xy[i][0];
            y[i] = xy[i][1];
        }

        scatter_colormap(
            VecXMap(x.data(), x.size()), MatXMap(y.data(), y.size(), 1), VecXMap(c.data(), c.size()), opts);
    }

    ///
    /// Create a scatter plot with point colors assigned from a color map with scaling designated by y-value magnitudes.
    ///
    /// @param x (N, 1) vector of x-values.
    /// @param y (N, 1) matrix of y-values corresponding to @c x.
    /// @param opts Scatter plot options.
    ///
    void scatter_colormap(const VecXRef& x, const VecXRef& y, const PlotScatterOptions& opts = PlotScatterOptions())
    {
        auto c_opts = opts;
        if (std::isnan(c_opts.color_axis_limits(0)))
        {
            c_opts.color_axis_limits(0) = y.minCoeff();
        }
        if (std::isnan(c_opts.color_axis_limits(1)))
        {
            c_opts.color_axis_limits(1) = y.maxCoeff();
        }

        const ColorMap colormap{opts.colormap_style};

        MatX colors(y.rows(), 3);
        for (auto i = 0; i < y.rows(); ++i)
        {
            const float c_value = rescale(y(i), c_opts.color_axis_limits(0), c_opts.color_axis_limits(1), 0.0f, 1.0f);
            colors.row(i) = colormap(c_value).getRGB();
        }

        scatter(x, y, colors, c_opts);
    }

    ///
    /// @overload
    ///
    void scatter_colormap(const vector<float>& x,
                          const vector<float>& y,
                          const PlotScatterOptions& opts = PlotScatterOptions())
    {
        scatter_colormap(VecXMap(x.data(), x.size()), MatXMap(y.data(), y.size(), 1), opts);
    }

    ///
    /// @overload
    ///
    void scatter_colormap(const vector<Vec2>& xy, const PlotScatterOptions& opts = PlotScatterOptions())
    {
        vector<float> x(xy.size());
        vector<float> y(xy.size());

        for (int i = 0; i < xy.size(); i++)
        {
            x[i] = xy[i][0];
            y[i] = xy[i][1];
        }

        scatter_colormap(VecXMap(x.data(), x.size()), MatXMap(y.data(), y.size(), 1), opts);
    }

    //
    // Assorted
    //

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
    void rectMinMax(const Vec2& min, const Vec2& max, const Color& color, float thickness = 0)
	{
		vector<Vec2> verts;
		verts.push_back(Vec2(min[0],min[1]));
		verts.push_back(Vec2(max[0],min[1]));
		verts.push_back(Vec2(max[0],max[1]));
		verts.push_back(Vec2(min[0],max[1]));

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