#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include <opencv2/opencv.hpp>
#include "Frame.h"
#include "comm/DebugLink.h"
#include "objects/BallObject.h"
#include "vp/VideoProcessor.h"

bool sigint = false;

void handleSigint(int signal)
{
	sigint = true;
}

int main(int argc, char** argv)
{
	signal(SIGINT, handleSigint);

	cv::VideoCapture capture(0);

	if (!capture.isOpened())
	{
		DebugLink::instance().msg(DebugLink::LEVEL_ERROR, "main: Failed to initialize the camera");
		capture.release();
		return 1;
	}

	capture.set(CV_CAP_PROP_FRAME_WIDTH, 1280);
	capture.set(CV_CAP_PROP_FRAME_HEIGHT, 720);
	capture.set(CV_CAP_PROP_FPS, 60);

	VideoProcessor vp(&capture);
	cv::Point fpsPosition(5, 20);
	char fpsString[16];

	while (!sigint)
	{
		Frame *frame = vp.processFrame();

		if (frame)
		{
			DebugLink::instance().frame(frame);
			delete frame;
		}
	}

	DebugLink::instance().msg(DebugLink::LEVEL_INFO, "main: Shutting down");
	DebugLink::instance().close();

	capture.release();

	return 0;
}
