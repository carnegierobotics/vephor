/**
 * Copyright 2023
 * Carnegie Robotics, LLC
 * 4501 Hatfield Street, Pittsburgh, PA 15201
 * https://www.carnegierobotics.com
 *
 * This code is provided under the terms of the Master Services Agreement (the Agreement).
 * This code constitutes CRL Background Intellectual Property, as defined in the Agreement.
**/

#include <gtest/gtest.h>
#include "vephor/base/union_find.h"

using namespace vephor;

TEST(union_find, test)
{
    UnionFind<int> uf;
    uf.addEdge(1,2);
    uf.addEdge(2,3);
    uf.addEdge(3,4);
    uf.addEdge(5,6);

    {
        auto sets = uf.getSets();

        for (const auto& set : sets)
        {
            v4print set.first;
            for (const auto& id : set.second)
            {
                v4print "\t", id;
            }
        }
    }

    uf.addEdge(5,4);

    {
        auto sets = uf.getSets();

        for (const auto& set : sets)
        {
            v4print set.first;
            for (const auto& id : set.second)
            {
                v4print "\t", id;
            }
        }
    }
}
