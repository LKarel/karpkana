#ifndef DEBUGCONTROLLER_H
#define DEBUGCONTROLLER_H

#include <thread>
#include <unistd.h>
#include "google/protobuf/message.h"
#include "google/protobuf/stubs/common.h"
#include "comm/pb/frame.pb.h"
#include "comm/protocol.h"
#include "comm/DebugServer.h"
#include "vp/VideoProcessor.h"

class DebugController
{
public:
	DebugController(VideoProcessor *vp);
	~DebugController();

	void stop();

private:
	VideoProcessor *vp;
	DebugServer server;

	bool isRunning;
	std::thread thread;

	void run();
	void broadcastImage(c22dlink::FrameImage::Type type, VideoFrame *frame, rgb *image);
	void broadcastMessage(uint8_t type, google::protobuf::Message &msg);
};

#endif
