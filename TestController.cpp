#include "TestController.h"

TestController::TestController(VideoProcessor &vp) :
	vp(vp),
	isRunning(true)
{
	this->thread = std::thread(&TestController::run, this);
}

TestController::~TestController()
{
}

void TestController::stop()
{
	if (!this->isRunning)
	{
		return;
	}

	this->isRunning = false;
	this->thread.join();
}

void TestController::run()
{
	while (this->isRunning)
	{
		VideoFrame *frame = this->vp.getFrame();

		if (!frame)
		{
			usleep(5000);
			continue;
		}
	}
}
