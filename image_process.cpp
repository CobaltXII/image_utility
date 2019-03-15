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

	int x_res;
	int y_res;

	unsigned char* source_image = stbi_load(argv[1], &x_res, &y_res, NULL, 3);

	if (!source_image)
	{
		nuke("Could not load an image (stbi_load).");
	}

	unsigned char* image = (unsigned char*)malloc(x_res * y_res * 3);

	if (!image)
	{
		nuke("Could not allocate a frame buffer (malloc).");
	}

	for (int j = 0; j < y_res; j++)
	{
		std::cout << "Completed row " << j + 1 << "/" << y_res << "\r" << std::flush;

		for (int i = 0; i < x_res; i++)
		{
			unsigned char* offset_source = source_image + (j * x_res + i) * 3;

			float source_r = offset_source[0] / 255.0f;
			float source_g = offset_source[1] / 255.0f;
			float source_b = offset_source[2] / 255.0f;

			float processed_r;
			float processed_g;
			float processed_b;

			{
				// Brighten.
				
				processed_r = source_r * 2.2f;
				processed_g = source_g * 2.2f;
				processed_b = source_b * 2.2f;
			}

			unsigned char* offset = image + (j * x_res + i) * 3;

			offset[0] = minf(255.000f, maxf(0.000f, processed_r * 255.000f));
			offset[1] = minf(255.000f, maxf(0.000f, processed_g * 255.000f));
			offset[2] = minf(255.000f, maxf(0.000f, processed_b * 255.000f));
		}
	}

	if (!stbi_write_png("image_process.png", x_res, y_res, 3, image, x_res * 3))
	{
		nuke("Could not save image (stbi_write_png).");
	}

	return EXIT_SUCCESS;
}