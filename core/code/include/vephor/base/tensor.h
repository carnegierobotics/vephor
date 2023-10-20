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

#include "common.h"

namespace vephor
{

template <int D>
using TensorIndex = Eigen::Matrix<int32_t, D, 1>;

template <int D>
class TensorIter
{
public:
	TensorIter(const TensorIndex<D>& p_sizes)
	: sizes(p_sizes)
	{
		n_elem = 1;
		for (int d = 0; d < D; d++)
		{
			n_elem *= sizes[d];
			curr_index[d] = 0;
		}
	}
	void operator++(int)
	{
		inner_index += 1;
		if (inner_index >= n_elem)
			inner_index = n_elem;
		else
		{
			int dim = D - 1;
			curr_index[dim]++;
			
			while (dim > 0 && curr_index[dim] == sizes[dim])
			{
				curr_index[dim] = 0;
				dim--;
				curr_index[dim]++;
			}
		}
	}
	bool atEnd() const
	{
		return inner_index == n_elem;
	}
	TensorIndex<D> getIndex() const
	{
		return curr_index;
	}
private:
	TensorIndex<D> sizes;
	int n_elem;
	int inner_index = 0;
	TensorIndex<D> curr_index;
};

template <int D, typename T>
class Tensor
{
public:
	Tensor();
	Tensor(const TensorIndex<D>& sizes);
	void fill(const T& item);
	TensorIndex<D> size() const {return sizes;}
	T& operator ()(const TensorIndex<D>& ind);
	const T& operator ()(const TensorIndex<D>& ind) const;
	TensorIter<D> getIter() const
	{
		return TensorIter<D>(sizes);
	}
	const vector<T>& getData() const {return data;}
	vector<T>& getData() {return data;}
	int getTypeSize() const {return sizeof(D);}
	T min() const {return *std::min_element(data.begin(), data.end());}
	T max() const {return *std::max_element(data.begin(), data.end());}
private:
	TensorIndex<D> sizes;
	vector<T> data;
};

template <int D, typename T>
Tensor<D,T>::Tensor()
: sizes(TensorIndex<D>::Zero())
{
	data.resize(0);
}

template <int D, typename T>
Tensor<D,T>::Tensor(const TensorIndex<D>& p_sizes)
: sizes(p_sizes)
{
	int n_elem = 1;
	
	for (int d = 0; d < D; d++)
		n_elem *= sizes[d];
	
	data.resize(n_elem);
}

template <int D, typename T>
void Tensor<D,T>::fill(const T& item)
{
	for (size_t i = 0; i < data.size(); i++)
		data[i] = item;
}

template <int D, typename T>
T& Tensor<D,T>::operator ()(const TensorIndex<D>& ind)
{
	int inner_index = ind[0];
	
	for (int d = 1; d < D; d++)
	{	
		inner_index *= sizes[d];
		inner_index += ind[d];
	}
	
	return data[inner_index];
}

template <int D, typename T>
const T& Tensor<D,T>::operator ()(const TensorIndex<D>& ind) const
{
	int inner_index = ind[0];
	
	for (int d = 1; d < D; d++)
	{	
		inner_index *= sizes[d];
		inner_index += ind[d];
	}
	
	return data[inner_index];
}
	
}