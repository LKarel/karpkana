#include "comm/Coilgun.h"

#define PING_INTERVAL 1000000

#define CMD_CHARGE "c"
#define CMD_ASK_ID "?"
#define CMD_PING "p"
#define CMD_KICK "k%d"
#define CMD_AUTO_CHARGE "ac%d"
#define CMD_FAILSAFE "fs%d"
#define CMD_DISCHARGE "d"
#define CMD_TRIBBLER_START "tg"
#define CMD_TRIBBLER_STOP "ts"

Coilgun::Coilgun(Hwlink *link) :
	link(link),
	chargeBegin(0)
{
	Log::printf("Coilgun: initializing");

	this->lastPing = microtime();

	this->link->command(CMD_FAILSAFE, 0);
	this->link->command(CMD_AUTO_CHARGE, 0);
	this->tribbler(false);

	Log::printf("Coilgun: ready");
}

Coilgun::~Coilgun()
{
	Log::printf("Coilgun: shutting down");

	this->link->command(CMD_DISCHARGE);
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

void Coilgun::chargePreload()
{
	long diff = microtime() - this->chargeBegin;

	if (diff > COILGUN_CHARGE_EXPIRE)
	{
		Log::printf("Coilgun: pre-charging");

		this->chargeBegin = microtime();
		this->link->command(CMD_CHARGE);
	}
}

void Coilgun::chargeSync()
{
	long diff = microtime() - this->chargeBegin;

	if (diff < COILGUN_MIN_CHARGE)
	{
		usleep(diff);
	}

	if (diff > COILGUN_CHARGE_EXPIRE)
	{
		this->link->command(CMD_CHARGE);
		usleep(COILGUN_MIN_CHARGE);
	}

	this->chargeBegin = 0;
}

void Coilgun::kick(uint16_t time)
{
	Log::printf("Coilgun: kicking: %d", time);

	this->link->command(CMD_KICK, time);
	this->chargeBegin = 0;
}
