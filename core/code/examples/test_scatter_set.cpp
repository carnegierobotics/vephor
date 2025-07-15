/**
 * Copyright 2025
 * Carnegie Robotics, LLC
 * 4501 Hatfield Street, Pittsburgh, PA 15201
 * https://www.carnegierobotics.com
 *
 * This source code is licensed under the Apache License, Version 2.0
 * found in the LICENSE file in the root directory of this source tree.
**/

#include "vephor_ext.h"

using namespace vephor;

void plot(const VecX& x, const MatX& y, const MatX& c, const MatX& s)
{
    Plot plt;

    plt.equal();

    plt.scatter(
        x,
        y,
        c,
        s,
        PlotScatterOptions{.size_in_screen_perc = 1.0F, 
                            .marker = PlotScatterMarker::CIRCLE});

    plt.show(false);
}

int main()
{
    VecX x(100);
    MatX y(100,3);
    MatX single_s(100, 1);
    MatX multi_s(100, 3);
    MatX single_c(100, 3);
    MatX multi_c(100, 9);

    for (int i = 0; i < 100; i++)
    {
        x[i] = i;

        y(i,0) = i;
        y(i,1) = i/2.0f;
        y(i,2) = i/4.0f;

        float peru = i / 100.0f;

        single_s(i,0) = 0.25f + 0.75f * peru;

        multi_s(i,0) = 0.25f + 0.75f * peru;
        multi_s(i,1) = 0.5f + 1.5f * peru;
        multi_s(i,2) = 1.0f + 2.0f * peru;

        single_c.row(i) = Vec3(peru,0,1-peru).transpose();

        multi_c.row(i).middleCols(0,3) = Vec3(peru,0,1-peru).transpose();
        multi_c.row(i).middleCols(3,3) = Vec3(1-peru,peru,0).transpose();
        multi_c.row(i).middleCols(6,3) = Vec3(0,1-peru,peru).transpose();
    }

    single_s = single_s * 0.01;
    multi_s = multi_s * 0.01;

    plot(x,y,MatX(),single_s);
    plot(x,y,MatX(),multi_s);
    plot(x,y,single_c,MatX());
    plot(x,y,multi_c,MatX());

    plot(x,y,single_c,single_s);
    plot(x,y,single_c,multi_s);
    plot(x,y,multi_c,single_s);
    plot(x,y,multi_c,multi_s);

    Window::renderInWaiting();
}
