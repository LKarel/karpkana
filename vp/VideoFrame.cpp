#include "VideoFrame.h"

VideoFrame::VideoFrame(int sequence) :
	sequence(sequence)
{
}

VideoFrame::~VideoFrame()
{
	for (std::vector<VideoFrame::Blob>::size_type i = 0; i < this->blobs.size(); i++)
	{
		delete this->blobs[i];
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
	if (region->color == VideoFrame::Blob::COLOR_BALL)
	{
		double width = (double) abs(region->x2 - region->x1);
		double ratio = width / (double) abs(region->y2 - region->y1);

		if (width < 35 || ratio > 1.45 || ratio < 0.65)
		{
			return NULL;
		}
	}

	return new VideoFrame::Blob(region->x1, region->y1, region->x2, region->y2, region->color);
}
