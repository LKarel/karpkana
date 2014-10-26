#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <thread>
#include "Frame.h"
#include "comm/DebugLink.h"
#include "vp/VideoProcessor.h"

class Controller
{
public:
	Controller(VideoProcessor *vp);
	~Controller();

	bool isRunning();
	void stop();

private:
	bool running;
	std::thread thread;

	VideoProcessor *vp;

	void run();
};

#endif
