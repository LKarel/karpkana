#include "VideoFrame.h"

#define GOAL_WIDTH_MIN 95
#define GOAL_RATIO_MIN 2.5
#define GOAL_RATIO_MAX 6.0

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

	double width = (double) abs(region->x2 - region->x1);
	double ratio = width / (double) abs(y2 - y1);

	if (region->color == VideoFrame::Blob::COLOR_BALL)
	{
		if (width > GOAL_WIDTH_MIN && ratio > GOAL_RATIO_MIN && ratio < GOAL_RATIO_MAX)
		{
			// It's a orange-like goal
			region->color = VideoFrame::Blob::COLOR_YELLOW;
		}
		else
		{
			// It's probably a ball
			if (width < 33 || ratio > 1.45 || ratio < 0.65)
			{
				return NULL;
			}
		}
	}

	if (region->color == VideoFrame::Blob::COLOR_YELLOW ||
		region->color == VideoFrame::Blob::COLOR_BLUE)
	{
		if (width < GOAL_WIDTH_MIN || ratio > GOAL_RATIO_MAX || ratio < GOAL_RATIO_MIN)
		{
			return NULL;
		}
	}

	return new VideoFrame::Blob(region->x1, y1, region->x2, y2, region->color);
}

int VideoFrame::Blob::width()
{
	return abs(this->x2 - this->x1);
}

int VideoFrame::Blob::height()
{
	return abs(this->y2 - this->y1);
}
