/**
 * Copyright 2025
 * Carnegie Robotics, LLC
 * 4501 Hatfield Street, Pittsburgh, PA 15201
 * https://www.carnegierobotics.com
 *
 * This source code is licensed under the Apache License, Version 2.0
 * found in the LICENSE file in the root directory of this source tree.
**/

#include "vephor/ogl/camera/trajectory_camera.h"

#include <Eigen/Dense>

#include <algorithm>
#include <cmath>
#include <iterator>
#include <sstream>
#include <stdexcept>
#include <utility>
#include <vector>

namespace vephor
{

//
// #####################################################################################################################
//

Polynomial<float> polynomialCurveFit1D(const VecX &t, const VecX &x, const int degree, std::error_code &error_code)
{
    static PolynomialCurveFitErrorCategory const polynomial_fit_error_category{};

    if (x.size() == 0 || t.size() == 0)
    {
        error_code = {static_cast<int>(PolynomialCurveFitError::EMPTY_DATA), polynomial_fit_error_category};
        return {};
    }
    if (t.size() != x.size())
    {
        error_code = {static_cast<int>(PolynomialCurveFitError::MISMATCHED_DATA), polynomial_fit_error_category};
        return {};
    }
    if (degree < 0)
    {
        error_code = {static_cast<int>(PolynomialCurveFitError::NON_POSITIVE_POLYNOMIAL_DEGREE),
                      polynomial_fit_error_category};
        return {};
    }

    int num_points = t.size();

    MatX X(num_points, degree + 1);
    for (int i = 0; i < num_points; ++i)
    {
        for (int j = 0; j < degree + 1; ++j)
        {
            X(i, j) = std::pow(t(i), j);
        }
    }

    MatX XT_X = X.transpose() * X;
    MatX XT_Y = X.transpose() * x;

    Eigen::LDLT<MatX> ldlt_of_XT_X(XT_X);
    VecX coefficients = ldlt_of_XT_X.solve(XT_Y);

    if (ldlt_of_XT_X.info() != Eigen::Success)
    {
        error_code = {static_cast<int>(PolynomialCurveFitError::LEAST_SQUARES_FAILURE), polynomial_fit_error_category};
        return {};
    }

    error_code = {static_cast<int>(PolynomialCurveFitError::SUCCESS), polynomial_fit_error_category};
    return Polynomial<float>{coefficients};
}

//
// #####################################################################################################################
//

PolynomialND<float> polynomialCurveFitND(const std::vector<float> &t_vector,
                                         const std::vector<VecX> &x_vector,
                                         const int degree,
                                         std::error_code &error_code)
{
    static PolynomialCurveFitErrorCategory const polynomial_fit_error_category{};

    const int dimensions = x_vector.front().size();

    if (t_vector.empty() || x_vector.empty())
    {
        error_code = {static_cast<int>(PolynomialCurveFitError::EMPTY_DATA), polynomial_fit_error_category};
        return {};
    }
    if (t_vector.size() != x_vector.size())
    {
        error_code = {static_cast<int>(PolynomialCurveFitError::MISMATCHED_DATA), polynomial_fit_error_category};
        return {};
    }
    if (degree < 0)
    {
        error_code = {static_cast<int>(PolynomialCurveFitError::NON_POSITIVE_POLYNOMIAL_DEGREE),
                      polynomial_fit_error_category};
        return {};
    }
    if (dimensions < 1)
    {
        error_code = {static_cast<int>(PolynomialCurveFitError::NULL_DIMENSION), polynomial_fit_error_category};
        return {};
    }

    const auto num_points = t_vector.size();
    const VecX t = VecXMap(t_vector.data(), t_vector.size());

    std::vector<Polynomial<float>> polynomials_by_dimension;
    for (int dimension = 0; dimension < dimensions; ++dimension)
    {
        //
        // Aggregate points
        //

        VecX x(num_points);
        for (int i = 0; i < num_points; ++i)
        {
            x(i) = x_vector[i](dimension);
        }

        //
        // Fit polynomial for this dimension
        //

        std::error_code dimension_error_code;
        const auto coefficients =
            polynomialCurveFit1D(/* t */ t, /* x */ x, /* degree */ degree, /* error_code */ dimension_error_code);
        if (dimension_error_code.value() != static_cast<int>(PolynomialCurveFitError::SUCCESS))
        {
            error_code = dimension_error_code;
            return {};
        }

        polynomials_by_dimension.emplace_back(coefficients);
    }

    error_code = {static_cast<int>(PolynomialCurveFitError::SUCCESS), polynomial_fit_error_category};
    return PolynomialND<float>(polynomials_by_dimension);
}

//
// #####################################################################################################################
//

bool inRange(float value, const std::pair<float, float> &range)
{
    return (value >= range.first) && (value < range.second);
}

//
// #####################################################################################################################
//

float wrapToRange(float value, const std::pair<float, float> &range)
{
    const float span = range.second - range.first;
    if (span <= 0)
    {
        throw std::runtime_error("[wrapToRange] Invalid range.");
    }

    if (value < range.first)
    {
        return range.second - std::fmod(range.first - value, span);
    }
    if (value > range.second)
    {
        return range.first + std::fmod(value - range.second, span);
    }

    return value;
}

//
// #####################################################################################################################
//

void TrajectoryCamera::setup(const json &data, Window &window, AssetManager &assets)
{
    //
    // Background
    //

    bool background_texture_nearest;
    const auto background_image = *getCameraBackground(/* data */ data, /* nearest */ background_texture_nearest);
    const auto background_texture =
        window.getTextureFromImage(/* img */ background_image, /* nearest */ background_texture_nearest);
    const auto background = make_shared<Background>(/* tex */ background_texture);
    window.add(/* obj */ background, /* parent_from_node */ Transform3(), /* on_overlay */ false, /* layer */ -1);

    //
    // Parse data
    //

    // Parse constant parameters
    motion_mode_ = data["motion_mode"];
    speed_ = data["speed"];
    start_time_ = data["start_time"];
    polynomial_degree_ = data["polynomial_degree"];

    // Parse the trajectory
    for (const auto &trajectory_node : data["trajectory"])
    {
        trajectory_nodes_.push_back(trajectory_node);
    }

    // Ensure nodes are sorted in ascending time order
    std::sort(/* first */ trajectory_nodes_.begin(),
              /* last */ trajectory_nodes_.end(),
              /* comp */ [](const TrajectoryNode &a, const TrajectoryNode &b) -> bool { return a.time < b.time; });

    // Record the time limits for later use
    time_range_ = {trajectory_nodes_.front().time, trajectory_nodes_.back().time};
    time_span_ = time_range_.second - time_range_.first;

    //
    // Fit polynomials to the trajectory nodes
    //

    std::vector<float> times;
    std::vector<VecX> to_vectors;
    std::vector<VecX> from_vectors;
    std::vector<VecX> up_hint_vectors;
    for (const auto &trajectory_node : trajectory_nodes_)
    {
        times.emplace_back(trajectory_node.time);
        to_vectors.emplace_back(trajectory_node.to);
        from_vectors.emplace_back(trajectory_node.from);
        up_hint_vectors.emplace_back(trajectory_node.up_hint); // TODO: Should we normalize these?
    }

    //
    // TODO: Constrain looping trajectories with matching start and end points so that they are continuous at the ends.
    // TODO: Solve for individual spline segments instead of entire spline at once for more stable larger trajectories.
    //

    std::error_code error_code;

    const auto to_polynomial = polynomialCurveFitND(
        /* times */ times, /* points */ to_vectors, /* degree */ polynomial_degree_, /* error_code */ error_code);
    if (error_code.value() != static_cast<int>(PolynomialCurveFitError::SUCCESS))
    {
        throw std::runtime_error("[TrajectoryCamera::setup()] Could not fit polynomial to the trajectory to vectors.");
    }

    const auto from_polynomial = polynomialCurveFitND(
        /* times */ times, /* points */ from_vectors, /* degree */ polynomial_degree_, /* error_code */ error_code);
    if (error_code.value() != static_cast<int>(PolynomialCurveFitError::SUCCESS))
    {
        throw std::runtime_error(
            "[TrajectoryCamera::setup()] Could not fit polynomial to the trajectory from vectors.");
    }

    const auto up_hint_polynomial = polynomialCurveFitND(
        /* times */ times, /* points */ up_hint_vectors, /* degree */ polynomial_degree_, /* error_code */ error_code);
    if (error_code.value() != static_cast<int>(PolynomialCurveFitError::SUCCESS))
    {
        throw std::runtime_error(
            "[TrajectoryCamera::setup()] Could not fit polynomial to the trajectory up hint vectors.");
    }

    trajectory_ = {.to = to_polynomial, .from = from_polynomial, .up_hint = up_hint_polynomial};

    //
    // Configure initial state
    //

    time_ = start_time_;
    world_from_camera_ = computePose(time_);
    window.setCamFromWorld(world_from_camera_);
}

//
// #####################################################################################################################
//

void TrajectoryCamera::update(Window &window, const float dt, const ControlInfo &control_info)
{
    time_ += dt * speed_;

    // Keep time from overflowing.  Our longest motion cycle is 2 * time_span and all cycle times are multiples of
    // time_span.
    time_ = wrapToRange(/* value */ time_,
                        /* range */ {time_range_.first, time_range_.first + 2 * time_span_});

    world_from_camera_ = computePose(time_);
    window.setCamFromWorld(world_from_camera_);
}

//
// #####################################################################################################################
//

json TrajectoryCamera::serialize()
{
    return {{"type", "trajectory"},
            {"from_trajectory", trajectory_nodes_},
            {"motion_mode", motion_mode_},
            {"speed", speed_},
            {"start_time", start_time_},
            {"polynomial_degree", polynomial_degree_}};
}

//
// #####################################################################################################################
//

TrajectoryNode TrajectoryCamera::evaluateTrajectory(const float time) const
{
    const auto to = trajectory_.to.evaluate(time);
    const auto from = trajectory_.from.evaluate(time);
    const auto up_hint = trajectory_.up_hint.evaluate(time);

    return {
        .time = time,
        .to = Vec3Map(to.data(), to.size()),
        .from = Vec3Map(from.data(), from.size()),
        .up_hint = Vec3Map(up_hint.data(), up_hint.size()),
    };
}

//
// #####################################################################################################################
//

Transform3 TrajectoryCamera::computePose(float time) const
{
    //
    // Handle motion mode via the trajectory time
    //

    if (motion_mode_ == "SINGLE")
    {
        time = std::clamp(time, time_range_.first, time_range_.second);
    }
    else if (motion_mode_ == "LOOP")
    {
        time = wrapToRange(/* value */ time, /* range */ time_range_);
    }
    else if (motion_mode_ == "OSCILLATE")
    {
        // This mode takes 2 * time_span to complete
        const std::pair<float, float> reverse_range{time_range_.second, time_range_.second + time_span_};

        time = wrapToRange(/* value */ time, /* range */ {time_range_.first, reverse_range.second});
        if (inRange(time, reverse_range))
        {
            time = time_range_.second - wrapToRange(/* value */ time, /* range */ time_range_);
        }
    }
    else
    {
        throw std::runtime_error("[TrajectoryCamera::computePose()] Bad motion mode: " + motion_mode_);
    }

    //
    // Compute transformation at current trajectory time
    //

    const auto node = evaluateTrajectory(/* time */ time);
    const auto world_from_camera =
        makeLookAtTransform(/* look_at */ node.to, /* look_from */ node.from, /* up_hint */ node.up_hint);

    return world_from_camera;
}

//
// #####################################################################################################################
//

}