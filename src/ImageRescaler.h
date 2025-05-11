#pragma once

#include <InteractiveToolkit/ITKCommon/ITKCommon.h>
#include <InteractiveToolkit-Extension/InteractiveToolkit-Extension.h>
#include "Bounds.h"

typedef char* PNG_ptr;

using namespace MathCore;


vec4f cubic_interp(const vec4f& p0, const vec4f& p1, const vec4f& p2, const vec4f& p3, float t)
{
	vec4f a0 = -0.5f * p0 + 1.5f * p1 - 1.5f * p2 + 0.5f * p3;
	vec4f a1 = p0 - 2.5f * p1 + 2.0f * p2 - 0.5f * p3;
	vec4f a2 = -0.5f * p0 + 0.5f * p2;
	vec4f a3 = p1;

	return ((a0 * t + a1) * t + a2) * t + a3;
}

float radial_kernel(float r) {
	if (r >= 2.0f) return 0.0f;
	r = OP<float>::maximum(r, 1e-5f); // avoid divide-by-zero
	float t = r;
	// Example: cubic B-spline-style falloff
	return (1.0f - r / 2.0f) * (1.0f - r / 2.0f);
}


class ImageRescaler
{
public:
	ITKCommon::Matrix<MathCore::vec4u8> input_image;

	ImageRescaler(const std::string& filename)
	{
		int w, h, chann, pixel_depth;
		char* data;
		data = ITKExtension::Image::PNG::readPNG(filename.c_str(), &w, &h, &chann, &pixel_depth, false);

		if (data)
		{
			if (chann == 4 && pixel_depth == 8)
			{
				uint8_t* data_uint8 = (uint8_t*)data;

				input_image.setSize(MathCore::vec2i(w, h));

				for (int y = 0; y < h; y++)
				{
					for (int x = 0; x < w; x++)
					{
						input_image[y][x] = MathCore::vec4u8(
							data_uint8[(y * w + x) * chann + 0],
							data_uint8[(y * w + x) * chann + 1],
							data_uint8[(y * w + x) * chann + 2],
							data_uint8[(y * w + x) * chann + 3]);
					}
				}
			}
			ITKExtension::Image::PNG::closePNG(data);
		}
	}
	~ImageRescaler()
	{
	}

	ITKCommon::Matrix<MathCore::vec4u8> output_image;

	void rescale(int new_w, int new_h)
	{
		using namespace MathCore;

		output_image.setSize(vec2i(new_w, new_h));
		output_image.clear(vec4u8(0xff, 0, 0xff, 0xff));

		vec2f output_pixel_size = 1.0f / (vec2f)output_image.size;

		vec2f input_sizef = (vec2f)input_image.size;
		vec2i input_sizei_minus_one = input_image.size - 1;
		vec2f input_pixel_size = 1.0f / (vec2f)input_image.size;

		ITK_ABORT(input_image.size.x != input_image.size.y, "input image has different dimentions: (%i != %i)", input_image.size.x, input_image.size.y);
		ITK_ABORT(output_image.size.x != output_image.size.y, "output image has different dimentions: (%i != %i)", output_image.size.x, output_image.size.y);

		printf("  input image size: %i, %i\n", input_image.size.x, input_image.size.y);


		if (output_image.size.x < input_image.size.x && output_image.size.y < input_image.size.y)
		{
			// case output is smaller than the input
#pragma omp parallel for
			for (int out_y = 0; out_y < output_image.size.y; out_y++)
			{
				for (int out_x = 0; out_x < output_image.size.x; out_x++)
				{
					Bounds output_bounds = Bounds(vec2i(out_x, out_y), output_pixel_size);

					vec2i aux_min = (vec2i)(OP<vec2f>::floor(output_bounds.min * input_sizef) + 0.005f);
					vec2i aux_max = (vec2i)(OP<vec2f>::ceil(output_bounds.max * input_sizef) + 0.005f);

					aux_min = OP<vec2i>::maximum(aux_min, vec2i(0));
					aux_max = OP<vec2i>::minimum(aux_max, input_sizei_minus_one);

					float total_area_sum = 0.0f;
					vec4f acc = vec4f(0, 0, 0, 0);
					for (int in_y = aux_min.y; in_y <= aux_max.y; in_y++)
					{
						for (int in_x = aux_min.x; in_x <= aux_max.x; in_x++)
						{
							Bounds input_bounds = Bounds(vec2i(in_x, in_y), input_pixel_size);
							auto intersect = Bounds::Intersect(input_bounds, output_bounds);
							ITK_ABORT(intersect.area < 0, "negative area error...");
							if (intersect.area == 0.0f) {
								continue;
							}
							float area_proportion = intersect.area / input_bounds.area;
							acc += (vec4f)input_image[in_y][in_x] * area_proportion * ((float)input_image[in_y][in_x].a / 255.0f);
							total_area_sum += area_proportion;
						}
					}
					ITK_ABORT(total_area_sum == 0.0f, "area calculation error...");
					acc /= OP<float>::maximum(total_area_sum, FloatTypeInfo<float>::min);

					output_image[out_y][out_x] = (vec4u8)(OP<vec4f>::clamp(acc, 0.0f, 255.0f) + 0.5f);

					//output_bounds.print();
					//printf("  x: %i -> %i  y: %i -> %i\n", aux_min.x, aux_max.x, aux_min.y, aux_max.y);
				}
			}
		}
		else if (output_image.size.x > input_image.size.x && output_image.size.y > input_image.size.y)
		{
			// case output is bigger than the input
#pragma omp parallel for
			for (int out_y = 0; out_y < output_image.size.y; out_y++)
			{
				for (int out_x = 0; out_x < output_image.size.x; out_x++)
				{
					auto pt_norm_space = vec2f((float)out_x, (float)out_y) * output_pixel_size;
					auto pt_input_space = pt_norm_space * input_sizef;

					vec2f center = pt_input_space;

					vec4f accum = vec4f(0.0f);
					float weight_sum = 0.0f;

					vec2i icenter = OP<vec2f>::floor(center);

					for (int dy = -2; dy <= 2; dy++)
					{
						for (int dx = -2; dx <= 2; dx++)
						{
							int sx = OP<int>::clamp(icenter.x + dx, 0, input_sizei_minus_one.x);
							int sy = OP<int>::clamp(icenter.y + dy, 0, input_sizei_minus_one.y);

							vec2f sample_pos = vec2f((float)sx, (float)sy);
							float dist = OP<vec2f>::length(center - sample_pos);  // radial distance

							float w = radial_kernel(dist);
							accum += (vec4f)input_image[sy][sx] * w * ((float)input_image[sy][sx].a / 255.0f);
							weight_sum += w;
						}
					}

					vec4f result = accum / OP<vec4f>::maximum(weight_sum, 1e-5f);
					output_image[out_y][out_x] = (vec4u8)(OP<vec4f>::clamp(result, 0.0f, 255.0f) + 0.5f);
				}
			}
		}
		else
		{
			// case output is equal to input
#pragma omp parallel for
			for (int out_y = 0; out_y < output_image.size.y; out_y++)
			{
				for (int out_x = 0; out_x < output_image.size.x; out_x++)
				{
					output_image[out_y][out_x] = input_image[out_y][out_x];
				}
			}

		}


		// ITKExtension::Image::PNG::writePNG("test.png", output_image.size.width, output_image.size.height, 4, (char*)output_image.array, false);

	}
};
