#ifndef COMM_DEBUGSERVER_H
#define COMM_DEBUGSERVER_H

#include <algorithm>
#include <map>
#include <mutex>
#include <queue>
#include <string.h>
#include <thread>
#include <vector>
#include "google/protobuf/message.h"
#include "comm/Log.h"
#include "comm/pb/colors.pb.h"
#include "comm/protocol.h"

class DebugServer
{
public:
	class Parser
	{
	public:
		~Parser();

		void handle(uint8_t *data, size_t count);

		std::queue<google::protobuf::Message *> messages;
	private:
		uint8_t type;

		int len;
		int lenReceived;

		uint8_t *payload;
		int payloadReceived;

		google::protobuf::Message *parseProtobuf(int type, uint8_t *data, size_t len);
	};

	DebugServer();
	~DebugServer();

	void start(int port);
	void stop();
	bool hasClients();
	void broadcast(const uint8_t *buf, size_t size);
	google::protobuf::Message *getIncoming();

private:
	int port;
	bool isRunning;
	std::thread *thread;

	std::vector<int> clients;
	std::mutex clientsMutex;

	std::map<int, Parser *> parsers;
	std::mutex parsersMutex;

	std::queue<google::protobuf::Message *> incoming;
	std::mutex incomingMutex;

	void run();
	Parser *getParser(int client);
};

#endif
