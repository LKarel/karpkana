#ifndef VP_VIDEOPROCESSOR_H
#define VP_VIDEOPROCESSOR_H

#include <mutex>
#include <opencv2/opencv.hpp>
#include <thread>
#include "Frame.h"

class VideoProcessor
{
public:
	VideoProcessor();
	~VideoProcessor();

	/**
	 * Insert a Mat for processing.
	 */
	void putMatFrame(cv::Mat mat);

	/**
	 * Get a processed frame.
	 */
	Frame *getFrame();

private:
	unsigned int sequence;

	cv::Mat *mat;
	std::mutex matMutex;

	void detectBalls(Frame *frame);
};

#endif
