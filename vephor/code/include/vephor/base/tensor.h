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