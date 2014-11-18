#include "comm/Motor.h"

#define PING_INTERVAL 1000000

#define CMD_PING "p"
#define CMD_SET_SPEED "sd%d"
#define CMD_FAILSAFE "fs%d"
#define CMD_SET_P "pg%d"
#define CMD_SET_I "ig%d"
#define CMD_QUERY_BALL "gb"

Motor::Motor(Hwlink *link) :
	link(link),
	lastPing(0),
	stall(0),
	ballState(-1)
{
	this->link->command(CMD_FAILSAFE, 1);
	this->link->command(CMD_SET_P, 6);
	this->link->command(CMD_SET_I, 8);
}

Motor::~Motor()
{
	this->setSpeed(0);

	delete this->link;
}

void Motor::tick()
{
	this->link->tick();

	long now = microtime();
	if (now - lastPing > PING_INTERVAL)
	{
		this->link->command(CMD_PING);
		this->lastPing = now;
	}

	while (!this->link->messages.empty())
	{
		char *msg = this->link->messages.front();
		int arg;

		if (sscanf(msg, "<stall:%d>", &arg) == 1)
		{
			if (arg >= 0 && arg <= 3)
			{
				this->stall = arg;

				Log::printf("Motor<id=%d>: stall changed: %d", this->link->id, this->stall);
			}
		}
		else if (sscanf(msg, "<b:%d>", &arg) == 1)
		{
			this->ballState = arg;

			Log::printf("Motor<id=%d>: ball state: %d", this->link->id, this->ballState);
		}

		free(msg);
		this->link->messages.pop();
	}
}

void Motor::setSpeed(int speed)
{
	if (speed < SPEED_MIN || speed > SPEED_MAX)
	{
		Log::printf("Motor: error: speed out of allowed bounds: %d", speed);
		return;
	}

	this->link->command(CMD_SET_SPEED, speed);
}

void Motor::stop()
{
	this->setSpeed(0);
}

int Motor::getStall()
{
	return this->stall;
}

bool Motor::queryBall()
{
	this->ballState = -1;
	this->link->command(CMD_QUERY_BALL);

	while (this->ballState == -1)
	{
		 //Block, until the result is available
		this->tick();
		usleep(500);
	}

	return (bool) this->ballState;
}
