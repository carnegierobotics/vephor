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

#pragma once

#include <ostream>
#include <sstream>
#include <string>
#include <system_error>
#include <utility>
#include <vector>

#include "camera_manager.h"
#include "../draw/background.h"

namespace vephor
{

///
/// Scalar polynomial.
///
template <typename T = float>
class Polynomial
{
public:
    ///
    /// Construct a trivial Polynomial.
    ///
    Polynomial() : Polynomial(Vec<Eigen::Dynamic, T>{0})
    {
    }

    ///
    /// Construct a Polynomial.
    ///
    /// @param[in] coefficients Coefficients in order of ascending degree (i.e., @f$ c_0 + c_1 x + c_2 x^2 + \ldots
    ///            @f$).
    ///
    explicit Polynomial(Vec<Eigen::Dynamic, T> coefficients)
    {
        setCoefficients(coefficients);
    }

    ///
    /// Evaluate this polynomial at a specific point.  Evaluation is done using Horner's method [1, 2].
    ///
    /// [1] https://en.wikipedia.org/wiki/Horner%27s_method
    /// [2] https://stackoverflow.com/a/72100357
    ///
    [[nodiscard]] T evaluate(const T t) const
    {
        const auto reverse_coefficients = coefficients_.reverse();

        float x = reverse_coefficients(0);
        for (int i = 1; i < reverse_coefficients.size(); ++i)
        {
            x = reverse_coefficients(i) + t * x;
        }

        return x;
    }

    ///
    /// Evaluate this polynomial at a specific point.
    ///
    [[nodiscard]] T operator()(const T t) const
    {
        return evaluate(t);
    }

    ///
    /// Set the polynomial coefficients.
    ///
    void setCoefficients(Vec<Eigen::Dynamic, T> coefficients)
    {
        coefficients_ = std::move(coefficients);

        // Assume a constant, zero-valued polynomial if no coefficients are given.
        if (coefficients_.size() == 0)
        {
            coefficients_ = Vec<Eigen::Dynamic, T>{0};
        }
    }

    ///
    /// Access the polynomial coefficients sorted in ascending degree order.
    ///
    [[nodiscard]] Vec<Eigen::Dynamic, T> coefficients() const
    {
        return coefficients_;
    }

    ///
    /// Access the polynomial degree.
    ///
    [[nodiscard]] int degree() const
    {
        return coefficients_.size() - 1;
    }

    ///
    /// Get a string representation of the polynomial.
    ///
    [[nodiscard]] std::string string() const
    {
        std::stringstream ss;
        for (int i = degree(); i >= 0; --i)
        {
            ss << coefficients_[i];
            if (i > 0)
            {
                ss << " t";
                if (i > 1)
                {
                    ss << "^" << i << " + ";
                }
                ss << " + ";
            }
        }

        return ss.str();
    }

private:
    Vec<Eigen::Dynamic, T> coefficients_; ///< Polynomial coefficients in ascending degree order.
};

///
/// Output stream insertion operator for Polynomial.
///
template <typename T>
std::ostream &operator<<(std::ostream &out, const Polynomial<T> &polynomial)
{
    return out << polynomial.string();
}

///
/// N-dimensional polynomial.
///
template <typename T>
class PolynomialND
{
public:
    ///
    /// Construct a trivial PolynomialND.
    ///
    PolynomialND() : PolynomialND(std::vector<Polynomial<T>>{Polynomial<float>{Vec<Eigen::Dynamic, T>{0}}})
    {
    }

    ///
    /// Construct a PolynomialND.
    ///
    explicit PolynomialND(const std::vector<Polynomial<T>> &polynomials)
    {
        setPolynomials(polynomials);
    }

    ///
    /// Evaluate this polynomial at a specific point.
    ///
    [[nodiscard]] Vec<Eigen::Dynamic, T> evaluate(const T &t) const
    {
        Vec<Eigen::Dynamic, T> x(dimension());
        for (int i = 0; i < dimension(); ++i)
        {
            x(i) = polynomials_[i].evaluate(t);
        }

        return x;
    }

