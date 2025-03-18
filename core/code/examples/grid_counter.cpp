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
#include <chrono>
#include <thread>

using namespace vephor;

std::vector<std::shared_ptr<RenderNode>> off_cube_nodes;
std::vector<std::shared_ptr<RenderNode>> on_cube_nodes;

void apply_number(int num)
{
    for (int i = 0; i < on_cube_nodes.size(); i++)
    {
        if (i == num)
        {
            off_cube_nodes[i]->setShow(false);
            on_cube_nodes[i]->setShow(true);
        }
        else
        {
            on_cube_nodes[i]->setShow(false);
            off_cube_nodes[i]->setShow(true);
        }
    }
}

int main()
{
    Window::setServerModeBYOC();

    Plot plt;
    plt.backColor(Vec3(0,0,0));
    plt.foreColor(Vec3(0.5,0.5,0.5));
    plt.gridColor(Vec3(0,0,0));

    const int N_CELLS = 20;

    auto g = std::make_shared<Grid>(N_CELLS/2);
    g->setColor(Vec3(0.25,0.25,0.25));
    plt.window().add(g);

    

    for (int i = 0; i < N_CELLS; i++)
    for (int j = 0; j < N_CELLS; j++)
    {
        auto cube = std::make_shared<Cube>();
        cube->setColor(Vec3(0.01,0.01,0.01));
        auto cube_node = plt.window().add(cube, Vec3(i-N_CELLS/2+0.5,j-N_CELLS/2+0.5,0));
        cube_node->setScale(0.45);
        off_cube_nodes.push_back(cube_node);
        
        cube = std::make_shared<Cube>();
        cube->setColor(Vec3(0,1,0));
        cube_node = plt.window().add(cube, Vec3(i-N_CELLS/2+0.5,j-N_CELLS/2+0.5,0));
        cube_node->setScale(0.45);
        cube_node->setShow(false);
        on_cube_nodes.push_back(cube_node);
    }

    int num = 0;

    while (true)
    {
        apply_number(num);
        num += 1;
        num %= on_cube_nodes.size();

        plt.show(false, false);
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }

    return 0;
}