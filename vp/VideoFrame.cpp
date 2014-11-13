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
	int y1 = CAPT_HEIGHT - region->y1;
	int y2 = CAPT_HEIGHT - region->y2;

	return new VideoFrame::Blob(region->x1, y1, region->x2, y2, region->color);
}

double VideoFrame::Blob::overlap(VideoFrame::Blob *blob)
{
	int overlapArea = MAX(0, MIN(this->x2, blob->x2) - MAX(this->x1, blob->x1)) *
		MAX(0, MAX(this->y1, blob->y1) - MIN(this->y2, blob->y2));
	int unionArea = this->width() * this->height() +
		blob->width() * blob->height() - overlapArea;

	return (double) overlapArea / (double) unionArea;
}

void VideoFrame::Blob::consume(VideoFrame::Blob *blob)
{
	this->x1 = MIN(this->x1, blob->x1);
	this->y1 = MAX(this->y1, blob->y1);
	this->x2 = MAX(this->x2, blob->x2);
	this->y2 = MIN(this->y2, blob->y2);
}

int VideoFrame::Blob::width()
{
	return abs(this->x2 - this->x1);
}

int VideoFrame::Blob::height()
{
	return abs(this->y2 - this->y1);
}
