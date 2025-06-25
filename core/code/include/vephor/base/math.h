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

#include <stdio.h>
#include <stdlib.h>
#include <cstdint>
#include <string>
#include <iostream>
#include <fstream>
#include <memory>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <array>
#include <chrono>
#include <random>
#include <Eigen/Core>
#include <manif/manif.h>

namespace vephor
{

//
// STL
//

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

//
// Utility macros
//

#define VEPHOR_CONCAT2(a, b) a##b
#define VEPHOR_CONCAT3(a, b, c) a##b##c

//
// Matrices
//

template <int Rows, int Cols, typename T = float>
using Mat = Eigen::Matrix<T, Rows, Cols>;

template <int Rows, int Cols>
using Matf = Mat<Rows, Cols, float>;

template <int Rows, int Cols>
using Matd = Mat<Rows, Cols, double>;

template <int Rows, int Cols>
using Matu = Mat<Rows, Cols, uint8_t>;

template <int Rows, int Cols>
using Mati = Mat<Rows, Cols, int32_t>;

template <int Rows, int Cols>
using Matui = Mat<Rows, Cols, uint32_t>;

template <int Rows, int Cols, typename T = float>
using MatRef = Eigen::Ref<const Mat<Rows, Cols, T>>;

template <int Rows, int Cols, typename T = float>
using MatMap = Eigen::Map<const Mat<Rows, Cols, T>>;

#define VEPHOR_DEFINE_CONCRETE_MATRIX_TYPES(name, rows, cols)                                                          \
    using VEPHOR_CONCAT2(Mat, name) = Mat<rows, cols>;                                                                 \
    using VEPHOR_CONCAT3(Mat, name, Ref) = MatRef<rows, cols>;                                                         \
    using VEPHOR_CONCAT3(Mat, name, Map) = MatMap<rows, cols>;                                                         \
    using VEPHOR_CONCAT3(Mat, name, f) = Matf<rows, cols>;                                                             \
    using VEPHOR_CONCAT3(Mat, name, d) = Matd<rows, cols>;                                                             \
    using VEPHOR_CONCAT3(Mat, name, u) = Matu<rows, cols>;                                                             \
    using VEPHOR_CONCAT3(Mat, name, i) = Mati<rows, cols>;                                                             \
    using VEPHOR_CONCAT3(Mat, name, ui) = Matui<rows, cols>;

VEPHOR_DEFINE_CONCRETE_MATRIX_TYPES(2, 2, 2)
VEPHOR_DEFINE_CONCRETE_MATRIX_TYPES(3, 3, 3)
VEPHOR_DEFINE_CONCRETE_MATRIX_TYPES(4, 4, 4)
VEPHOR_DEFINE_CONCRETE_MATRIX_TYPES(X, Eigen::Dynamic, Eigen::Dynamic)

#undef VEPHOR_DEFINE_CONCRETE_MATRIX_TYPES

//
// Vectors
//

template <int Rows, typename T = float>
using Vec = Mat<Rows, 1, T>;

template <int Rows>
using Vecf = Vec<Rows, float>;

template <int Rows>
using Vecd = Vec<Rows, double>;

template <int Rows>
using Vecu = Vec<Rows, uint8_t>;

template <int Rows>
using Veci = Vec<Rows, int32_t>;

template <int Rows>
using Vecui = Vec<Rows, uint32_t>;

template <int Rows, typename T = float>
using VecRef = Eigen::Ref<const Vec<Rows, T>>;

template <int Rows, typename T = float>
using VecMap = Eigen::Map<const Vec<Rows, T>>;

#define VEPHOR_DEFINE_CONCRETE_VECTOR_TYPES(name, rows)                                                                \
    using VEPHOR_CONCAT2(Vec, name) = Vec<rows>;                                                                       \
    using VEPHOR_CONCAT3(Vec, name, Ref) = VecRef<rows>;                                                               \
    using VEPHOR_CONCAT3(Vec, name, Map) = VecMap<rows>;                                                               \
    using VEPHOR_CONCAT3(Vec, name, f) = Vecf<rows>;                                                                   \
    using VEPHOR_CONCAT3(Vec, name, d) = Vecd<rows>;                                                                   \
    using VEPHOR_CONCAT3(Vec, name, u) = Vecu<rows>;                                                                   \
    using VEPHOR_CONCAT3(Vec, name, i) = Veci<rows>;                                                                   \
    using VEPHOR_CONCAT3(Vec, name, ui) = Vecui<rows>;

VEPHOR_DEFINE_CONCRETE_VECTOR_TYPES(2, 2)
VEPHOR_DEFINE_CONCRETE_VECTOR_TYPES(3, 3)
VEPHOR_DEFINE_CONCRETE_VECTOR_TYPES(4, 4)
VEPHOR_DEFINE_CONCRETE_VECTOR_TYPES(X, Eigen::Dynamic)

#undef VEPHOR_DEFINE_CONCRETE_VECTOR_TYPES

//
// Colors
//

class Color
{
public:
    constexpr Color() : r(0.0f), g(0.0f), b(0.0f), a(1.0f)
    {
    }

