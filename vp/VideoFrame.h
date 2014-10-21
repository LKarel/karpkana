#ifndef VP_VIDEOFRAME_H
#define VP_VIDEOFRAME_H

#include <vector>
#include <opencv2/opencv.hpp>
#include "objects/BaseObject.h"

class VideoFrame
{
public:
	VideoFrame();
	~VideoFrame();

	void addObject(BaseObject *obj);

	cv::Mat *sourceMat;
	cv::Mat *hsvMat;
	std::vector<BaseObject *> objects;
};

#endif
