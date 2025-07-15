/**
 * Copyright 2025
 * Carnegie Robotics, LLC
 * 4501 Hatfield Street, Pittsburgh, PA 15201
 * https://www.carnegierobotics.com
 *
 * This source code is licensed under the Apache License, Version 2.0
 * found in the LICENSE file in the root directory of this source tree.
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