    constexpr Color(const float p_r, const float p_g, const float p_b, const float p_a = 1.0f)
        : r(p_r), g(p_g), b(p_b), a(p_a)
    {
    }

    Color(const Vec3 &vec) : r(vec(0)), g(vec(1)), b(vec(2)), a(1.0f)
    {
    }

    Color(const Vec3u &vec)
        : r(static_cast<float>(vec(0)) / 255.0f), g(static_cast<float>(vec(1)) / 255.0f),
          b(static_cast<float>(vec(2)) / 255.0f), a(1.0f)
    {
    }

    Color(const Vec4 &vec) : r(vec(0)), g(vec(1)), b(vec(2)), a(vec(3))
    {
    }

    Color(const Vec4u &vec)
        : r(static_cast<float>(vec(0)) / 255.0f), g(static_cast<float>(vec(1)) / 255.0f),
          b(static_cast<float>(vec(2)) / 255.0f), a(static_cast<float>(vec(3)) / 255.0f)
    {
    }

    const Vec3 getRGB() const
    {
        return {r, g, b};
    }

    const Vec4 getRGBA() const
    {
        return {r, g, b, a};
    }

    float getAlpha() const
    {
        return a;
    }

	static Color fromHSL(float H, float S, float L)
	{
		H = fmod(H, 360.0f);

		// From LLM
		float C = (1 - std::fabs(2 * L - 1)) * S;
		float H_prime = H / 60.0;  // Divide by 60 to get the sector of the hue circle
		float X = C * (1 - std::fabs(fmod(H_prime, 2) - 1));
		
		float R1, G1, B1;
		
		if (0 <= H_prime && H_prime < 1) {
			R1 = C; G1 = X; B1 = 0;
		} else if (1 <= H_prime && H_prime < 2) {
			R1 = X; G1 = C; B1 = 0;
		} else if (2 <= H_prime && H_prime < 3) {
			R1 = 0; G1 = C; B1 = X;
		} else if (3 <= H_prime && H_prime < 4) {
			R1 = 0; G1 = X; B1 = C;
		} else if (4 <= H_prime && H_prime < 5) {
			R1 = X; G1 = 0; B1 = C;
		} else if (5 <= H_prime && H_prime < 6) {
			R1 = C; G1 = 0; B1 = X;
		} else {
			R1 = 0; G1 = 0; B1 = 0;
		}

		float m = L - C / 2.0;
		float R = R1 + m;
		float G = G1 + m;
		float B = B1 + m;

		return Color(R,G,B);
	}

	static Color random()
	{
		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_int_distribution<> dist(0, 360);

		return fromHSL(dist(gen),0.5,0.5);
	}

private:
    float r, g, b, a;
};

//
// Transforms
//

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
	Mat3 matrix() const
	{
		return q.toRotationMatrix();
	}
	Eigen::Quaternionf quat() const
	{
		return q;
	}
	Vec3 rvec() const
	{
		Eigen::AngleAxisf aa(q);
		return aa.angle() * aa.axis();
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
	Vec3 rvec() const
	{
		Eigen::AngleAxisf aa(transform.quat());
		return aa.angle() * aa.axis();
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
	void setTranslation(const Vec3& t)
	{
		transform.translation(t);
	}
	void setRotation(const Orient3& o)
	{
		transform.quat(o.quat());
	}
	manif::SE3f manifoldTransform() const
	{
		return transform;
	}
	Transform3 interp(const Transform3& other, float perc) const
	{
		// To get the shortest path, we take the delta transform and interpolate that
		Transform3 self_from_other = this->inverse() * other;
		return transform * (self_from_other.transform.log() * perc).exp();

		// This is the absolute version, this has caused problems with angle wrap
		//return Transform3((transform.log() * (1 - perc) + other.transform.log() * perc).exp());
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
		scaled_transform.setTranslation(scaled_transform.translation() * scale);
		
		return TransformSim3(transform * scaled_transform, scale * other.scale);
	}
	TransformSim3 operator* (const Transform3& other) const
	{
		Transform3 scaled_transform = other;
		scaled_transform.setTranslation(scaled_transform.translation() * scale);
		
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
	Mat3 rotation() const
	{
		return transform.rotation();
	}
	Orient3 quat() const
	{
		return Orient3(transform.quat());
	}
	void setTranslation(const Vec3& t)
	{
		transform.setTranslation(t);
	}
	void setRotation(const Orient3& o)
	{
		transform.setRotation(o.quat());
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
