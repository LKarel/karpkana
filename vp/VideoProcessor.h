#ifndef VP_VIDEOPROCESSOR_H
#define VP_VIDEOPROCESSOR_H

#include <opencv2/opencv.hpp>
#include "vp/VideoFrame.h"

class VideoProcessor
{
public:
	VideoProcessor(cv::VideoCapture *capture);
	~VideoProcessor();

	float fps;

	VideoFrame *processFrame();

private:
	cv::VideoCapture *capture;

	void detectBalls(VideoFrame *frame);
};

#endif
