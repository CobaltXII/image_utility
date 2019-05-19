#define STB_IMAGE_IMPLEMENTATION

#include "stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION

#include "stb_image_write.h"

#include <iostream>
#include <sstream>
#include <string>
#include <memory>

#define minf(x, y) ((x) < (y) ? (x) : (y))
#define maxf(x, y) ((x) > (y) ? (x) : (y))

int x_res;
int y_res;

struct rgb {
	float r;
	float g;
	float b;
};

rgb peek(unsigned char* img, int x, int y) {
	if (x < 0 || x >= x_res ||
		y < 0 || y >= y_res)
		return {0.0f, 0.0f, 0.0f};
	unsigned char* offset = img + (y * x_res + x) * 3;
	return {
		offset[0] / 255.0f,
		offset[1] / 255.0f,
		offset[2] / 255.0f
	};
}

void poke(unsigned char* img, int x, int y, rgb pixel) {
	if (x < 0 || x >= x_res ||
		y < 0 || y >= y_res)
		return;
	unsigned char* offset = img + (y * x_res + x) * 3;
	offset[0] = minf(255.000f, maxf(0.000f, pixel.r * 255));
	offset[1] = minf(255.000f, maxf(0.000f, pixel.g * 255));
	offset[2] = minf(255.000f, maxf(0.000f, pixel.b * 255));
}

void nuke(std::string note)
{
	std::cout << note << std::endl;

	exit(EXIT_FAILURE);
}

int main(int argc, char** argv)
{
	if (argc != 2)
	{
		std::cout << "Usage: " << argv[0] << " <path>" << std::endl;
	}

	unsigned char* source_image = stbi_load(argv[1], &x_res, &y_res, NULL, 3);

	if (!source_image)
	{
		nuke("Could not load an image (stbi_load).");
	}

	std::cout << "x_res: " << x_res << std::endl;
	std::cout << "y_res: " << y_res << std::endl;

	unsigned char* image = (unsigned char*)malloc(x_res * y_res * 3);

	memcpy(image, source_image, x_res * y_res * 3);

	if (!image)
	{
		nuke("Could not allocate a frame buffer (malloc).");
	}

	const float darken = 1.0f;
	const float darken2 = 4.0f;

	for (int j = 0; j < y_res; j++)
	{
		std::cout << "Completed row " << j + 1 << "/" << y_res << "\r" << std::flush;

		for (int i = 0; i < x_res; i++)
		{
			// Lazy man's NTSC filter.
			rgb source = peek(source_image, i, j);
			rgb left = peek(image, i - 1, j);
			// rgb left2 = peek(image, i - 2, j);
			rgb right = peek(image, i + 1, j);
			rgb bottom = peek(image, i, j + 1);
			left.r += source.r / darken;
			// left2.r += source.r / darken2;
			// left2.g += source.g / darken2;
			// left2.b += source.b / darken2;
			right.g += source.g / darken;
			bottom.b += source.b / darken;
			poke(image, i - 1, j, left);
			// poke(image, i - 2, j, left2);
			poke(image, i + 1, j, right);
			// poke(image, i, j + 1, bottom);
		}
	}

	if (!stbi_write_png("singlepass.png", x_res, y_res, 3, image, x_res * 3))
	{
		nuke("Could not save image (stbi_write_png).");
	}

	return EXIT_SUCCESS;
}