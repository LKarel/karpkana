#include "Frame.h"

Frame::Frame() :
	sourceMat(NULL),
	objects()
{
}

Frame::~Frame()
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

void Frame::addObject(BaseObject *obj)
{
	this->objects.push_back(obj);
}
