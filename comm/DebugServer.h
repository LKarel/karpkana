#ifndef COMM_DEBUGSERVER_H
#define COMM_DEBUGSERVER_H

#include <algorithm>
#include <map>
#include <mutex>
#include <queue>
#include <string.h>
#include <thread>
#include <vector>
#include "comm/Log.h"

class DebugServer
{
public:
	class Message
	{
	public:
		Message(uint8_t type, uint8_t *data) : type(type), data(data)
		{
		};

		~Message()
		{
			if (this->data)
			{
				delete this->data;
			}
		};

		uint8_t type;
		uint8_t *data;
	};

	class Parser
	{
	public:
		~Parser();

		void handle(uint8_t *data, size_t count);

		std::queue<DebugServer::Message *> messages;
	private:
		uint8_t type;

		int len;
		int lenReceived;

		uint8_t *payload;
		int payloadReceived;
	};

	DebugServer();
	~DebugServer();

	void start(int port);
	void stop();
	bool hasClients();
	void broadcast(const uint8_t *buf, size_t size);
	Message *getIncoming();

private:
	int port;
	bool isRunning;
	std::thread *thread;

	std::vector<int> clients;
	std::mutex clientsMutex;

	std::map<int, Parser *> parsers;
	std::mutex parsersMutex;

	std::queue<DebugServer::Message *> incoming;
	std::mutex incomingMutex;

	void run();
	Parser *getParser(int client);
};

#endif
