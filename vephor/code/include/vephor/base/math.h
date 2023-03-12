#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <iostream>
#include <fstream>
#include <memory>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <array>
#include <chrono>
#include <Eigen/Core>
#include <manif/manif.h>

namespace vephor
{

using std::vector;
using std::unordered_set;
using std::unordered_map;
using std::array;
using std::min;
using std::max;
using std::string;
using std::to_string;
using std::istream;
using std::ifstream;
using std::ostream;
using std::ofstream;
using std::cout;
using std::endl;
using std::shared_ptr;
using std::make_shared;
using std::unique_ptr;
using std::weak_ptr;
using std::make_unique;
using std::chrono::time_point;
using std::chrono::high_resolution_clock;
using std::chrono::nanoseconds;
using std::chrono::duration_cast;
using std::numeric_limits;
using std::pair;
using std::swap;

using Vec2 = Eigen::Matrix<float, 2, 1>;
using Vec2d = Eigen::Matrix<double, 2, 1>;
using Vec3u = Eigen::Matrix<uint8_t, 3, 1>;
using Vec3 = Eigen::Matrix<float, 3, 1>;
using Vec3d = Eigen::Matrix<double, 3, 1>;
using Vec3Ref = Eigen::Ref<const Vec3>;
using Vec4u = Eigen::Matrix<uint8_t, 4, 1>;
using Vec4 = Eigen::Matrix<float, 4, 1>;
using Vec4d = Eigen::Matrix<double, 4, 1>;
using Vec4Ref = Eigen::Ref<const Vec4>;
using VecX = Eigen::Matrix<float, Eigen::Dynamic, 1>;
using VecXRef = Eigen::Ref<const VecX>;
using VecXMap = Eigen::Map<const VecX>;
using VecXui = Eigen::Matrix<uint32_t, Eigen::Dynamic, 1>;

using Vec2i = Eigen::Matrix<int32_t, 2, 1>;
using Vec3i = Eigen::Matrix<int32_t, 3, 1>;

using Mat2 = Eigen::Matrix<float, 2, 2>;
using Mat3 = Eigen::Matrix<float, 3, 3>;
using Mat4 = Eigen::Matrix<float, 4, 4>;
using MatX = Eigen::Matrix<float, Eigen::Dynamic, Eigen::Dynamic>;
using MatXRef = Eigen::Ref<const MatX>;
using MatXMap = Eigen::Map<const MatX>;

class Color
{
public:
	Color()
	: inner(0,0,0,1)
	{}
	Color(float r, float g, float b, float a=1.0f)
	: inner(r,g,b,a)
	{}
	Color(const Vec3& vec)
	{
		inner.head<3>() = vec;
		inner[3] = 1;
	}
	Color(const Vec3u& vec)
	{
		inner.head<3>() = vec.cast<float>() / 255.0f;
		inner[3] = 1;
	}
	Color(const Vec4& vec)
	: inner(vec)
	{}
	Color(const Vec4u& vec)
	: inner(vec.cast<float>() / 255.0f)
	{}
	const Vec3 getRGB() const
	{
		return inner.head<3>();
	}
	const Vec4& getRGBA() const
	{
		return inner;
	}
	float getAlpha() const
	{
		return inner[3];
	}
private:
	Vec4 inner;
};

class Orient3
{
public:
	Orient3()
	: q(Eigen::Quaternionf::Identity())
	{}
	Orient3(const Eigen::Quaternionf& p_q)
	: q(p_q)
	{}
	Orient3(const Vec3& r)
	{
		float angle = r.norm();
		Vec3 axis;
		if (angle > 1e-3)
			axis = r / angle;
		else
			axis = Vec3(1,0,0);

		q = Eigen::Quaternionf(Eigen::AngleAxisf(angle, axis));
	}
	static Orient3 fromMatrix(const Mat3& R)
	{
		Eigen::AngleAxisf aa;
		aa.fromRotationMatrix(R);
		return Orient3(Eigen::Quaternionf(aa));
	}
	Orient3 inverse() const
	{
		return Orient3(q.inverse());
	}
	void normalize()
	{
		q.normalize();
	}
	Eigen::Quaternionf quat() const
	{
		return q;
	}
	Orient3 operator* (const Orient3& other) const
	{
		return q * other.q;
	}
	Vec3 operator* (const Vec3& v) const
	{
		return q * v;
	}
private:
	Eigen::Quaternionf q;
};

class Transform3
{
public:
	Transform3()
	: transform(manif::SE3f::Identity())
	{}
	Transform3(const Vec3& t, const Orient3& r)
	: transform(t, r.quat())
	{}
	Transform3(const Vec3& t, const Vec3& r = Vec3::Zero())
	: transform(t, Orient3(r).quat())
	{}
	Transform3(const manif::SE3f& p_transform)
	: transform(p_transform)
	{}
	Transform3 inverse() const
	{
		return Transform3(transform.inverse());
	}
	Mat4 matrix() const
	{
		return transform.transform();
	}
	Mat3 rotation() const
	{
		return transform.rotation();
	}
	Orient3 quat() const
	{
		return Orient3(transform.quat());
	}
	void quat(const Orient3& q)
	{
		transform.quat(q.quat());
	}
	Transform3 operator* (const Transform3& other) const
	{
		return transform * other.transform;
	}
	Vec3 operator* (const Vec3& v) const
	{
		return transform.act(v);
	}
	Vec3 translation() const
	{
		return transform.translation();
	}
	void translation(const Vec3& t)
	{
		transform.translation(t);
	}
private:
	manif::SE3f transform;
};

struct TransformSim3
{
	TransformSim3()
	{}
	TransformSim3(const Transform3& p_transform, float p_scale = 1.0f)
	: transform(p_transform), scale(p_scale)
	{}
	TransformSim3(const Vec3& p_t, const Vec3& p_r = Vec3::Zero(), float p_scale = 1.0f)
	: transform(p_t,p_r), scale(p_scale)
	{}
	TransformSim3 operator* (const TransformSim3& other) const
	{
		Transform3 scaled_transform = other.transform;
		scaled_transform.translation(scaled_transform.translation() * scale);
		
		return TransformSim3(transform * scaled_transform, scale * other.scale);
	}
	TransformSim3 operator* (const Transform3& other) const
	{
		Transform3 scaled_transform = other;
		scaled_transform.translation(scaled_transform.translation() * scale);
		
		return TransformSim3(transform * scaled_transform, scale);
	}
	Vec3 operator* (const Vec3& v) const
	{
		return transform * (scale * v);
	}
	Vec3 translation() const
	{
		return transform.translation();
	}
	void translation(const Vec3& t)
	{
		transform.translation(t);
	}
	Mat3 rotation() const
	{
		return transform.rotation();
	}
	Orient3 quat() const
	{
		return Orient3(transform.quat());
	}
	void quat(const Orient3& q)
	{
		transform.quat(q.quat());
	}
	Mat4 matrix() const
	{
		Mat4 m = transform.matrix();
		m.block<3,3>(0,0) *= scale;
		return m;
	}
	
