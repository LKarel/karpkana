#ifndef KBUI_H
#define KBUI_H

#include <mutex>
#include <stdio.h>
#include <thread>
#include <unistd.h>

class Kbui
{
public:
	static const int CMD_BEGIN = 1;
	static const int CMD_STOP = 2;

	Kbui();
	~Kbui();

	void stop();
	int getCommand();

private:
	std::thread thread;
	std::mutex cmdMutex;
	bool isRunning;
	int cmd;

	void run();
};

#endif