    ///
    /// Evaluate this polynomial at a specific point.
    ///
    [[nodiscard]] Vec<Eigen::Dynamic, T> operator()(const T &t) const
    {
        return evaluate(t);
    }

    ///
    /// Set the component polynomials.
    ///
    void setPolynomials(const std::vector<Polynomial<T>> &polynomials)
    {
        polynomials_ = polynomials;

        // If no polynomials are provided, fill with a 1-D, zero-valued polynomial of degree 1.
        if (polynomials.empty())
        {
            polynomials_ = std::vector<Polynomial<T>>{{0}};
        }

        // Compute the max polynomial degree
        degree_ = 0;
        for (const auto &polynomial : polynomials_)
        {
            degree_ = std::max(degree_, polynomial.degree());
        }
    }

    ///
    /// Access the polynomial coefficients sorted in ascending degree order.
    ///
    [[nodiscard]] std::vector<Polynomial<T>> polynomials() const
    {
        return polynomials_;
    }

    ///
    /// Access the polynomial degree.
    ///
    [[nodiscard]] int dimension() const
    {
        return polynomials_.size();
    }

    ///
    /// Access the polynomial degree.
    ///
    [[nodiscard]] int degree() const
    {
        return degree_;
    }

    ///
    /// Get a string representation of the polynomial.
    ///
    [[nodiscard]] std::string string() const
    {
        std::stringstream ss;
        ss << "[ ";
        bool is_first{true};
        for (const auto &polynomial : polynomials_)
        {
            if (!is_first)
            {
                ss << ", ";
            }
            ss << polynomial.string();

            is_first = false;
        }
        ss << " ]";

        return ss.str();
    }

private:
    std::vector<Polynomial<T>> polynomials_; ///< Component 1-D polynomials.
    int degree_;                             ///< Degree of the contained polynomial.
};

///
/// Output stream insertion operator for PolynomialND.
///
template <typename T>
std::ostream &operator<<(std::ostream &out, const PolynomialND<T> &polynomial)
{
    return out << polynomial.string();
}

///
/// Error code for a polynomial curve fit.
///
enum class PolynomialCurveFitError
{
    SUCCESS = 0,                    ///< Successful fit.
    EMPTY_DATA,                     ///< Provided data was empty.
    MISMATCHED_DATA,                ///< Provided data dimensions were not consistent.
    NON_POSITIVE_POLYNOMIAL_DEGREE, ///< Requested polynomial degree for fit was invalid.
    NULL_DIMENSION,                 ///< Provided data has null dimension.
    LEAST_SQUARES_FAILURE,          ///< Least squares solver failed.
};

///
/// Error code category for a polynomial curve fit.
///
class PolynomialCurveFitErrorCategory : public std::error_category
{
public:
    ///
    /// Get the category name.
    ///
    [[nodiscard]] const char *name() const noexcept override
    {
        return "PolynomialCurveFitError";
    }

