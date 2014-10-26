#ifndef COMM_FRAMEBROADCASTER_H
#define COMM_FRAMEBROADCASTER_H

#include <mutex>
#include <thread>
#include <unistd.h>
#include "mm.h"
#include "Frame.h"
#include "comm/DebugServer.h"
#include "objects/BallObject.h"
#include "objects/BaseObject.h"

class FrameBroadcaster
{
public:
	FrameBroadcaster(DebugServer *server);
	~FrameBroadcaster();

	void stop();
	void putFrame(Frame *frame);

private:
	bool isRunning;
	DebugServer *server;
	std::thread thread;

	Frame *frame;
	std::mutex frameMutex;

	void run();
	void broadcastFrame(Frame *frame);
	void broadcastObject(int sequence, BaseObject *object);
	void broadcastBallObject(int sequence, BallObject *ball);
};

#endif
