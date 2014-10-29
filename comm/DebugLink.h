#ifndef COMM_DEBUGLINK_H
#define COMM_DEBUGLINK_H

#include <fstream>
#include <mutex>
#include <thread>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "3rdparty/cmvision.h"
#include "3rdparty/jpge.h"
#include "comm/DebugServer.h"
#include "main.h"

#define DEBUGLINK_FPS_SAMPLES 5

class DebugLink
{
public:
	static const uint8_t PROTOCOL_TYPE_BALL = 0x2;
	static const uint8_t PROTOCOL_TYPE_IMAGE = 0x3;
	static const uint8_t PROTOCOL_TYPE_FPS = 0xA;

	static const uint8_t FPS_CAPTURE = 0x01;
	static const uint8_t FPS_PROC = 0x02;
	static const uint8_t FPS_CTRL = 0x03;

	static DebugLink &instance();

	void close();
	void image(int sequence, rgb *img);
	void fps(uint8_t type, int fps);

private:
	DebugLink();
	DebugLink(DebugLink const&);
	~DebugLink();

	void operator=(DebugLink const&);
	void run();
	void broadcastImage();

	std::thread thread;
	DebugServer *server;
	bool isRunning;
	int frameSequence;

	rgb *imageData;
	std::mutex imageMutex;

	std::vector<int> fpsData;
};

#endif
