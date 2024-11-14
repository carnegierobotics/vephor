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

#include "common.h"

namespace vephor
{
	
using TransformCallback = std::function<void(const TransformSim3&)>;

class TransformNode
{
public:
	TransformNode(const TransformSim3& p_parent_from_node, TransformNode* p_parent = NULL)
	: parent_from_node(p_parent_from_node), parent(p_parent)
	{
        std::cout << "[" << __FILE__ << "][" << __LINE__ << "][TransformNode::" << __func__ << "]\n";
		if (parent)
        {
            std::cout << "[" << __FILE__ << "][" << __LINE__ << "][TransformNode::" << __func__ << "]\n";
            world_from_parent = parent->getWorldTransform();
        }
        std::cout << "[" << __FILE__ << "][" << __LINE__ << "][TransformNode::" << __func__ << "]\n";
    }
	Vec3 getPos() const
	{
        std::cout << "[" << __FILE__ << "][" << __LINE__ << "][TransformNode::" << __func__ << "]\n";
		return parent_from_node.translation();
	}
	void setPos(const Vec3& pos)
	{
        std::cout << "[" << __FILE__ << "][" << __LINE__ << "][TransformNode::" << __func__ << "]\n";
		parent_from_node.setTranslation(pos);
        std::cout << "[" << __FILE__ << "][" << __LINE__ << "][TransformNode::" << __func__ << "]\n";
		updateTree();
        std::cout << "[" << __FILE__ << "][" << __LINE__ << "][TransformNode::" << __func__ << "]\n";
	}
	Orient3 getOrient() const {return parent_from_node.quat();}
    void setOrient(Orient3 orient)
	{
        std::cout << "[" << __FILE__ << "][" << __LINE__ << "][TransformNode::" << __func__ << "]\n";
		orient.normalize(); 
        std::cout << "[" << __FILE__ << "][" << __LINE__ << "][TransformNode::" << __func__ << "]\n";
		parent_from_node.setRotation(orient);
        std::cout << "[" << __FILE__ << "][" << __LINE__ << "][TransformNode::" << __func__ << "]\n";
		updateTree();
        std::cout << "[" << __FILE__ << "][" << __LINE__ << "][TransformNode::" << __func__ << "]\n";
	}
	TransformSim3 getTransform() const
	{
        std::cout << "[" << __FILE__ << "][" << __LINE__ << "][TransformNode::" << __func__ << "]\n";
		return parent_from_node;
	}
	void setTransform(const TransformSim3& t)
	{
        std::cout << "[" << __FILE__ << "][" << __LINE__ << "][TransformNode::" << __func__ << "]\n";
		parent_from_node = t;
        std::cout << "[" << __FILE__ << "][" << __LINE__ << "][TransformNode::" << __func__ << "]\n";
		updateTree();
	}
	float getScale() const
	{
        std::cout << "[" << __FILE__ << "][" << __LINE__ << "][TransformNode::" << __func__ << "]\n";
		return parent_from_node.scale;
	}
	void setScale(float p_scale)
	{
        std::cout << "[" << __FILE__ << "][" << __LINE__ << "][TransformNode::" << __func__ << "]\n";
		parent_from_node.scale = p_scale;
        std::cout << "[" << __FILE__ << "][" << __LINE__ << "][TransformNode::" << __func__ << "]\n";
		updateTree();
	}
	void updateTree()
	{
        std::cout << "[" << __FILE__ << "][" << __LINE__ << "][TransformNode::" << __func__ << "]\n";
		TransformSim3 world_from_node = getWorldTransform();
        std::cout << "[" << __FILE__ << "][" << __LINE__ << "][TransformNode::" << __func__ << "]\n";
		for (const auto& callback : callbacks)
        {
            std::cout << "[" << __FILE__ << "][" << __LINE__ << "][TransformNode::" << __func__ << "]\n";
            callback(world_from_node);
        }
        std::cout << "[" << __FILE__ << "][" << __LINE__ << "][TransformNode::" << __func__ << "]\n";
		for (auto& child : children)
		{
            std::cout << "[" << __FILE__ << "][" << __LINE__ << "][TransformNode::" << __func__ << "]\n";
			child->updateParentTransform(world_from_node);
		}
        std::cout << "[" << __FILE__ << "][" << __LINE__ << "][TransformNode::" << __func__ << "]\n";
	}
	void updateParentTransform(const TransformSim3& p_world_from_parent)
	{
        std::cout << "[" << __FILE__ << "][" << __LINE__ << "][TransformNode::" << __func__ << "]\n";
		world_from_parent = p_world_from_parent;
        std::cout << "[" << __FILE__ << "][" << __LINE__ << "][TransformNode::" << __func__ << "]\n";
		updateTree();
        std::cout << "[" << __FILE__ << "][" << __LINE__ << "][TransformNode::" << __func__ << "]\n";
	}
	TransformSim3 getWorldTransform() const
	{
        std::cout << "[" << __FILE__ << "][" << __LINE__ << "][TransformNode::" << __func__ << "]\n";
		return world_from_parent * parent_from_node;
	}
	shared_ptr<TransformNode> addChild(const TransformSim3& parent_from_child)
	{
        std::cout << "[" << __FILE__ << "][" << __LINE__ << "][TransformNode::" << __func__ << "]\n";
		auto child = make_shared<TransformNode>(parent_from_child, this);
        std::cout << "[" << __FILE__ << "][" << __LINE__ << "][TransformNode::" << __func__ << "]\n";
		children.push_back(child);
        std::cout << "[" << __FILE__ << "][" << __LINE__ << "][TransformNode::" << __func__ << "]\n";
		return child;
	}
	void addChild(const shared_ptr<TransformNode>& node)
	{
        std::cout << "[" << __FILE__ << "][" << __LINE__ << "][TransformNode::" << __func__ << "]\n";
		node->parent = this;
        std::cout << "[" << __FILE__ << "][" << __LINE__ << "][TransformNode::" << __func__ << "]\n";
		children.push_back(node);
        std::cout << "[" << __FILE__ << "][" << __LINE__ << "][TransformNode::" << __func__ << "]\n";
		node->updateParentTransform(getWorldTransform());
        std::cout << "[" << __FILE__ << "][" << __LINE__ << "][TransformNode::" << __func__ << "]\n";
	}
	TransformNode* getParent() const
    {
        std::cout << "[" << __FILE__ << "][" << __LINE__ << "][TransformNode::" << __func__ << "]\n";
        return parent;
    }
	void setName(const string& p_name)
    {
        std::cout << "[" << __FILE__ << "][" << __LINE__ << "][TransformNode::" << __func__ << "]\n";
        name = p_name;
    }
	string getName() const
    {
        std::cout << "[" << __FILE__ << "][" << __LINE__ << "][TransformNode::" << __func__ << "]\n";
        return name;
    }
	void addTransformCallback(TransformCallback p_callback)
	{
        std::cout << "[" << __FILE__ << "][" << __LINE__ << "][TransformNode::" << __func__ << "]\n";
		callbacks.push_back(p_callback);
        std::cout << "[" << __FILE__ << "][" << __LINE__ << "][TransformNode::" << __func__ << "]\n";
	}
private:
	string name;
	TransformSim3 parent_from_node;
	TransformNode* parent = NULL;
	vector<shared_ptr<TransformNode>> children;
	vector<TransformCallback> callbacks;
	TransformSim3 world_from_parent;
};
	
}