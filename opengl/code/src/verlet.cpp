/**
 * Copyright 2023
 * Carnegie Robotics, LLC
 * 4501 Hatfield Street, Pittsburgh, PA 15201
 * https://www.carnegierobotics.com
 *
 * This code is provided under the terms of the Master Services Agreement (the Agreement).
 * This code constitutes CRL Background Intellectual Property, as defined in the Agreement.
**/

#include "vephor/verlet.h"

namespace vephor
{

template <typename T>
void removeDestroyedObjects(vector<T>& objects)
{
	vector<size_t> alive_obj_inds;
	alive_obj_inds.reserve(objects.size());

	for (size_t i = 0; i < objects.size(); i++)
	{
		const auto& obj = objects[i];
		
		if (!obj->checkDestroy())
			alive_obj_inds.push_back(i);
	}
	
	if (alive_obj_inds.size() < objects.size())
	{
		vector<T> alive_objects;
		alive_objects.reserve(objects.size());
		
		for (auto ind : alive_obj_inds)
		{
			alive_objects.push_back(std::move(objects[ind]));
		}
		
		objects = move(alive_objects);
	}
}

void Verlet::update(float dt)
{
    // Verlet update
    for (auto& obj : objects)
    {
        if (obj->mass == 0.0f)
            continue;

        Vec3 vel = obj->getPos() - obj->last_pos;
        if (obj->reset_velocity)
		{
			vel = Vec3::Zero();
		}
		
		obj->reset_velocity = false;
        obj->last_pos = obj->getPos();
        obj->setPos(obj->getPos() + vel + obj->offset + Vec3(0,0,grav_acc) * dt * dt / 2.0);
		obj->offset = Vec3::Zero();
    }

    for (int i = 0; i < 2; i++)
    {
        for (const auto& constraint : constraints)
        {
            const Vec3& pos1 = constraint.obj1->getPos();
            const Vec3& pos2 = constraint.obj2->getPos();

            Vec3 vec = pos1 - pos2;
            Vec3 mean = (pos1 + pos2) / 2.0;
            float dist = vec.norm();
			if (dist < 1e-3)
				vec = Vec3(1,0,0);
			else
				vec /= dist;

            //v4print pos1.transpose(), pos2.transpose(), vec.transpose(), mean.transpose(), dist, constraint.dist, "P1", (mean + vec * constraint.dist / 2.0).transpose(), "P2", (mean - vec * constraint.dist / 2.0).transpose();

            float apply_dist = dist + (constraint.dist - dist) * 0.9f;

            if (constraint.obj1->mass > 0.0f && constraint.obj2->mass > 0.0f)
            {
                constraint.obj1->setPos(mean + vec * apply_dist / 2.0);
                constraint.obj2->setPos(mean - vec * apply_dist / 2.0);
            }
            else if (constraint.obj1->mass > 0.0f)
            {
                constraint.obj1->setPos(pos2 + vec * apply_dist);
            }
            else
            {
                constraint.obj2->setPos(pos1 - vec * apply_dist);
            }
        }
    }

    // TODO: different sized hashes for different sized objects
    SpatialHash<size_t, 3> obj_hash(30.0);

    // Collision update
    for (const auto& obj1 : infinite_objs)
    {
        for (const auto& obj2 : finite_objs)
        {
            compareObjects(obj2, obj1, dt);
        }
    }
	
    for (size_t i = 0; i < finite_objs.size(); i++)
    {
        obj_hash.addAndFanOut(finite_objs[i]->getPos(), i);
    }

    for (size_t i = 0; i < finite_objs.size(); i++)
    {
        auto nearby_objs = obj_hash.lookup(finite_objs[i]->getPos());

        for (const auto& j : nearby_objs)
        {
            if (i >= j)
                continue;

            compareObjects(finite_objs[i], finite_objs[j], dt);
        }
    }

    /*for (size_t i = 0; i < objects.size(); i++)
    {
        for (size_t j = i+1; j < objects.size(); j++)
        {
            compareObjects(objects[i].get(), objects[j].get(), dt);
        }
    }*/
	
	removeDestroyedObjects(infinite_objs);
	removeDestroyedObjects(finite_objs);
	removeDestroyedObjects(objects);
}

void Verlet::compareObjects(PhysicsObject* obj1, PhysicsObject* obj2, float dt)
{
    if (obj1->mass == 0.0f && obj2->mass == 0.0f)
        return;

    Vec3 push_dir;
    float dist = checkCollisionDist(*obj1, *obj2, push_dir);
    
    if (dist > 0.0f)
    {
        Vec3 total_push = push_dir * dist;

        if (obj1->mass == 0.0f)
        {
            if (obj1->water)
                obj2->setPos(obj2->getPos() - Vec3(0,0,grav_acc) * dt * dt);
            else
                obj2->setPos(obj2->getPos() - total_push);
        }
        else if (obj2->mass == 0.0f)
        {
            if (obj2->water)
                obj1->setPos(obj1->getPos() - Vec3(0,0,grav_acc) * dt * dt);
            else
                obj1->setPos(obj1->getPos() + total_push);
        }
        else
        {
            float mass_weight = obj1->mass / (obj1->mass + obj2->mass);

            obj1->setPos(obj1->getPos() + (1-mass_weight)*total_push);
            obj2->setPos(obj2->getPos() - mass_weight*total_push);
        }
		
		obj1->onCollision(*obj2);
		obj2->onCollision(*obj1);
    }
}

float Verlet::checkSpherePlaneCollisionDist(const PhysicsObject& sphere, const PhysicsObject& plane, Vec3& push_dir)
{
    const float& sphere_rad = sphere.shape->params[0] * sphere.getScale();

    const Vec3& normal = plane.shape->params;
    float dist = normal.dot(sphere.getPos() - plane.getPos());
    if (dist < sphere_rad)
    {
        push_dir = normal;
        return -dist + sphere_rad;
    }

    return 0.0f;
}

float Verlet::checkSphereSolidCollisionDist(const PhysicsObject& sphere, const PhysicsObject& solid, Vec3& push_dir)
{
	const float& sphere_rad = sphere.shape->params[0] * sphere.getScale() / solid.getScale();

    Vec3 offset = sphere.getPos() - solid.getPos();
	offset /= solid.getScale();
	offset = solid.getOrient().inverse() * offset;

    auto s = std::static_pointer_cast<SolidShape>(solid.shape);
	
	float dist = calcSphereSolidPushDist(
		offset,
		sphere_rad,
		s->solid,
		push_dir
	);
	
	push_dir = solid.getOrient() * push_dir;
	
	return -dist * solid.getScale();
}

float Verlet::checkSphereHeightMapCollisionDist(const PhysicsObject& sphere, const PhysicsObject& hm, Vec3& push_dir)
{
	const float& sphere_rad = sphere.shape->params[0] * sphere.getScale() / hm.getScale();
	
	Vec3 offset = sphere.getPos() - hm.getPos();
	offset /= hm.getScale();
	offset = hm.getOrient().inverse() * offset;
	
	auto h = std::static_pointer_cast<HeightMapShape>(hm.shape);
	
	Vec3 orig_offset = offset;
	Vec2 offset2 = offset.head<2>();
	
	Vec2i lower = ((offset2 + h->center - Vec2(sphere_rad, sphere_rad)) / h->res).array().floor().cast<int>();
	Vec2i upper = ((offset2 + h->center + Vec2(sphere_rad, sphere_rad)) / h->res).array().floor().cast<int>();
	//lower += h->center_index;
	//upper += h->center_index;
	
	//v4print "HM coll pre:", lower.transpose(), upper.transpose();
	
	if (lower[0] < 0)
		lower[0] = 0;
	if (lower[1] < 0)
		lower[1] = 0;
	if (upper[0] >= h->info_by_cell.rows())
		upper[0] = h->info_by_cell.rows() - 1;
	if (upper[1] >= h->info_by_cell.cols())
		upper[1] = h->info_by_cell.cols() - 1;
	
	Vec2 diag_norm(-0.7071,0.7071);
	
	//v4print "HM coll:", lower.transpose(), upper.transpose();
	
	for (int r = lower[0]; r <= upper[0]; r++)
	{
		for (int c = lower[1]; c <= upper[1]; c++)
		{
			const auto& info = h->info_by_cell(r,c);
			
			if (info.min_z - offset[2] > sphere_rad)
				continue;
			if (offset[2] - info.max_z > sphere_rad)
				continue;
			
			float diag_dist = diag_norm.dot(offset2) + info.diag_plane_offset;
			
			//v4print r,c,diag_dist;
			
			//diag_dist *= -1;
			float plane0_diag_dist = diag_dist - sphere_rad;
			if (plane0_diag_dist < 0)
			{
				float dist = info.planes[0].head<3>().dot(offset) + info.planes[0][3] - sphere_rad;
				//v4print "p0:", dist, info.planes[0].transpose();
				if (dist < 0)
				{
					offset -= info.planes[0].head<3>() * dist; 
					offset2 = offset.head<2>();
					diag_dist = diag_norm.dot(offset2) + info.diag_plane_offset;
				}
			}
			
			float plane1_diag_dist = -diag_dist - sphere_rad; 
			if (plane1_diag_dist < 0)
			{
				float dist = info.planes[1].head<3>().dot(offset) + info.planes[1][3] - sphere_rad;
				//v4print "p1:", dist, info.planes[1].transpose();
				if (dist < 0)
				{
					offset -= info.planes[1].head<3>() * dist; 
					offset2 = offset.head<2>();
				}
			}
		}
	}
	
	Vec3 push_vec = offset - orig_offset;
	
	float push_dist = push_vec.norm();
	
	if (push_dist < 1e-3)
		return 0;
	
	push_dir = push_vec / push_dist;
	
	return push_dist;
}

float Verlet::checkCollisionDist(const PhysicsObject& obj1, const PhysicsObject& obj2, Vec3& push_dir)
{
    ShapeType type1 = obj1.shape->type;
    ShapeType type2 = obj2.shape->type;

    if (type1 == ShapeType::SPHERE && type2 == ShapeType::PLANE)
    {
        return checkSpherePlaneCollisionDist(obj1, obj2, push_dir);
    }
    else if (type1 == ShapeType::PLANE && type2 == ShapeType::SPHERE)
    {
        float dist = checkSpherePlaneCollisionDist(obj2, obj1, push_dir);
        push_dir *= -1;
        return dist;
    }
 	else if (type1 == ShapeType::SPHERE && type2 == ShapeType::SOLID)
    {
        return checkSphereSolidCollisionDist(obj1, obj2, push_dir);
    }
    else if (type1 == ShapeType::SOLID && type2 == ShapeType::SPHERE)
    {
        float dist = checkSphereSolidCollisionDist(obj2, obj1, push_dir);
        push_dir *= -1;
        return dist;
    }
	else if (type1 == ShapeType::SPHERE && type2 == ShapeType::HEIGHT_MAP)
    {
        return checkSphereHeightMapCollisionDist(obj1, obj2, push_dir);
    }
    else if (type1 == ShapeType::HEIGHT_MAP && type2 == ShapeType::SPHERE)
    {
        float dist = checkSphereHeightMapCollisionDist(obj2, obj1, push_dir);
        push_dir *= -1;
        return dist;
    }
    else
    if (type1 == ShapeType::SPHERE && type2 == ShapeType::SPHERE)
    {
        const float& obj1_rad = obj1.shape->params[0] * obj1.getScale();
        const float& obj2_rad = obj2.shape->params[0] * obj2.getScale();

        Vec3 vec = obj1.getPos() - obj2.getPos();
        float dist_sq = dot2(vec);
        if (dist_sq < pow(obj1_rad + obj2_rad, 2.0f))
        {
            float dist = sqrt(dist_sq);
			if (dist < 1e-3)
				push_dir = Vec3(1,0,0);
			else
				push_dir = vec / dist;
            return obj1_rad + obj2_rad - dist;
        }
    }

    return 0.0f;
}

}