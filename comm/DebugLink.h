#ifndef COMM_DEBUGLINK_H
#define COMM_DEBUGLINK_H

#include <fstream>
#include <mutex>
#include <stdio.h>
#include <string.h>
#include <thread>
#include <vector>
#include "objects/BaseObject.h"

class DebugLink
{
public:
	static const int LEVEL_DEBUG = 1;
	static const int LEVEL_INFO = 2;
	static const int LEVEL_WARN = 3;
	static const int LEVEL_ERROR = 4;

	static const uint8_t PROTOCOL_TYPE_EVENT = 0x1;
	static const uint8_t PROTOCOL_TYPE_MSG = 0x7;

	DebugLink();
	~DebugLink();

	void close();
	void msg(int level, const std::string message);
	void event(int event);
	void object(int sequence, BaseObject *object);

private:
	class Server
	{
	public:
		Server(DebugLink *dl, int port);
		~Server();

		void start();
		void stop();
		int broadcast(const uint8_t *buf, size_t size);

	private:
		DebugLink *debug;
		int port;
		bool isRunning;
		std::vector<int> clients;
		std::mutex clientsMutex;
		std::thread *thread;

		void listen();
	};

	void localMsg(int level, const std::string message);

	Server server;
	std::ofstream logFile;
};

#endif
