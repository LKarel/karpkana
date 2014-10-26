#ifndef FRAME_H
#define FRAME_H

#include <vector>
#include <opencv2/opencv.hpp>
#include "mm.h"
#include "objects/BaseObject.h"

class Frame
{
public:
	Frame(unsigned int sequence);
	~Frame();

	void addObject(BaseObject *obj);

	struct mm_refdata _mm;

	unsigned int sequence;
	cv::Mat *sourceMat;
	cv::Mat *hsvMat;
	std::vector<BaseObject *> objects;
};

#endif
