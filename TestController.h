#ifndef TESTCONTROLLER_H
#define TESTCONTROLLER_H

#include <thread>
#include <unistd.h>
#include "vp/VideoProcessor.h"

class TestController
{
public:
	TestController(VideoProcessor &vp);
	~TestController();

	void stop();

private:
	VideoProcessor &vp;
	bool isRunning;
	std::thread thread;

	void run();
};

#endif
