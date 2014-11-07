#include "vp/ImageCamera.h"

ImageCamera::ImageCamera(const char *path)
{
	int width = 0;
	int height = 0;
	int comps = 0;

	unsigned char *rgb = jpgd::decompress_jpeg_image_from_file(path, &width, &height, &comps, 3);

	if (!rgb)
	{
		Log::printf("ImageCamera: opening or decoding the image failed");
		assert(false);
		return;
	}

	assert(width == CAPT_WIDTH);
	assert(height == CAPT_HEIGHT);

	this->data = (unsigned char *) malloc(width * height * 2);

	for (ssize_t i = 0; i < (width * height) / 2; ++i)
	{
		size_t p1 = i * 6;
		size_t p2 = (i * 6) + 3;

		int r = (rgb[p1] + rgb[p2]) / 2;
		int g = (rgb[p1 + 1] + rgb[p2 + 1]) / 2;
		int b = (rgb[p1 + 2] + rgb[p2 + 2]) / 2;

		int y1 = 0.257 * rgb[p1] + 0.504 * rgb[p1 + 1] + 0.098 * rgb[p1 + 2] + 16;
		int y2 = 0.257 * rgb[p2] + 0.504 * rgb[p2 + 1] + 0.098 * rgb[p2 + 2] + 16;
		int u = -0.148 * r - 0.291 * g + 0.439 * b + 128;
		int v =  0.439 * r - 0.368 * g - 0.071 * b + 128;

		this->data[i * 4] = LIMIT(y1, 0, 255);
		this->data[(i * 4) + 1] = LIMIT(u, 0, 255);
		this->data[(i * 4) + 2] = LIMIT(y2, 0, 255);
		this->data[(i * 4) + 3] = LIMIT(v, 0, 255);
	}
}

ImageCamera::~ImageCamera()
{
	free(this->data);
}
