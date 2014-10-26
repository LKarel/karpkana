#include "Controller.h"

Controller::Controller(VideoProcessor *vp) :
	vp(vp),
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
		frame = this->vp->getFrame();

		if (!frame)
		{
			continue;
		}

		DebugLink::instance().frame(frame);

		delete frame;
	}
}
