#pragma once

#include "vephor.h"

namespace vephor
{

class Verlet
{
public:
	struct PhysicsObject;

	using CollisionCallback = std::function<void(const PhysicsObject&)>;

	enum class ShapeType
    {
        SPHERE,
        PLANE,
		SOLID,
		HEIGHT_MAP
    };

    struct Shape
    {
        ShapeType type;
        VecX params;
        float radius = 0.0f;
    };

	struct PhysicsObject
    {
    public:
        virtual ~PhysicsObject(){}
        virtual void setPos(const Vec3& pos) = 0;
        virtual Vec3 getPos() const = 0;
		virtual float getScale() const = 0;
		virtual Orient3 getOrient() const = 0;
		void onCollision(const PhysicsObject& other)
		{
			if (collision_callback)
			{
				collision_callback(other);
			}
		}
		void setCollisionCallback(CollisionCallback callback)
		{
			collision_callback = callback;
		}
		void applyOffset(const Vec3& off)
		{
			offset += off;
		}
		void resetVelocity()
		{
			reset_velocity = true;
		}

		bool reset_velocity = false;
		Vec3 offset = Vec3::Zero();
        Vec3 last_pos = Vec3::Zero();
        shared_ptr<Shape> shape;
        float mass = 0.0f;
        bool water = false;
		CollisionCallback collision_callback = NULL;
		
		void setDestroy() {destroy = true;}
		bool checkDestroy() const {return destroy;}
	private:
		bool destroy = false;
    };
	
	struct SolidShape : public Shape
    {
		Solid solid;
	};
	
	struct HeightMapCellInfo
	{
		array<Vec4, 2> planes;
		float diag_plane_offset;
		float min_z;
		float max_z;
	};
	
	struct HeightMapShape : public Shape
    {
		Vec2 center;
		Vec2i center_index;
		Eigen::Matrix<HeightMapCellInfo, Eigen::Dynamic, Eigen::Dynamic> info_by_cell;
		float res;
	};

    static shared_ptr<Shape> makeSphere(float radius)
    {
        auto s = make_shared<Shape>();
        s->type = ShapeType::SPHERE;
        s->params.resize(1);
        s->params[0] = radius;
        s->radius = radius * 2;
        return s;
    }

    static shared_ptr<Shape> makePlane(const Vec3& normal)
    {
        auto s = make_shared<Shape>();
        s->type = ShapeType::PLANE;
        s->params = normal;
        return s;
    }
	
	static shared_ptr<Shape> makeSolid(const Solid& solid)
	{
		auto s = make_shared<SolidShape>();
		s->type = ShapeType::SOLID;
        s->solid = solid;
        return s;
	}
	
	static shared_ptr<Shape> makeHeightMap(const MatX& heights, float res)
	{
		auto s = make_shared<HeightMapShape>();
		s->type = ShapeType::HEIGHT_MAP;
		s->res = res;
		s->center_index = Vec2i(heights.rows()/2, heights.cols()/2);
		s->info_by_cell.resize(heights.rows() - 1, heights.cols() - 1);
		
		Vec2 center((heights.rows()-1)/2.0f * res, (heights.cols()-1)/2.0f * res);
		s->center = center;
		
		for (int r = 0; r < heights.rows() - 1; r++)
		{
			float x = r * res - center[0];

			for (int c = 0; c < heights.cols() - 1; c++)
			{
				float y = c * res - center[1];
				Vec3 v00(x, y, -heights(r,c));
				Vec3 v10(x+res, y, -heights(r+1,c));
				Vec3 v01(x, y+res, -heights(r,c+1));
				Vec3 v11(x+res, y+res, -heights(r+1,c+1));
				
				Vec3 norm0 = (v11-v00).cross(v10-v00);
				Vec3 norm1 = (v01-v00).cross(v11-v00);
				norm0 /= norm0.norm();
				norm1 /= norm1.norm();
				
				//s->info_by_cell(r,c).corner_vertex = v00;
				s->info_by_cell(r,c).diag_plane_offset = 0.7071*v00[0] - 0.7071*v00[1];
				s->info_by_cell(r,c).min_z = min({
					v00[2],
					v01[2],
					v10[2],
					v11[2]
				});
				s->info_by_cell(r,c).max_z = max({
					v00[2],
					v01[2],
					v10[2],
					v11[2]
				});
				
				Vec4 plane0;
				plane0.head<3>() = norm0;
				plane0[3] = -norm0.dot(v00);
				s->info_by_cell(r,c).planes[0] = plane0;
				
				Vec4 plane1;
				plane1.head<3>() = norm1;
				plane1[3] = -norm1.dot(v00);
				s->info_by_cell(r,c).planes[1] = plane1;
			}
		}
		
        return s;
	}

    template <class T>
    struct TPhysicsObject : public PhysicsObject
    {
    public:
        TPhysicsObject(const shared_ptr<T>& p_obj)
        : obj(p_obj)
        {} 
        virtual void setPos(const Vec3& pos) override
        {
            obj->setPos(pos);
        }
        virtual Vec3 getPos() const override
        {
            return obj->getPos();
        }
		virtual float getScale() const override
		{
			return obj->getScale();
		}
		virtual Orient3 getOrient() const override
		{
			return obj->getOrient();
		}
    private:
        shared_ptr<T> obj;
    };

    struct Constraint
    {
        PhysicsObject* obj1;
        PhysicsObject* obj2;
        float dist;
    };

    Verlet(float p_grav_acc=9.8)
    : grav_acc(p_grav_acc)
    {}
    template <class T>
    PhysicsObject* add(const shared_ptr<T>& obj, const shared_ptr<Shape>& shape, float mass = 0.0f, bool water = false)
    {
        unique_ptr<PhysicsObject> inner_obj = make_unique<TPhysicsObject<T>>(obj);
        objects.push_back(move(inner_obj));
        (*objects.rbegin())->last_pos = obj->getPos();
        (*objects.rbegin())->shape = shape;
        (*objects.rbegin())->mass = mass;
        (*objects.rbegin())->water = water;
		
		auto obj_ptr = objects.rbegin()->get();
		if (obj_ptr->shape->radius == 0.0f)
			infinite_objs.push_back(obj_ptr);
		else
			finite_objs.push_back(obj_ptr);
		
        return obj_ptr;
    }
    void addConstraint(PhysicsObject* obj1, PhysicsObject* obj2, float dist)
    {
        constraints.push_back(Constraint{obj1, obj2, dist});
    }
    void update(float dt);
private:
    float grav_acc;

    void compareObjects(PhysicsObject* obj1, PhysicsObject* obj2, float dt);
    float checkSpherePlaneCollisionDist(const PhysicsObject& sphere, const PhysicsObject& plane, Vec3& push_dir);
    float checkSphereSolidCollisionDist(const PhysicsObject& sphere, const PhysicsObject& solid, Vec3& push_dir);
	float checkSphereHeightMapCollisionDist(const PhysicsObject& sphere, const PhysicsObject& hm, Vec3& push_dir);
	float checkCollisionDist(const PhysicsObject& obj1, const PhysicsObject& obj2, Vec3& push_dir);

    vector<unique_ptr<PhysicsObject>> objects;
	vector<PhysicsObject*> infinite_objs;
	vector<PhysicsObject*> finite_objs;
    vector<Constraint> constraints;
};

}