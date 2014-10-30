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
#include "comm/Log.h"

#define COMM_HWLINK_MOTORS_COUNT 3
//#define COMM_HWLINK_TIMEOUT 1500000
#define COMM_HWLINK_TIMEOUT 1000000000

class Hwlink
{
public:
	Hwlink(const char *device);
	~Hwlink();

	bool isOpen();
	void tick();
	void command(const char *fmt, ...);

	std::queue<char *> messages;

private:
	int fd;
	int id;
	std::vector<char> buffer;
};

#endif
