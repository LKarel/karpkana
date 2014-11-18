#ifndef KBUI_H
#define KBUI_H

#include <fcntl.h>
#include <mutex>
#include <queue>
#include <stdio.h>
#include <thread>
#include <unistd.h>

class Kbui
{
public:
	static const int CMD_BEGIN = 1;
	static const int CMD_STOP = 2;

	Kbui(int fd);
	~Kbui();

	void stop();
	int cmd();

private:
	std::thread thread;
	std::mutex cmdsMutex;
	std::queue<int> cmds;
	bool isRunning;
	int fd;

	void run();
};

#endif
