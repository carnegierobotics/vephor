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

#include "math.h"

namespace vephor
{

template <typename T> bool find(const std::vector<T>& vec, const T& item)
{
    return std::find(vec.begin(), vec.end(), item) != vec.end();
}

template <typename T, typename S, typename H> bool find(const std::unordered_map<T, S, H>& map, const T& item)
{
    return map.find(item) != map.end();
}

template <typename T, typename S, typename H> bool find(const std::unordered_set<T, S, H>& set, const T& item)
{
    return set.find(item) != set.end();
}

template <typename T> void extend(std::vector<T>& to, const std::vector<T>& from)
{
    to.insert(to.end(), from.begin(), from.end());
}

template <typename T> void remove_value(std::vector<T>& vec, const T& value)
{
    vec.erase(std::remove(vec.begin(), vec.end(), value), vec.end());
}

template <class T, size_t D>
std::ostream& operator<<(std::ostream& out, const std::array<T,D>& index)
{
	out << "(";
	for (size_t d = 0; d < D; d++)
	{
		out << index[d];
		if (d < D - 1)
		{
			out << ",";
		}
	}
	out << ")";
	return out;
}

template <class T, size_t D>
std::array<T,D> operator+(const std::array<T,D>& l, const std::array<T,D>& r)
{
	std::array<T,D> out = l;
	for (size_t i = 0; i < D; i++)
	{
		out[i] += r[i];
	}
	return out;
}

template <class T, size_t D>
std::array<T,D> operator-(const std::array<T,D>& l, const std::array<T,D>& r)
{
	std::array<T,D> out = l;
	for (size_t i = 0; i < D; i++)
	{
		out[i] -= r[i];
	}
	return out;
}

namespace detail {
    struct print
    {
        bool space;
        print() : space(false)
        {
        }
        ~print()
        {
            std::cout << std::endl;
        }

        template <typename T> print& operator,(const T& t)
        {
            if (space)
                std::cout << ' ';
            else
                space = true;
            std::cout << t;
            return *this;
        }
    };
} // namespace detail

}

#define v4print vephor::detail::print(),