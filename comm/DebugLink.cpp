#include <iostream>
#include <opencv2/opencv.hpp>
#include <stdio.h>
#include <string.h>
#include "util.h"
#include "comm/DebugLink.h"
#include "comm/DebugServer.h"

DebugLink::DebugLink() :
	logFile("debuglink_local.log", std::ios::out | std::ios::app)
{
	this->server = new DebugServer();
	this->server->start(11000);
}

DebugLink::~DebugLink()
{
	if (this->server)
	{
		delete this->server;
	}

	if (this->logFile.is_open())
	{
		this->logFile.close();
	}
}

DebugLink &DebugLink::instance()
{
	static DebugLink instance;
	return instance;
}

void DebugLink::close()
{
	if (this->server)
	{
		this->server->stop();
	}
}

void DebugLink::msg(int level, const std::string message)
{
	this->localMsg(level, message);

	if (this->server->hasClients())
	{
		size_t len = (size_t) message.length();

		if (len > 65535)
		{
			len = 65535;
			this->msg(DebugLink::LEVEL_WARN, "DebugLink: Outgoing message exceeds 65535 bytes, ignoring the rest");
		}

		uint8_t data[len + 4];

		data[0] = DebugLink::PROTOCOL_TYPE_MSG;
		data[1] = level;
		data[2] = (len >> 8) & 0xFF;
		data[3] = len & 0xFF;

		for (int i = 0; i < len; ++i)
		{
			data[i + 4] = static_cast<uint8_t>(message[i]);
		}

		this->server->broadcast(data, len + 4);
	}
}

void DebugLink::event(int event)
{
	this->localMsg(DebugLink::LEVEL_INFO, std::string("Emitting event ")
		.append(std::to_string(event)));

	if (this->server->hasClients())
	{
		if (event > 65535)
		{
			// Cannot send event of this type
			return;
		}

		uint8_t data[] = {
			DebugLink::PROTOCOL_TYPE_EVENT,
			(uint8_t) ((event >> 8) & 0xFF),
			(uint8_t) ((event >> 0) & 0xFF)
		};

		this->server->broadcast(data, 3);
	}
}

void DebugLink::object(int sequence, BaseObject *object)
{
}

void DebugLink::frame(Frame *frame)
{
	if (!this->server->hasClients())
	{
		return;
	}

	int width = 640;
	int height = 360;

	cv::Mat mat;
	cv::resize(*frame->sourceMat, mat, cv::Size(width, height));

	std::vector<unsigned char> buffer;
	cv::imencode(".jpg", mat, buffer);

	uint8_t header[] = {
		DebugLink::PROTOCOL_TYPE_FRAME,

		// Frame sequence number
		(uint8_t) ((frame->sequence >> 24) & 0xFF),
		(uint8_t) ((frame->sequence >> 16) & 0xFF),
		(uint8_t) ((frame->sequence >> 8) & 0xFF),
		(uint8_t) (frame->sequence & 0xFF),

		// Frame height
		(uint8_t) ((frame->hsvMat->size().height >> 8) & 0xFF),
		(uint8_t) (frame->hsvMat->size().height & 0xFF),

		// Image width
		(uint8_t) ((width >> 8) & 0xFF),
		(uint8_t) (width & 0xFF),

		// Image height
		(uint8_t) ((height >> 8) & 0xFF),
		(uint8_t) (height & 0xFF),

		(uint8_t) ((buffer.size() >> 24) & 0xFF),
		(uint8_t) ((buffer.size() >> 16) & 0xFF),
		(uint8_t) ((buffer.size() >> 8) & 0xFF),
		(uint8_t) (buffer.size() & 0xFF)
	};

	this->server->broadcast(header, 15);
	this->server->broadcast(&buffer[0], buffer.size());

	for (std::vector<BaseObject *>::size_type i = 0; i < frame->objects.size(); ++i)
	{
		object(frame->sequence, frame->objects[i]);
	}
}

void DebugLink::localMsg(int level, const std::string message)
{
	std::string levelStr;

	switch (level)
	{
		case DebugLink::LEVEL_INFO: levelStr = "INFO"; break;
		case DebugLink::LEVEL_WARN: levelStr = "WARN"; break;
		case DebugLink::LEVEL_ERROR: levelStr = "ERROR"; break;
		default: levelStr = "DEBUG";
	}

	char log[2048];
	snprintf(log, 2048, "[%llu] %s: %s", microtime() / 1000, levelStr.c_str(), message.c_str());

	if (this->logFile.is_open())
	{
		this->logFile << log << "\n";
	}

	std::cout << log << std::endl;
}
