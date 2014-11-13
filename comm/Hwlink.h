#ifndef COMM_HWLINK_H
#define COMM_HWLINK_H

#include <errno.h>
#include <fcntl.h>
#include <queue>
#include <stdlib.h>
#include <string.h>
#include <termio.h>
#include <unistd.h>
#include <vector>
#include "main.h"
#include "comm/Log.h"

#define COMM_HWLINK_MOTORS_COUNT 3
#define COMM_HWLINK_TIMEOUT 1500000

class Hwlink
{
public:
	Hwlink(const char *device);
#if HW_SIMULATE == 1
	Hwlink(int id);
#endif
	~Hwlink();

	bool isOpen();
	void tick();
	void command(const char *fmt, ...);

	std::queue<char *> messages;
	int id;

private:
	int fd;
	std::vector<char> buffer;
};

#endif
