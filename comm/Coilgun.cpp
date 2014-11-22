#include "comm/Coilgun.h"

#define PING_INTERVAL 1000000

#define CMD_CHARGE "c"
#define CMD_ASK_ID "?"
#define CMD_PING "p"
#define CMD_KICK "k%d"
#define CMD_AUTO_CHARGE "ac%d"
#define CMD_FAILSAFE "fs%d"
#define CMD_TRIBBLER_START "tg"
#define CMD_TRIBBLER_STOP "ts"

Coilgun::Coilgun(Hwlink *link) :
	link(link)
{
	Log::printf("Coilgun: initializing");

	this->lastPing = microtime();

	this->link->command(CMD_FAILSAFE, 0);
	this->tribbler(false);

	Log::printf("Coilgun: ready");
}

Coilgun::~Coilgun()
{
	Log::printf("Coilgun: shutting down");

	this->link->command(CMD_AUTO_CHARGE, 0);
	this->tribbler(false);

	delete this->link;
}

void Coilgun::tick()
{
	this->link->tick();

	long now = microtime();
	if (now - this->lastPing >= PING_INTERVAL)
	{
		this->link->command(CMD_PING);
		this->lastPing = now;
	}
}

void Coilgun::tribbler(bool active)
{
	this->link->command(active ? CMD_TRIBBLER_START : CMD_TRIBBLER_STOP);
}

void Coilgun::chargeSync()
{
	Log::printf("Coilgun: charging");
	this->link->command(CMD_CHARGE);
	usleep(700 * 1000);
}

void Coilgun::kick(uint16_t time)
{
	Log::printf("Coilgun: kicking: %d", time);
	this->link->command(CMD_KICK, time);
}