	Transform3 transform;
	float scale = 1.0f;
};

inline float sign(float v)
{
    return v >= 0 ? 1 : -1;
}

inline float dot2(const Vec3& v)
{
    return v.dot(v);
}

inline float clamp(float v, float vmin, float vmax)
{
    return min(max(v, vmin), vmax);
}

inline Vec3 makeFacing(Vec3 facing, Vec3 base_facing = Vec3(0,0,1))
{
	facing = facing / facing.norm();
	base_facing = base_facing / base_facing.norm();
	
	Vec3 axis = base_facing.cross(facing);
	float axis_mag = axis.norm();
	if (axis_mag > 1e-3)
		axis /= axis_mag;
	else
		axis = Vec3(1,0,0);
	
	float angle = acos(clamp(facing.dot(base_facing), -1.0f, 1.0f));
	
	return angle * axis;
}

inline MatX toMat(const vector<Vec2>& v)
{
	MatX mat(v.size(), 2);
	
	for (int i = 0; i < v.size(); i++)
	{
		mat.row(i) = v[i].transpose();
	}
	
	return mat;
}

inline MatX toMat(const vector<Vec3>& v)
{
	MatX mat(v.size(), 3);
	
	for (int i = 0; i < v.size(); i++)
	{
		mat.row(i) = v[i].transpose();
	}
	
	return mat;
}

inline MatX toMat(const vector<Vec4>& v)
{
	MatX mat(v.size(), 4);
	
	for (int i = 0; i < v.size(); i++)
	{
		mat.row(i) = v[i].transpose();
	}
	
	return mat;
}

inline Vec3 findCrossVec(const Vec3& v)
{
    vector<Vec3> test_vecs = {
        Vec3(1,0,0),
        Vec3(0,1,0),
        Vec3(0,0,1),
    };

    float min_dval = 2.0;
    Vec3 min_v;

    for (const auto& other_v : test_vecs)
    {
        float dval = fabs(other_v.dot(v));
        if (dval < min_dval)
        {
            min_dval = dval;
            min_v = other_v;
        }
    }

    min_v = v.cross(min_v).cross(v);
    min_v /= min_v.norm();
    return min_v;
}

inline Mat3 skew(const Vec3& v)
{
    Mat3 out = Mat3::Zero();

    out(2,1) = v[0];
    out(2,0) = -v[1];
    out(1,0) = v[2];

    out(1,2) = -v[0];
    out(0,2) = v[1];
    out(0,1) = -v[2];

    return out;
}

inline Mat3 rodrigues(const Vec3& v)
{
    float angle = v.norm();
    if (angle == 0)
        return Mat3::Identity();
    Vec3 axis = v / angle;
    Mat3 skew_axis = skew(axis);

    return Mat3::Identity() + sin(angle)*skew_axis + (1-cos(angle))*skew_axis*skew_axis;
}

}
