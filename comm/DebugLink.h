#ifndef COMM_DEBUGLINK_H
#define COMM_DEBUGLINK_H

#include <fstream>
#include <stdio.h>
#include "Frame.h"
#include "comm/FrameBroadcaster.h"
#include "comm/DebugServer.h"
#include "objects/BaseObject.h"
#include "objects/BallObject.h"

class DebugLink
{
public:
	static const int LEVEL_DEBUG = 1;
	static const int LEVEL_INFO = 2;
	static const int LEVEL_WARN = 3;
	static const int LEVEL_ERROR = 4;

	static const uint8_t PROTOCOL_TYPE_EVENT = 0x1;
	static const uint8_t PROTOCOL_TYPE_BALL = 0x2;
	static const uint8_t PROTOCOL_TYPE_MSG = 0x7;
	static const uint8_t PROTOCOL_TYPE_FRAME = 0x8;
	static const uint8_t PROTOCOL_TYPE_FPS = 0xA;

	static const uint8_t FPS_CAPTURE = 0x01;
	static const uint8_t FPS_PROC = 0x02;
	static const uint8_t FPS_CTRL = 0x03;

	static DebugLink &instance();

	void close();
	void msg(int level, const std::string message);
	void event(int event);
	void frame(Frame *frame);
	void fps(uint8_t type, int fps);

private:
	DebugLink();
	DebugLink(DebugLink const&);
	~DebugLink();

	void operator=(DebugLink const&);
	void localMsg(int level, const std::string message);

	DebugServer *server;
	FrameBroadcaster *frameBroadcaster;
	std::ofstream logFile;
};

#endif
