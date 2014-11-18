#ifndef COMM_MOTOR_H
#define COMM_MOTOR_H

#include "comm/Hwlink.h"

class Motor
{
public:
	static const int SPEED_MIN = -190;
	static const int SPEED_MAX = 190;

	static const int STALL_NONE = 0;
	static const int STALL_WARN = 1;
	static const int STALL_ERROR = 2;

	Motor(Hwlink *link);
	~Motor();

	void tick();
	void setSpeed(int speed);
	void stop();
	int getStall();
	bool queryBall();

private:
	Hwlink *link;
	int lastPing;
	int stall;
	int ballState;
};

#endif
