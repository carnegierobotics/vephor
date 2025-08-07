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

#include "tensor.h"
#include <filesystem>

#define STB_IMAGE_IMPLEMENTATION
#include "thirdparty/nothings/stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "thirdparty/nothings/stb_image_write.h"

namespace vephor
{

namespace fs = std::filesystem;

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
	// all indexing in this class is done (x,y), not (row,column)
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
	bool in(const Vec2i& ind) const
	{
		if (ind[0] < 0)
			return false;
		if (ind[1] < 0)
			return false;
		if (ind[0] >= data.size()[1])
			return false;
		if (ind[1] >= data.size()[0])
			return false;
		return true;
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
		img.setData(data.template cast<S>(mult, bias));
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
	const Eigen::Matrix<T, Eigen::Dynamic, 1>& getData() const {return data.getData();}
	int getTypeSize() const {return sizeof(T);}
	void getBuffer(const char*& buf_data, int& buf_size) const
	{
		buf_data = reinterpret_cast<const char*>(data.getData().data());
		buf_size = getData().size() * sizeof(T);
	}
	void copyFromBuffer(const char* buf_data, int buf_size)
	{
		data.getData() = Eigen::Map<const Eigen::Matrix<T, Eigen::Dynamic, 1>>(
			reinterpret_cast<const T*>(buf_data),
			buf_size / sizeof(T)
		);
		
		//.assign(reinterpret_cast<const T*>(buf_data), reinterpret_cast<const T*>(buf_data+buf_size));
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

		int row_elements = test_row_end - test_row_start;
		int row_size = row_elements * sizeof(T);
		vector<T> row_holder(row_elements);

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
	void saveRaw(const string& path)
	{
		data.saveRaw(path);
	}
	void setData(const Tensor<3, T>& p_data)
	{
		data = p_data;
	}
private:
	int channels;
	Tensor<3, T> data;
};

inline std::vector<Vec2i> plotLineBresenham(const Vec2i& start, const Vec2i& end)
{
    int dx = abs(end[0] - start[0]);
    int sx = start[0] < end[0] ? 1 : -1;
    int dy = -abs(end[1] - start[1]);
    int sy = start[1] < end[1] ? 1 : -1;
    int error = dx + dy;

    std::vector<Vec2i> pts;
    int x0 = start[0];
    int y0 = start[1];

    while (true)
    {
        pts.push_back(Vec2i(x0, y0));

        if (x0 == end[0] && y0 == end[1]) break;
        int e2 = 2 * error;
        if (e2 >= dy)
        {
            if (x0 == end[0]) break;
            error = error + dy;
            x0 = x0 + sx;
        }
        if (e2 <= dx)
        {
            if (y0 == end[1]) break;
            error = error + dx;
            y0 = y0 + sy;
        }
    }

    return pts;
}

// only keep the part of the line on the positive side of the plane
inline void clipLineToPlane(Vec2& start, Vec2& end, const Vec3& plane)
{   
    const float epsilon = 1e-3;

    Vec2 vec = end - start;
    float length = vec.norm();
    if (length < epsilon)
        return;
    Vec2 slope = vec / length;

    float start_dist = start.dot(plane.head<2>()) + plane[2];
    float end_dist = end.dot(plane.head<2>()) + plane[2];

    float plane_dist_per_line_dist = slope.dot(plane.head<2>());

    if (fabs(plane_dist_per_line_dist) < epsilon)
        return;

    if (start_dist < epsilon)
        start = start + slope * ((-start_dist + epsilon) / plane_dist_per_line_dist);

    if (end_dist < epsilon)
        end = end + slope * ((-end_dist + epsilon) / plane_dist_per_line_dist);
}

inline void clipLineToImage(Vec2& start, Vec2& end, const Vec2i& image_size)
{
    clipLineToPlane(start, end, Vec3(1,0,0));
    clipLineToPlane(start, end, Vec3(0,1,0));
    clipLineToPlane(start, end, Vec3(-1,0,image_size[0]));
    clipLineToPlane(start, end, Vec3(0,-1,image_size[1]));

    if (start[0] < 0 || start[0] > image_size[0] || start[1] < 0 || start[1] > image_size[1])
    {
        start = Vec2::Zero();
        end = Vec2::Zero();
    }

    if (end[0] < 0 || end[0] > image_size[0] || end[1] < 0 || end[1] > image_size[1])
    {
        start = Vec2::Zero();
        end = Vec2::Zero();
    }
}

template <typename T, int N>
void drawLine(Image<T>& im, const Vec2& start, const Vec2& end, const Eigen::Matrix<T, N, 1>& color, int rad = 1)
{
	Vec2 line_start = start;
	Vec2 line_end = end;

	Vec2i image_size = im.getSize();
	clipLineToImage(line_start, line_end, image_size);
	auto pixels = plotLineBresenham(line_start.cast<int>(), line_end.cast<int>());

	Vec2 vec = end - start;
	Vec2 cross(vec[1], -vec[0]);
	float length = vec.norm();
	cross = cross / length;

	std::vector<Vec2i> offsets;
	for (int r = -rad; r <= rad; r++)
	{
		offsets.push_back((cross * r).cast<int>());
	}

	for (const auto& px : pixels)
	{
		for (const auto& off : offsets)
		{
			Vec2i npx(px[0]+off[0],px[1]+off[1]);
			if (im.in(npx))
				im(npx) = color;
		}
	}
}

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

inline shared_ptr<Image<float>> generateFlatNormalImage(const Vec2i& size)
{
	auto image = make_shared<Image<float>>(size[0], size[1], 3);

    Vec3 normal(0,0,1);

	for (int i = 0; i < size[0]; i++)
	{
		for (int j = 0; j < size[1]; j++)
		{
			(*image)(i,j) = normal;
		}
	}
    
    return image;
}

inline shared_ptr<Image<uint8_t>> convertNormalImageToNormalMap(const Image<float>& normal_image)
{
	auto normal_map = make_shared<Image<uint8_t>>(normal_image.getSize()[0], normal_image.getSize()[1], 3);

	for (int i = 0; i < normal_image.getSize()[0]; i++)
	{
		for (int j = 0; j < normal_image.getSize()[1]; j++)
		{
			(*normal_map)(i,j) = ((normal_image(i,j) + Vec3(1,1,1)) / 2.0f * 255.0f).cast<uint8_t>();
		}
	}
    
    return normal_map;
}

inline shared_ptr<Image<uint8_t>> loadImage(const string& file)
{
	if (!fs::exists(file))
		throw std::runtime_error("Image does not exist at path: " + file);

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