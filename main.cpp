#include <stdio.h>
#include <opencv2/opencv.hpp>
#include "Frame.h"
#include "objects/BallObject.h"
#include "vp/VideoProcessor.h"

int main(int argc, char** argv)
{
	cv::VideoCapture capture(0);

	if (!capture.isOpened())
	{
		// Failed to access the camera
		return -1;
	}

	capture.set(CV_CAP_PROP_FRAME_WIDTH, 1280);
	capture.set(CV_CAP_PROP_FRAME_HEIGHT, 720);
	capture.set(CV_CAP_PROP_FPS, 60);

	VideoProcessor vp(&capture);
	cv::Point fpsPosition(5, 20);
	char fpsString[16];

	while (true)
	{
		Frame *frame = vp.processFrame();
		cv::Mat preview(*frame->sourceMat);

		for (std::vector<BaseObject *>::size_type i = 0; i != frame->objects.size(); ++i)
		{
			switch (frame->objects[i]->type)
			{
				case BallObject::TYPE:
				{
					BallObject *ball = (BallObject *) frame->objects[i];

					cv::circle(preview,
						cv::Point(ball->visual.x, ball->visual.y),
						ball->visual.radius,
						cv::Scalar(0, 0, 255));
				}
				break;
			}
		}

		snprintf(fpsString, 16, "FPS: %.f", vp.fps);
		cv::putText(preview,
			fpsString, fpsPosition,
			cv::FONT_HERSHEY_SIMPLEX, 0.4,
			cv::Scalar(0, 0, 255));

		cv::imshow("Frame", preview);
		cv::waitKey(1);

		delete frame;
	}

	capture.release();

	return 0;
}
