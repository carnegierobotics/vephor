/**
 * Copyright 2023 - 2025
 * Carnegie Robotics, LLC
 * 4501 Hatfield Street, Pittsburgh, PA 15201
 * https://www.carnegierobotics.com
 *
 * This code is provided under the terms of the Master Services Agreement (the Agreement).
 * This code constitutes CRL Background Intellectual Property, as defined in the Agreement.
 **/

#include "vephor_ext.h"

using namespace vephor;


vector<float> linspace(const float start, const float end, const int num)
{
    vector<float> result;

    if (num <= 0)
    {
        return result;
    }

    if (num == 1)
    {
        result.push_back(start);
        return result;
    }

    const auto delta = (end - start) / (num - 1);
    for (int i = 0; i < num; ++i)
    {
        result.push_back(start + i * delta);
    }

    return result;
}


int main()
{
    Plot plt;

    plt.title("Squared");
    plt.xlabel("X");
    plt.ylabel("Y");

    //
    // Data
    //

    const auto xs = linspace(-2.0, 2.0, 101);

    vector<float> ys_linear;
    vector<float> ys_sqrt;
    vector<float> ys_square;
    vector<float> ys_cube;
    vector<float> ys_sin;
    for (const auto x : xs)
    {
        ys_linear.emplace_back(x);
        ys_sqrt.emplace_back(std::sqrt(x));
        ys_square.emplace_back(x * x);
        ys_cube.emplace_back(x * x * x);
        ys_sin.emplace_back(std::sin(x));
    }

    //
    // Line plots
    //

    plt.plot(xs, ys_linear, {.label = "x thick line", .thickness_in_screen_perc = 1.0});
    plt.plot(xs, ys_linear, {.label = "x thin line"});

    //
    // Scatter plots
    //

    plt.scatter(xs, ys_cube, {.label = "x^3 scatter", .color = {0.0F, 0.0F, 1.0F}, .marker = PlotScatterMarker::PLUS});

    {
        vector<float> xs_half;
        vector<float> ys_half;
        for (int i = 0; i < xs.size(); ++i)
        {
            if (i % 2 == 0)
            {
                xs_half.emplace_back(xs[i]);
                ys_half.emplace_back(ys_cube[i]);
            }
        }

        plt.scatter(xs_half,
                    ys_half,
                    {.label = "x^3 half-step scatter", .color = {0.0F, 1.0F, 0.0F}, .marker = PlotScatterMarker::STAR});
    }

    //
    // Colored scatter plots
    //

    {
        //
        // Plot a square root function with random colors assigned to each point
        //

        std::random_device rd;
        std::mt19937 rng(rd());
        std::uniform_real_distribution<float> c_dist(0, 1);

        vector<Color> c;
        for (int i = 0; i < xs.size(); ++i)
        {
            c.emplace_back(c_dist(rng), c_dist(rng), c_dist(rng));
        }

        plt.scatter(xs, ys_sqrt, c, {.label = "sqrt(x) with random colors", .marker = PlotScatterMarker::CIRCLE});
    }

    {
        //
        // Plot a square function with colors assigned via a y-value color map
        //

        plt.scatter_colormap(xs,
                             ys_square,
                             PlotScatterOptions{.label = "x^2 colored by y-value",
                                                .marker = PlotScatterMarker::DIAMOND,
                                                .colormap_style = ColorMap::Style::VIRIDIS});
    }

    {
        //
        // Plot a sine function with colors assigned via an x-value color map
        //

        plt.scatter_colormap(xs,
                             ys_sin,
                             xs,
                             PlotScatterOptions{.label = "sin(x) colored by x-value",
                                                .marker = PlotScatterMarker::STAR,
                                                .colormap_style = ColorMap::Style::JET});
    }

    {
        plt.label("Custom Label", {1.0F, 0.0F, 0.0F}, "square");
    }

    plt.show();

    return 0;
}
