#define STB_IMAGE_WRITE_IMPLEMENTATION

#include "stb_image_write.h"

#define STB_PERLIN_IMPLEMENTATION

#include "stb_perlin.h"

#include <iostream>
#include <sstream>
#include <string>
#include <memory>

#include "gradient.h"

#define minf(x, y) ((x) < (y) ? (x) : (y))
#define maxf(x, y) ((x) > (y) ? (x) : (y))

#define map1101(x) ((x) * 0.5f + 0.5f)

void nuke(std::string note)
{
	std::cout << note << std::endl;

	exit(EXIT_FAILURE);
}

int main(int argc, char** argv)
{
	gradient color_map;

	color_map.stops.push_back(gradient_stop(map1101(0.0f - 1.0000f), {0.0f, 0.0f, 0.5f}));
	color_map.stops.push_back(gradient_stop(map1101(0.0f - 0.2500f), {0.0f, 0.0f, 1.0f}));
	color_map.stops.push_back(gradient_stop(map1101(0.0f + 0.0000f), {0.0f, 0.5f, 1.0f}));
	color_map.stops.push_back(gradient_stop(map1101(0.0f + 0.0625f), {0.9f, 0.9f, 0.2f}));
	color_map.stops.push_back(gradient_stop(map1101(0.0f + 0.1250f), {0.1f, 0.6f, 0.0f}));
	color_map.stops.push_back(gradient_stop(map1101(0.0f + 0.3750f), {0.8f, 0.8f, 0.0f}));
	color_map.stops.push_back(gradient_stop(map1101(0.0f + 0.7500f), {0.5f, 0.5f, 0.5f}));
	color_map.stops.push_back(gradient_stop(map1101(0.0f + 1.0000f), {1.0f, 1.0f, 1.0f}));

	int supersample = 1;

	int x_res = 1024 * supersample;
	int y_res = 1024 * supersample;

	unsigned char* image = (unsigned char*)malloc(x_res * y_res * 3);

	if (!image)
	{
		nuke("Could not allocate a frame buffer (malloc).");
	}

	float frequency = 2.0f;

	for (int j = 0; j < y_res; j++)
	{
		std::cout << "Completed row " << j + 1 << "/" << y_res << "\r" << std::flush;

		for (int i = 0; i < x_res; i++)
		{
			unsigned char* offset = image + (j * x_res + i) * 3;

			float noise_value = stb_perlin_fbm_noise3
			(
				float(i) / float(x_res) * frequency,
				float(j) / float(y_res) * frequency,

				0.0f, 2.0f, 0.75f, 8
			);

			float cx = float(i) + 0.5f - float(x_res) / 2.0f;
			float cy = float(j) + 0.5f - float(y_res) / 2.0f;

			noise_value -= sqrtf(cx * cx + cy * cy) / float(powf(x_res, 1.0f / 1.f));

			rgb_tuple color = color_map.sample(minf(1.0f, maxf(-1.0f, noise_value)) * 0.5f + 0.5f);

			offset[0] = minf(255.000f, maxf(0.000f, color.r * 255.000f));
			offset[1] = minf(255.000f, maxf(0.000f, color.g * 255.000f));
			offset[2] = minf(255.000f, maxf(0.000f, color.b * 255.000f));
		}
	}

	if (supersample > 1)
	{
		unsigned char* supersampled = (unsigned char*)malloc
		(
			x_res / supersample *
			y_res / supersample *

			3 * sizeof(unsigned char)
		);

		if (!supersample)
		{
			nuke("Could not allocate a frame buffer for supersampling (malloc).");
		}

		x_res = x_res / supersample;
		y_res = y_res / supersample;

		for (int j = 0; j < y_res; j++)
		for (int i = 0; i < x_res; i++)
		{
			unsigned char* pixel = supersampled + (j * x_res + i) * 3;

			int ssi = i * supersample;
			int ssj = j * supersample;

			float sum_r = 0.0f;
			float sum_g = 0.0f;
			float sum_b = 0.0f;

			for (int v = 0; v < supersample; v++)
			for (int u = 0; u < supersample; u++)
			{
				unsigned char* sspixel = image + ((ssj + v) * (x_res * supersample) + (ssi + u)) * 3;

				sum_r += sspixel[0];
				sum_g += sspixel[1];
				sum_b += sspixel[2];
			}

			sum_r /= supersample * supersample;
			sum_g /= supersample * supersample;
			sum_b /= supersample * supersample;

			pixel[0] = fmin(255, fmax(0, int(sum_r)));
			pixel[1] = fmin(255, fmax(0, int(sum_g)));
			pixel[2] = fmin(255, fmax(0, int(sum_b)));
		}

		image = supersampled;
	}

	if (!stbi_write_png("terrain.png", x_res, y_res, 3, image, x_res * 3))
	{
		nuke("Could not save image (stbi_write_png).");
	}

	return EXIT_SUCCESS;
}