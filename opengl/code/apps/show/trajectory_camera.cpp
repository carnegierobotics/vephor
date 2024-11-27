//
// Copyright 2024
// Carnegie Robotics, LLC
// 4501 Hatfield Street, Pittsburgh, PA 15201
// https://www.carnegierobotics.com/
//
// This code is provided under the terms of the Master Services Agreement (the Agreement).
// This code constitutes CRL Background Intellectual Property, as defined in the Agreement.
//
// Significant history (date, user, job code, action):
//   2024-11-26, emusser@carnegierobotics.com, 2045.01, Created file.
//

#include "trajectory_camera.h"

#include "vephor.h"

#include <Eigen/Dense>

#include <cmath>
#include <iterator>
#include <sstream>
#include <stdexcept>
#include <utility>
#include <vector>

//
// #####################################################################################################################
//

std::optional<Polynomial<float>> polynomialCurveFit1D(const VecX &t, const VecX &x, const int degree)
{
    int numPoints = t.size();

    if (x.size() != numPoints || numPoints == 0 || degree < 0)
    {
        std::cerr << "Invalid input." << std::endl;
        return std::nullopt;
    }

    MatX X(numPoints, degree + 1);
    for (int i = 0; i < numPoints; ++i)
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
        std::cerr << "Failed to solve for coefficients of the polynomial curve fit." << std::endl;
        return std::nullopt;
    }

    return Polynomial<float>{coefficients};
}

//
// #####################################################################################################################
//

std::optional<PolynomialND<float>> polynomialCurveFitND(const std::vector<float> &t_vector,
                                                        const std::vector<VecX> &x_vector,
                                                        const int degree)
{
    const int dimensions = x_vector.front().size();

    if (t_vector.empty() || x_vector.empty())
    {
        throw std::runtime_error("[polynomialCurveFitND()] Data (t, x) cannot be empty.");
    }
    if (t_vector.size() != x_vector.size())
    {
        throw std::runtime_error("[polynomialCurveFitND()] Input dimensions don't match.");
    }
    if (degree <= 0)
    {
        throw std::runtime_error("[polynomialCurveFitND()] Polynomial degree must be positive.");
    }
    if (dimensions < 1)
    {
        throw std::runtime_error("[polynomialCurveFitND()] Data dimension must be at least 1.");
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

        const auto coefficients = polynomialCurveFit1D(/* t */ t, /* x */ x, /* degree */ degree);
        if (!coefficients.has_value())
        {
            return std::nullopt;
        }

        polynomials_by_dimension.emplace_back(coefficients.value());
    }

    return PolynomialND<float>(polynomials_by_dimension);
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
    else if (value > range.second)
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

    const auto to_polynomial =
        polynomialCurveFitND(/* times */ times, /* points */ to_vectors, /* degree */ polynomial_degree_);
    if (!to_polynomial.has_value())
    {
        throw std::runtime_error("[TrajectoryCamera::setup()] Could not fit polynomial to the trajectory to vectors.");
    }

    const auto from_polynomial =
        polynomialCurveFitND(/* times */ times, /* points */ from_vectors, /* degree */ polynomial_degree_);
    if (!from_polynomial.has_value())
    {
        throw std::runtime_error(
            "[TrajectoryCamera::setup()] Could not fit polynomial to the trajectory from vectors.");
    }

    const auto up_hint_polynomial =
        polynomialCurveFitND(/* times */ times, /* points */ up_hint_vectors, /* degree */ polynomial_degree_);
    if (!up_hint_polynomial.has_value())
    {
        throw std::runtime_error(
            "[TrajectoryCamera::setup()] Could not fit polynomial to the trajectory up hint vectors.");
    }

    trajectory_ = {.to = to_polynomial.value(), .from = from_polynomial.value(), .up_hint = up_hint_polynomial.value()};

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
    //
    // TODO: Integrate speed.
    //

    time_ += dt;
    time_ = wrapToRange(/* value */ time_, /* range */ time_range_);

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
    time = wrapToRange(/* value */ time, /* range */ time_range_);
    const auto node = evaluateTrajectory(/* time */ time);
    const auto world_from_camera =
        makeLookAtTransform(/* look_at */ node.to, /* look_from */ node.from, /* up_hint */ node.up_hint);

    //
    // TODO: Add handling for the various modes, currently this will just loop.
    //

    return world_from_camera;
}

//
// #####################################################################################################################
//
