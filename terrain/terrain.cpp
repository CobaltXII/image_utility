#define STB_IMAGE_WRITE_IMPLEMENTATION

#include "stb_image_write.h"

#include <iostream>
#include <sstream>
#include <string>
#include <memory>

#define minf(x, y) ((x) < (y) ? (x) : (y))
#define maxf(x, y) ((x) > (y) ? (x) : (y))

void nuke(std::string note)
{
	std::cout << note << std::endl;

	exit(EXIT_FAILURE);
}

int main(int argc, char** argv)
{
	int supersample = 1;

	int x_res = 1024 * supersample;
	int y_res = 1024 * supersample;

	float x_resf = float(x_res);
	float y_resf = float(y_res);

	unsigned char* image = (unsigned char*)malloc(x_res * y_res * 3);

	if (!image)
	{
		nuke("Could not allocate a frame buffer (malloc).");
	}

	float gamma = 1.0f / 2.2f;

	for (int j = 0; j < y_res; j++)
	{
		std::cout << "Completed row " << j + 1 << "/" << y_res << "\r" << std::flush;

		for (int i = 0; i < x_res; i++)
		{
			unsigned char* offset = image + (j * x_res + i) * 3;

			// Generate prime ray.

			float ray_dx = (0.0f + (float(i + 0.5f) / x_resf) * 2.0f - 1.0f) * fov_adjust;
			float ray_dy = (1.0f - (float(j + 0.5f) / y_resf) * 2.0f + 0.0f) * fov_adjust;

			float ray_dz = -1.0f;

			ray_dx *= aspect;

			float ray_length = sqrtf
			(
				ray_dx * ray_dx +
				ray_dy * ray_dy +
				ray_dz * ray_dz
			);

			ray_dx /= ray_length;
			ray_dy /= ray_length;
			ray_dz /= ray_length;

			float color_r = float(i) / float(x_res);
			float color_g = float(j) / float(y_res);

			float color_b = 0.500f;

			offset[0] = minf(255.000f, maxf(0.000f, powf(color_r, gamma) * 255.000f));
			offset[1] = minf(255.000f, maxf(0.000f, powf(color_g, gamma) * 255.000f));
			offset[2] = minf(255.000f, maxf(0.000f, powf(color_b, gamma) * 255.000f));
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