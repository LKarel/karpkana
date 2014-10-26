#include <thread>
#include "Frame.h"
#include "util.h"
#include "objects/BallObject.h"
#include "VideoProcessor.h"

VideoProcessor::VideoProcessor() :
	sequence(0),
	mat(NULL)
{
}

VideoProcessor::~VideoProcessor()
{
}

void VideoProcessor::putMatFrame(cv::Mat mat)
{
	std::lock_guard<std::mutex> lock(this->matMutex);

	if (this->mat)
	{
		delete this->mat;
	}

	this->mat = new cv::Mat(mat.clone());
}

Frame *VideoProcessor::getFrame()
{
	cv::Mat *source;

	{
		std::lock_guard<std::mutex> lock(this->matMutex);
		source = this->mat;

		// Avoid the Mat from getting deleted in putMatFrame
		this->mat = NULL;
	}

	if (!source)
	{
		return NULL;
	}

	Frame *ret = new Frame(this->sequence++);

	ret->sourceMat = source;
	ret->hsvMat = new cv::Mat();

	// Convert colors to HSV
	cv::cvtColor(*ret->sourceMat, *ret->hsvMat, CV_BGR2HSV);

	detectBalls(ret);

	return ret;
}

void VideoProcessor::detectBalls(Frame *frame)
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
