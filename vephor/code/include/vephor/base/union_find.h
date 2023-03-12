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