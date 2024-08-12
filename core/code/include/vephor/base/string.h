//
// Copyright 2024
// Carnegie Robotics, LLC
// 4501 Hatfield Street, Pittsburgh, PA 15201
// https://www.carnegierobotics.com
//
// This code is provided under the terms of the Master Services Agreement (the Agreement).
// This code constitutes CRL Background Intellectual Property, as defined in the Agreement.
//

#pragma once

#include <algorithm>
#include <iterator>
#include <string>
#include <string_view>

namespace vephor
{

inline std::string to_lower(std::string_view string)
{
    std::string out;
    std::transform(
            string.cbegin(), string.cend(), std::back_inserter(out),
            [](const unsigned char c) { return std::tolower(c); });

    return out;
}

inline std::string to_upper(std::string_view string)
{
    std::string out;
    std::transform(
            string.cbegin(), string.cend(), std::back_inserter(out),
            [](const unsigned char c) { return std::toupper(c); });

    return out;
}

} // vephor
