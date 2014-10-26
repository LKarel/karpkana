#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include <opencv2/opencv.hpp>
#include "Controller.h"
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
	signal(SIGPIPE, SIG_IGN);

	cv::VideoCapture capture(0);

	if (!capture.isOpened())
	{
		DebugLink::instance().msg(DebugLink::LEVEL_ERROR, "main: Failed to initialize the camera");
		capture.release();
		return 1;
	}

	capture.set(CV_CAP_PROP_FRAME_WIDTH, 1280);
	capture.set(CV_CAP_PROP_FRAME_HEIGHT, 720);
	capture.set(CV_CAP_PROP_FPS, 30);

	VideoProcessor vp;
	Controller ctrl(&vp);
	cv::Mat mat;

	DebugLink::instance().msg(DebugLink::LEVEL_INFO, "main: Starting camera loop");

	while (!sigint && ctrl.isRunning())
	{
		if (!capture.read(mat))
		{
			DebugLink::instance().msg(DebugLink::LEVEL_ERROR, "main: No frame received");
			break;
		}

		vp.putMatFrame(mat);
	}

	DebugLink::instance().msg(DebugLink::LEVEL_INFO, "main: Shutting down");
	DebugLink::instance().close();

	capture.release();
	ctrl.stop();

	return 0;
}
