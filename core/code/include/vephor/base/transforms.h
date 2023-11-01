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
	
using TransformCallback = std::function<void(const TransformSim3&)>;

class TransformNode
{
public:
	TransformNode(const TransformSim3& p_parent_from_node, TransformNode* p_parent = NULL)
	: parent_from_node(p_parent_from_node), parent(p_parent)
	{
		if (parent)
			world_from_parent = parent->getWorldTransform();
	}
	Vec3 getPos() const
	{
		return parent_from_node.translation();
	}
	void setPos(const Vec3& pos)
	{
		parent_from_node.setTranslation(pos);
		updateTree();
	}
	Orient3 getOrient() const {return parent_from_node.quat();}
    void setOrient(Orient3 orient)
	{
		orient.normalize(); 
		parent_from_node.setRotation(orient);
		updateTree();
	}
	TransformSim3 getTransform() const
	{
		return parent_from_node;
	}
	void setTransform(const TransformSim3& t)
	{
		parent_from_node = t;
		updateTree();
	}
	float getScale() const
	{
		return parent_from_node.scale;
	}
	void setScale(float p_scale)
	{
		parent_from_node.scale = p_scale;
		updateTree();
	}
	void updateTree()
	{
		TransformSim3 world_from_node = getWorldTransform();
		for (const auto& callback : callbacks)
			callback(world_from_node);
		for (auto& child : children)
		{
			child->updateParentTransform(world_from_node);
		}
	}
	void updateParentTransform(const TransformSim3& p_world_from_parent)
	{
		world_from_parent = p_world_from_parent;
		updateTree();
	}
	TransformSim3 getWorldTransform() const
	{
		return world_from_parent * parent_from_node;
	}
	shared_ptr<TransformNode> addChild(const TransformSim3& parent_from_child)
	{
		auto child = make_shared<TransformNode>(parent_from_child, this);
		children.push_back(child);
		return child;
	}
	void addChild(const shared_ptr<TransformNode>& node)
	{
		node->parent = this;
		children.push_back(node);
		node->updateParentTransform(getWorldTransform());
	}
	TransformNode* getParent() const {return parent;}
	void setName(const string& p_name){name = p_name;}
	string getName() const{return name;}
	void addTransformCallback(TransformCallback p_callback)
	{
		callbacks.push_back(p_callback);
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