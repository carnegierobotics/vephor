/**
 * Copyright 2023
 * Carnegie Robotics, LLC
 * 4501 Hatfield Street, Pittsburgh, PA 15201
 * https://www.carnegierobotics.com
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the Carnegie Robotics, LLC nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL CARNEGIE ROBOTICS, LLC BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
**/

#pragma once

#include "common.h"

namespace vephor
{

template <class T>
class UnionFind
{
public:
	void addEdge(T id1, T id2)
	{
		id1 = find(id1);
		id2 = find(id2);

		if (id1 == id2)
			return;

		auto& node1 = nodes[id1];
		auto& node2 = nodes[id2];

		Node* first_node;
		Node* second_node;

		if ((node1.rank) < (node2.rank))
		{
			second_node = &node1;
			first_node = &node2;
		}
		else
		{
			second_node = &node2;
			first_node = &node1;
		}

		second_node->parent = first_node->parent;
		if (first_node->rank == second_node->rank)
			first_node->rank++;
	}
	unordered_map<T, unordered_set<T>> getSets()
	{
		unordered_map<T, unordered_set<T>> sets;

		for (const auto& node : nodes)
		{
			T root = find(node.first);
			sets[root].insert(node.first);
		}

		return sets;
	}
private:
	struct Node
	{
		T parent;
		int rank = 0;
	};

	T find(T id)
	{
		if (!vephor::find(nodes, id))
		{
			nodes[id].parent = id;
			return id;
		}

		while (true)
		{
			int next_id = nodes[id].parent;

			if (next_id == id)
				break;

			int next_parent = nodes[next_id].parent;
			id = next_id;
			nodes[id].parent = next_parent;
		}

		return id;
	}
	unordered_map<T,Node> nodes;

};
	
}