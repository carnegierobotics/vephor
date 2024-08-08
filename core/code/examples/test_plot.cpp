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

    plt.title("Squared");

    plt.xlabel("X Axis");
    plt.ylabel("Y Axis");

    {
        vector<float> x;
        vector<float> y;
        for (int i = 0; i < 100; i++) {
            x.push_back(i);
            y.push_back(i * i);
        }
        PlotLineOptions opts;
        opts.label = "Thick Line";
        opts.thickness = 0.025;
        plt.plot(x, y, opts);
    }

    {
        vector<float> x;
        vector<float> y;
        for (int i = 0; i < 100; i++) {
            x.push_back(i);
            y.push_back(i * i);
        }
        PlotLineOptions opts;
        opts.label = "Less Thick Line";
        opts.thickness = 0.01;
        plt.plot(x, y, opts);
    }

    {
        vector<float> x;
        vector<float> y;
        for (int i = 0; i < 100; i++) {
            x.push_back(i);
            y.push_back(i * i);
        }
        PlotLineOptions opts;
        opts.label = "Thin Line";
        plt.plot(x, y, opts);
    }

    {
        vector<float> x;
        vector<float> y;
        for (int i = 0; i < 100; i++) {
            x.push_back(static_cast<float>(i));
            y.push_back(static_cast<float>(i * i));
        }

        plt.scatter(x, y, {.label = "Scatter", .color = {0.0F, 0.0F, 1.0F}, .marker = PlotScatterMarker::PLUS});
    }

    {
        vector<float> x;
        vector<float> y;
        for (int i = 0; i < 100; i += 2) {
            x.push_back(static_cast<float>(i));
            y.push_back(static_cast<float>(i * i));
        }

        plt.scatter(x, y,
                    {.label = "Double-Step Scatter", .color = {0.0F, 1.0F, 0.0F}, .marker = PlotScatterMarker::STAR});
    }

    {
        plt.label("Custom Label", {1.0F, 0.0F, 0.0F}, "square");
    }

    plt.show();

    return 0;
}