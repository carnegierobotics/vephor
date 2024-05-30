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

inline Mat4 makePerspectiveProj(float fov_deg, const Vec2i& window_size, float near_z, float far_z)
{
    Mat4 proj;
    proj.setZero();

    float aspect = (float)window_size[0] / window_size[1];

    float tan_half_angle = tan(fov_deg / 2 * M_PI / 180.0);

    proj(0,0) = 1 / (aspect * tan_half_angle);
    proj(1,1) = 1 / tan_half_angle;
    proj(2,2) = -far_z / (far_z - near_z);
    proj(3,2) = -far_z * near_z / (far_z - near_z);

    proj(2,3) = -1;

    return proj;
}

inline Mat4 makeOrthoProj(const Vec3& near, const Vec3& far)
{
    Mat4 proj;
    proj.setZero();

    proj(0,0) = 2/(far[0] - near[0]);
    proj(1,1) = 2/(far[1] - near[1]);
    proj(2,2) = -2/(far[2] - near[2]);
    proj(3,3) = 1;

    proj(0,3) = -(far[0] + near[0]) / (far[0] - near[0]);
    proj(1,3) = -(far[1] + near[1]) / (far[1] - near[1]);
    proj(2,3) = -(far[2] + near[2]) / (far[2] - near[2]);

    return proj.transpose();
}

inline Transform3 makeLookAtTransform(const Vec3& look_at, const Vec3& look_from, const Vec3& up_hint)
{
    Vec3 forward = look_at - look_from;
    forward /= forward.norm();

    Vec3 up_hint_norm = up_hint / up_hint.norm();

    if (fabs(forward.dot(up_hint_norm)) >= 1.0f)
    {
        // Nudge forward slightly to get it away from up
        forward += findCrossVec(forward)*0.001f;
        forward /= forward.norm();
    }

    Vec3 up = forward.cross(up_hint_norm).cross(forward);
    up /= up.norm();

    Vec3 right = forward.cross(up);
    right /= right.norm();

    Mat3 world_from_cam;
    world_from_cam.col(2) = -forward;
    world_from_cam.col(1) = up;
    world_from_cam.col(0) = right;

    return Transform3(look_from, Orient3::fromMatrix(world_from_cam)).inverse();
}

}