    ///
    /// Translate error code to an informative message.
    ///
    [[nodiscard]] std::string message(int ev) const override
    {
        switch (static_cast<PolynomialCurveFitError>(ev))
        {
        case PolynomialCurveFitError::EMPTY_DATA:
            return "Empty data.";
        case PolynomialCurveFitError::MISMATCHED_DATA:
            return "Inconsistently sized data.";
        case PolynomialCurveFitError::NON_POSITIVE_POLYNOMIAL_DEGREE:
            return "Polynomial does not have a positive degree.";
        case PolynomialCurveFitError::NULL_DIMENSION:
            return "Data has no dimension.";
        case PolynomialCurveFitError::LEAST_SQUARES_FAILURE:
            return "Least squares solve was unsuccessful.";
        default:
            return "Unknown error.";
        }
    }
};

///
/// Fit a polynomial @f$ x(t) @f$ to a set of 1-D data.
///
Polynomial<float> polynomialCurveFit1D(const VecX &t, const VecX &x, int degree, std::error_code &error_code);

///
/// Fit a polynomial @f$ \boldsymbol{x}(t) @f$ to a set of N-D data.
///
PolynomialND<float> polynomialCurveFitND(const std::vector<float> &t,
                                         const std::vector<VecX> &x,
                                         int degree,
                                         std::error_code &error_code);

///
/// Check if a value falls in a specified range.
///
bool inRange(float value, const std::pair<float, float> &range);

///
/// Wrap a value so that it remains within a specified range.
///
float wrapToRange(float value, const std::pair<float, float> &range);

///
/// Node in a camera trajectory.
///
struct TrajectoryNode
{
    float time;   ///< Trajectory time, in seconds.
    Vec3 to;      ///< Camera target point in the world frame.
    Vec3 from;    ///< Camera location in the world frame.
    Vec3 up_hint; ///< World frame vector indicating the upward direction.
};

///
/// Serialize a TrajectoryNode to json.  Compatible with the nlohmann/json serializer [1].
///
/// [1] https://json.nlohmann.me/features/arbitrary_types/
///
inline void to_json(nlohmann::json &j, const TrajectoryNode &trajectory_node)
{
    j = nlohmann::json{{"time", trajectory_node.time},
                       {"to", toJson(trajectory_node.to)},
                       {"from", toJson(trajectory_node.from)},
                       {"up_hint", toJson(trajectory_node.up_hint)}};
}

///
/// Deserialize a TrajectoryNode from json.  Compatible with the nlohmann/json serializer [1].
///
/// [1] https://json.nlohmann.me/features/arbitrary_types/
///
inline void from_json(const nlohmann::json &j, TrajectoryNode &trajectory_node)
{
    trajectory_node.time = j.at("time");
    trajectory_node.to = readVec3(j.at("to"));
    trajectory_node.from = readVec3(j.at("from"));
    trajectory_node.up_hint = readVec3(j.at("up_hint"));
}

///
/// Time-smoothed trajectory.
///
struct SmoothedTrajectory
{
    PolynomialND<float> to;      ///< Time-parameterized function producing the camera target point in the world frame.
    PolynomialND<float> from;    ///< Time-parameterized function producing the camera location in the world frame.
    PolynomialND<float> up_hint; ///< Time-parameterized function producing a world frame up-direction vector.
};

///
/// Camera that follows a specified, spline-interpolated trajectory.
///
class TrajectoryCamera : public CameraManager
{
public:
    ///
    /// Setup the camera.
    ///
    void setup(const json &data, Window &window, AssetManager &assets) override;

    ///
    /// Update the camera with a new time step.
    ///
    void update(Window &window, float dt, const ControlInfo &control_info) override;

    ///
    /// Serialize contained data to JSON.
    ///
    json serialize() override;

private:
    std::vector<TrajectoryNode> trajectory_nodes_; ///< Discrete nodes to which the trajectory spline is fit.
    std::string motion_mode_;                      ///< Trajectory tracking mode.
    float speed_;                                  //< Speed multiplier for trajectory tracking.
    float start_time_;                             ///< Trajectory start time.
    int polynomial_degree_;                        ///< Degree of the polynomial spline fit.

    SmoothedTrajectory trajectory_;      ///< Trajectory for the camera to follow.
    std::pair<float, float> time_range_; ///< Range of times for the contained trajectory.
    float time_span_;                    ///< Time span for the contained trajectory.

    float time_; ///< Current trajectory time.  This value gets wrapped to the time range, where appropriate.
    Transform3 world_from_camera_; ///< Current camera pose in the world frame.

    ///
    /// Evaluate the trajectory at a specific time.
    ///
    [[nodiscard]] TrajectoryNode evaluateTrajectory(float time) const;

    ///
    /// Compute the trajectory pose at a specific time.
    ///
    [[nodiscard]] Transform3 computePose(float time) const;
};

}