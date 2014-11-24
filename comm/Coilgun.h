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

#define COILGUN_MIN_CHARGE 1200000
#define COILGUN_CHARGE_EXPIRE 5000000

class Coilgun
{
public:
	Coilgun(Hwlink *link);
	~Coilgun();

	void tick();
	void command(const char *cmd, ...);
	void tribbler(bool active);
	void chargePreload();
	void chargeSync();
	void kick(uint16_t time);

private:
	Hwlink *link;
	long lastPing;
	long chargeBegin;

	std::vector<char> parseBuf;
};

#endif
