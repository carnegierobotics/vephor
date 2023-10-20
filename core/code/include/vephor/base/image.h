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

#include "tensor.h"

#define STB_IMAGE_IMPLEMENTATION
#include "thirdparty/nothings/stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "thirdparty/nothings/stb_image_write.h"

namespace vephor
{

template <typename T>
class Image
{
public:
	using Vec = Eigen::Matrix<T, Eigen::Dynamic, 1>;

	Image(){}
	Image(int p_width, int p_height, int p_channels)
	: channels(p_channels), data({p_height, p_width, p_channels})
	{
		data.fill(0);
	}
	Eigen::Ref<Vec> operator ()(int x, int y)
	{
		T* ptr = &(data({y, x, 0}));
		return Eigen::Map<Vec>(ptr, channels);
	}
	Eigen::Ref<Vec> operator ()(const Vec2i& ind)
	{
		T* ptr = &(data({ind[1], ind[0], 0}));
		return Eigen::Map<Vec>(ptr, channels);
	}
	Eigen::Ref<const Vec> operator ()(int x, int y) const
	{
		const T* ptr = &(data({y, x, 0}));
		return Eigen::Map<const Vec>(ptr, channels);
	}
	Eigen::Ref<const Vec> operator ()(const Vec2i& ind) const
	{
		const T* ptr = &(data({ind[1], ind[0], 0}));
		return Eigen::Map<const Vec>(ptr, channels);
	}
	Vec2i getSize() const {return Vec2i(data.size()[1], data.size()[0]);}
	int getChannels() const {return channels;}
	Image<T> changeChannels(int new_channels) const
	{
		Image<T> img(getSize()[0], getSize()[1], new_channels);

		if (new_channels == 1)
		{
			// Average existing channels
			auto iter = getIter();
			while (!iter.atEnd())
			{
				img(iter.getIndex())[0] = (*this)(iter.getIndex()).mean();
				iter++;
			}
		}
		else if (channels == 1)
		{
			// Copy to all new channels
			auto iter = getIter();
			while (!iter.atEnd())
			{
				T val = (*this)(iter.getIndex())[0];
				Vec new_v(new_channels);
				new_v.fill(val);
				img(iter.getIndex()) = new_v;
				iter++;
			}
		}

		return img;
	}
	template <typename S>
	Image<S> cast(T mult = 1.0f, T bias = 1.0f)
	{
		Image<S> img(getSize()[0], getSize()[1], channels);

		auto iter = img.getIter();

		Vec bias_v(channels);
		for (int c = 0; c < channels; c++)
		{
			bias_v[c] = bias;
		}
		
		while(!iter.atEnd())
		{
			const auto ind = iter.getIndex();
			img(ind) = ((*this)(ind) * mult + bias_v).template cast<S>();
			iter++;
		}

		return img;
	}
	T min() const
	{
		return data.min();
	}
	T max() const
	{
		return data.max();
	}
	const vector<T>& getData() const {return data.getData();}
	int getTypeSize() const {return sizeof(T);}
	void getBuffer(const char*& buf_data, int& buf_size) const
	{
		buf_data = reinterpret_cast<const char*>(data.getData().data());
		buf_size = getData().size() * sizeof(T);
	}
	void copyFromBuffer(const char* buf_data, int buf_size)
	{
		data.getData().assign(reinterpret_cast<const T*>(buf_data), reinterpret_cast<const T*>(buf_data+buf_size));
	}
	TensorIter<2> getIter() const
	{
		return TensorIter<2>(getSize());
	}
	void flipYInPlace()
	{
		if (data.size()[0] < 2)
			return;

		T* test_row_start = &data({0,0,0});
		T* test_row_end = &data({1,0,0});

		int row_size = test_row_end - test_row_start;
		int row_elements = row_size / sizeof(T);
		vector<T> row_holder(row_size);

		for (int row = 0; row < data.size()[0] / 2; row++)
		{
			T* first_row_start = &data({row,0,0});
			T* first_row_end = &data({row+1,0,0});
			T* last_row_start = &data({data.size()[0]-1-row,0,0});
			T* last_row_end = &data({data.size()[0]-row,0,0});

			memcpy(row_holder.data(), first_row_start, row_size);
			memcpy(first_row_start, last_row_start, row_size);
			memcpy(last_row_start, row_holder.data(), row_size);
		}
	}
private:
	int channels;
	Tensor<3, T> data;
};


inline shared_ptr<Image<uint8_t>> generateSimpleImage(const Vec2i& size, const Vec3& rgb)
{
	auto image = make_shared<Image<uint8_t>>(size[0], size[1], 3);

    Vec3u rgb_u;
    rgb_u[0] = rgb[0] * 255.0;
    rgb_u[1] = rgb[1] * 255.0;
    rgb_u[2] = rgb[2] * 255.0;

	for (int i = 0; i < size[0]; i++)
	{
		for (int j = 0; j < size[1]; j++)
		{
			(*image)(i,j) = rgb_u;
		}
	}
    
    return image;
}

inline shared_ptr<Image<uint8_t>> generateSimpleImage(const Vec2i& size, const Vec4& rgba)
{
	auto image = make_shared<Image<uint8_t>>(size[0], size[1], 4);

    Vec4u rgba_u;
    rgba_u[0] = rgba[0] * 255.0;
    rgba_u[1] = rgba[1] * 255.0;
    rgba_u[2] = rgba[2] * 255.0;
	rgba_u[3] = rgba[3] * 255.0;

	for (int i = 0; i < size[0]; i++)
	{
		for (int j = 0; j < size[1]; j++)
		{
			(*image)(i,j) = rgba_u;
		}
	}
    
    return image;
}

inline shared_ptr<Image<uint8_t>> generateGradientImage(const Vec2i& size, const Vec3& top_rgb, const Vec3& bottom_rgb)
{
	auto image = make_shared<Image<uint8_t>>(size[0], size[1], 3);
    
    for (int i = 0; i < size[0]; i++)
	{
		for (int j = 0; j < size[1]; j++)
		{
			float perc = j / (float)(size[1] - 1);
			Vec3 curr_rgb = top_rgb + perc * (bottom_rgb - top_rgb);

			Vec3u rgb_u;
			rgb_u[0] = curr_rgb[0] * 255.0;
			rgb_u[1] = curr_rgb[1] * 255.0;
			rgb_u[2] = curr_rgb[2] * 255.0;

			(*image)(i,j) = rgb_u;
		}
    }

	return image;
}

inline shared_ptr<Image<uint8_t>> generateGradientImage(const Vec2i& size, const Vec4& top_rgba, const Vec4& bottom_rgba)
{
	auto image = make_shared<Image<uint8_t>>(size[0], size[1], 4);
    
    for (int i = 0; i < size[0]; i++)
	{
		for (int j = 0; j < size[1]; j++)
		{
			float perc = j / (float)(size[1] - 1);
			Vec4 curr_rgba = top_rgba + perc * (bottom_rgba - top_rgba);

			Vec4u rgba_u;
			rgba_u[0] = curr_rgba[0] * 255.0;
			rgba_u[1] = curr_rgba[1] * 255.0;
			rgba_u[2] = curr_rgba[2] * 255.0;
			rgba_u[3] = curr_rgba[3] * 255.0;

			(*image)(i,j) = rgba_u;
		}
    }

	return image;
}

inline shared_ptr<Image<uint8_t>> generateCheckerboardImage(const Vec2i& size, const Vec2i& n_cells, const Vec3& rgb_1, const Vec3& rgb_2)
{
	auto image = make_shared<Image<uint8_t>>(size[0], size[1], 3);

    Vec3u rgb_1_u;
    rgb_1_u[0] = rgb_1[0] * 255.0;
    rgb_1_u[1] = rgb_1[1] * 255.0;
    rgb_1_u[2] = rgb_1[2] * 255.0;
	
	Vec3u rgb_2_u;
    rgb_2_u[0] = rgb_2[0] * 255.0;
    rgb_2_u[1] = rgb_2[1] * 255.0;
    rgb_2_u[2] = rgb_2[2] * 255.0;

	for (int i = 0; i < size[0]; i++)
	{
		for (int j = 0; j < size[1]; j++)
		{
			int c_cell_index = ((float)i / size[0]) * n_cells[0];
			int r_cell_index = ((float)j / size[1]) * n_cells[1];

			if ((c_cell_index + r_cell_index) % 2 == 0)
			{
				(*image)(i,j) = rgb_1_u;
			}
			else
			{
				(*image)(i,j) = rgb_2_u;
			}
		}
	}
    
    return image;
}

inline shared_ptr<Image<uint8_t>> generateCheckerboardImage(const Vec2i& size, const Vec2i& n_cells, const Vec4& rgba_1, const Vec4& rgba_2)
{
	auto image = make_shared<Image<uint8_t>>(size[0], size[1], 4);

    Vec4u rgba_1_u;
    rgba_1_u[0] = rgba_1[0] * 255.0;
    rgba_1_u[1] = rgba_1[1] * 255.0;
    rgba_1_u[2] = rgba_1[2] * 255.0;
	rgba_1_u[3] = rgba_1[3] * 255.0;
	
	Vec4u rgba_2_u;
    rgba_2_u[0] = rgba_2[0] * 255.0;
    rgba_2_u[1] = rgba_2[1] * 255.0;
    rgba_2_u[2] = rgba_2[2] * 255.0;
	rgba_2_u[3] = rgba_2[3] * 255.0;

	for (int i = 0; i < size[0]; i++)
	{
		for (int j = 0; j < size[1]; j++)
		{
			int c_cell_index = ((float)i / size[0]) * n_cells[0];
			int r_cell_index = ((float)j / size[1]) * n_cells[1];

			if ((c_cell_index + r_cell_index) % 2 == 0)
			{
				(*image)(i,j) = rgba_1_u;
			}
			else
			{
				(*image)(i,j) = rgba_2_u;
			}
		}
	}
    
    return image;
}

inline shared_ptr<Image<uint8_t>> loadImage(const string& file)
{
	int width, height, comps;
	const uint8_t* img = stbi_load(file.c_str(), &width, &height, &comps, 0);
	
	auto image = make_shared<Image<uint8_t>>(width, height, comps);
	image->copyFromBuffer(reinterpret_cast<const char*>(img), width * height * comps);
	
	stbi_image_free((void*)img);
	
	return image;
}

inline void saveImage(const string& file, const Image<uint8_t>& image)
{
	stbi_write_png(file.c_str(), image.getSize()[0], image.getSize()[1], image.getChannels(), image.getData().data(), 0);
}

}