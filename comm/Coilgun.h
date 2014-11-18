#ifndef COMM_COILGUN_H
#define COMM_COILGUN_H

#include <errno.h>
#include <fcntl.h>
#include <inttypes.h>
#include <unistd.h>
#include <vector>
#include "comm/Hwlink.h"
#include "comm/Log.h"
#include "util.h"

class Coilgun
{
public:
	Coilgun(Hwlink *link);
	~Coilgun();

	void tick();
	void command(const char *cmd, ...);
	void tribbler(bool active);
	void chargeSync();
	void kick(uint16_t time);

private:
	Hwlink *link;
	long lastPing;

	std::vector<char> parseBuf;
};

#endif
