#include <thread>
#include "util.h"
#include "objects/BallObject.h"
#include "VideoProcessor.h"

VideoProcessor::VideoProcessor(cv::VideoCapture *capture) :
	fps(0),
	capture(capture)
{
}

VideoProcessor::~VideoProcessor()
{
}

VideoFrame *VideoProcessor::processFrame()
{
	long beginTime = microtime();
	VideoFrame *ret = new VideoFrame();

	ret->sourceMat = new cv::Mat();
	ret->hsvMat = new cv::Mat();

	if (!this->capture->read(*ret->sourceMat))
	{
		return NULL;
	}

	cv::cvtColor(*ret->sourceMat, *ret->hsvMat, CV_BGR2HSV);

	detectBalls(ret);

	this->fps = 1000000.f / (float) (microtime() - beginTime);

	return ret;
}

void VideoProcessor::detectBalls(VideoFrame *frame)
{
	// OFF bottle cap
	cv::Scalar lower(0, 120, 165);
	cv::Scalar upper(16, 180, 210);

	cv::Mat mask(frame->hsvMat->rows, frame->hsvMat->cols, CV_8U);
	cv::inRange(*frame->hsvMat, lower, upper, mask);

	cv::GaussianBlur(mask, mask, cv::Size(13, 13), 6, 6);

	std::vector<cv::Vec3f> circles;
	cv::HoughCircles(mask, circles, CV_HOUGH_GRADIENT, 1, 100, 200, 30, 50, 450);

	for (std::vector<cv::Vec3f>::size_type i = 0; i != circles.size(); ++i)
	{
		int x = circles[i][0];
		int y = circles[i][1];
		int radius = circles[i][2];

		frame->addObject((BaseObject *) BallObject::createByVisual(x, y, radius));
	}
}
