#ifndef FRAME_H
#define FRAME_H

#include <vector>
#include <opencv2/opencv.hpp>
#include "objects/BaseObject.h"

class Frame
{
public:
	Frame();
	~Frame();

	void addObject(BaseObject *obj);

	cv::Mat *sourceMat;
	cv::Mat *hsvMat;
	std::vector<BaseObject *> objects;
};

#endif
