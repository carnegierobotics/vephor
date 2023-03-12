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