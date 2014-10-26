#ifndef COMM_DEBUGSERVER_H
#define COMM_DEBUGSERVER_H

#include <mutex>
#include <thread>
#include <vector>

class DebugServer
{
public:
	DebugServer();
	~DebugServer();

	void start(int port);
	void stop();
	bool hasClients();
	void broadcast(const uint8_t *buf, size_t size);

private:
	int port;
	bool isRunning;
	std::thread *thread;

	std::vector<int> clients;
	std::mutex clientsMutex;

	void run();
};

#endif
