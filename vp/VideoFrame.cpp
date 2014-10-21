#include "VideoFrame.h"

VideoFrame::VideoFrame() :
	sourceMat(NULL),
	objects()
{
}

VideoFrame::~VideoFrame()
{
	if (this->sourceMat)
	{
		delete this->sourceMat;
	}

	if (this->hsvMat)
	{
		delete this->hsvMat;
	}

	for (std::vector<BaseObject *>::size_type i = 0; i != this->objects.size(); ++i)
	{
		delete this->objects[i];
	}
}

void VideoFrame::addObject(BaseObject *obj)
{
	this->objects.push_back(obj);
}
