#ifndef VP_VIDEOPROCESSOR_H
#define VP_VIDEOPROCESSOR_H

#include <opencv2/opencv.hpp>
#include "Frame.h"

class VideoProcessor
{
public:
	VideoProcessor(cv::VideoCapture *capture);
	~VideoProcessor();

	float fps;

	Frame *processFrame();

private:
	cv::VideoCapture *capture;

	void detectBalls(Frame *frame);
};

#endif
