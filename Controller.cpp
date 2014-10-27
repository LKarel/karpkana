#include "Controller.h"

Controller::Controller(VideoProcessor *vp) :
	vp(vp),
	fps(0),
	running(true)
{
	this->thread = std::thread(&Controller::run, this);
}

Controller::~Controller()
{
	stop();
}

bool Controller::isRunning()
{
	return this->running;
}

void Controller::stop()
{
	if (!this->running)
	{
		return;
	}

	this->running = false;
	this->thread.join();
}

void Controller::run()
{
	Frame *frame;

	while (this->running)
	{
		long begin = microtime();

		frame = this->vp->getFrame();

		if (!frame)
		{
			continue;
		}

		MM_INIT(frame);

		DebugLink::instance().frame(frame);

		MM_DEC(frame);

		DebugLink::instance().fps(DebugLink::FPS_CTRL, 1000000.0 / (microtime() - begin));
	}
}
