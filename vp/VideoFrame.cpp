#include "VideoFrame.h"

VideoFrame::VideoFrame(int sequence) :
	sequence(sequence),
	imageOriginal(NULL),
	imageClassify(NULL)
{
}

VideoFrame::~VideoFrame()
{
	for (std::vector<VideoFrame::Blob>::size_type i = 0; i < this->blobs.size(); i++)
	{
		delete this->blobs[i];
	}

	if (this->imageOriginal)
	{
		delete[] this->imageOriginal;
	}

	if (this->imageClassify)
	{
		delete[] this->imageClassify;
	}
}

VideoFrame::Blob::Blob(int x1, int y1, int x2, int y2, int color)
{
	this->x1 = x1;
	this->x2 = x2;
	this->y1 = y1;
	this->y2 = y2;
	this->color = color;
}

VideoFrame::Blob *VideoFrame::Blob::fromRegion(CMVision::region *region)
{
	return new VideoFrame::Blob(region->x1, region->y1, region->x2, region->y2, region->color);
}

int VideoFrame::Blob::width()
{
	return abs(this->x2 - this->x1);
}

int VideoFrame::Blob::height()
{
	return abs(this->y2 - this->y1);
}
