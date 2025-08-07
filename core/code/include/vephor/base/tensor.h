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
#include "json.h"
#include <typeinfo>

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
	bool in(const TensorIndex<D>& ind) const;
	TensorIter<D> getIter() const
	{
		return TensorIter<D>(sizes);
	}
	const Eigen::Matrix<T, Eigen::Dynamic, 1>& getData() const {return data;}
	Eigen::Matrix<T, Eigen::Dynamic, 1>& getData() {return data;}
	int getTypeSize() const {return sizeof(D);}
	T min() const {return data.minCoeff();}
	T max() const {return data.maxCoeff();}
	void operator +=(const Tensor<D,T>& t)
	{
		if (t.data.size() != data.size())
			std::runtime_error("Can't add unequal tensor sizes.");
		data += t.data;
	}
	void operator *=(const T& m)
	{
		data *= m;
	}
	template <typename S>
	Tensor<D,S> cast(T mult = 1.0f, T bias = 1.0f)
	{
		Tensor<D,S> cast_t(sizes);
		cast_t.setData(((data * mult).array() + bias).template cast<S>());
		return cast_t;
	}
	void saveRaw(const string& path)
	{
		std::ofstream out(path+".bin", std::ios::binary);
    	out.write(reinterpret_cast<const char*>(data.data()), data.size() * sizeof(T));

		json size_info;
		for (int i = 0; i < D; i++)
			size_info.push_back(sizes[i]);

		json info;
		info["size"] = size_info;
		info["type_size"] = sizeof(T);
		info["type"] = typeid(T).name();

		std::ofstream size_out(path+".json");
		size_out << info;
	}
	void setData(const Eigen::Matrix<T, Eigen::Dynamic, 1>& p_data)
	{
		data = p_data;
	}
private:
	TensorIndex<D> sizes;
	Eigen::Matrix<T, Eigen::Dynamic, 1> data;
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

template <int D, typename T>
bool Tensor<D,T>::in(const TensorIndex<D>& ind) const
{
	for (int d = 0; d < D; d++)
	{	
		if (ind[d] < 0)
			return false;
		if (ind[d] >= sizes[d])
			return false;
	}
	
	return true;
}
	
}