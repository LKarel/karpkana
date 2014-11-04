#ifndef TESTCONTROLLER_H
#define TESTCONTROLLER_H

#include <thread>
#include <unistd.h>
#include "World.h"
#include "comm/HwScanner.h"
#include "comm/Log.h"
#include "comm/Motor.h"
#include "vp/VideoFrame.h"
#include "vp/VideoProcessor.h"

class TestController
{
public:
	TestController(VideoProcessor *vp);
	~TestController();

	void stop();

private:
	VideoProcessor *vp;
	bool isRunning;
	std::thread thread;
	World world;

	void run();
};

#endif
