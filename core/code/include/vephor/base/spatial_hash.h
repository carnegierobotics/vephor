/**
 * Copyright 2025
 * Carnegie Robotics, LLC
 * 4501 Hatfield Street, Pittsburgh, PA 15201
 * https://www.carnegierobotics.com
 *
 * This source code is licensed under the Apache License, Version 2.0
 * found in the LICENSE file in the root directory of this source tree.
**/

#pragma once

#include "common.h"

namespace vephor
{

template <int D> using VecD = Eigen::Matrix<float, D, 1>;
template <int D> using VecDi = Eigen::Matrix<int, D, 1>;

}

namespace std {
    
template <int D>
struct hash<vephor::VecDi<D>>
{
    std::size_t operator()(const vephor::VecDi<D>& v) const
    {
        using std::size_t;
        using std::hash;
        using std::string;

        // Compute individual hash values for first,
        // second and third and combine them using XOR
        // and bit shifting:

        std::size_t result = hash<int>{}(v[0]);

        for (int i = 1; i < D; i++)
        {
            result = (result << 1) ^ v[i];
        }

        return result;
}
};

}

namespace vephor
{

template <typename T, int D>
class SpatialHash
{
public:
    SpatialHash(float p_res):res(p_res){}
    VecDi<D> add(const VecD<D>& pos, const T& data)
    {
        VecDi<D> index = (pos/res).template cast<int>();
        hash[index].push_back(data);
        return index;
    }
    VecDi<D> addAndFanOut(const VecD<D>& pos, const T& data)
    {
        VecDi<D> index = (pos/res).template cast<int>();

        VecDi<D> offset;
        offset.fill(-1);

        while (true)
        {
            VecDi<D> curr_index = index + offset;
            hash[curr_index].push_back(data);

            int curr_d = 0;
            while (curr_d < D)
            {
                offset[curr_d]++;
                if (offset[curr_d] <= 1)
                    break;
                offset[curr_d] = -1;
                curr_d++;
            }
            if (curr_d == D)
                break;
        }

        return index;
    }
    void removeFanOut(const VecDi<D>& index, const T& data)
    {
        VecDi<D> offset;
        offset.fill(-1);

        while (true)
        {
            VecDi<D> curr_index = index + offset;
            auto& cell = hash[curr_index];
            cell.erase(std::find(cell.begin(), cell.end(), data));

            int curr_d = 0;
            while (curr_d < D)
            {
                offset[curr_d]++;
                if (offset[curr_d] <= 1)
                    break;
                offset[curr_d] = -1;
                curr_d++;
            }
            if (curr_d == D)
                break;
        }
    }
    const vector<T>& lookup(const VecD<D>& pos)
    {
        VecDi<D> index = (pos/res).template cast<int>();
        return hash[index];
    }
	vector<const vector<T>*> lookupForRadius(const VecD<D>& pos, float radius)
	{
		VecD<D> min_corner = pos;
		VecD<D> max_corner = pos;
		
		for (int i = 0; i < D; i++)
		{
			min_corner[i] -= radius;
			max_corner[i] += radius;
		}
		
		VecDi<D> min_index = (min_corner/res).template cast<int>();
		VecDi<D> max_index = (max_corner/res).template cast<int>();
		
		vector<const vector<T>*> lists;
		
		VecDi<D> curr_index = min_index;
		while (true)
		{
			lists.push_back(&hash[curr_index]);
			
			bool success = false;
			for (int i = 0; i < D; i++)
			{
				curr_index[i]++;
				if (curr_index[i] < max_index[i])
				{
					success = true;
					break;
				}
				curr_index[i] = min_index[i];
			}
			if (!success)
				break;
		}
		
		return lists;
	}
    VecDi<D> getCell(const VecD<D>& pos)
    {
        return (pos/res).template cast<int>();
    }
    const std::unordered_map<VecDi<D>, vector<T>>& getHash() const{return hash;}
    void clear() {hash.clear();}
private:
    float res;
    std::unordered_map<VecDi<D>, vector<T>> hash;
};

}

