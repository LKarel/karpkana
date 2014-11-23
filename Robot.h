#ifndef ROBOT_H
#define ROBOT_H

#define DIRECTION_FWD 1

#include <dirent.h>
#include "main.h"
#include "comm/Coilgun.h"
#include "comm/Hwlink.h"
#include "comm/Log.h"
#include "comm/Motor.h"

class Robot
{
public:
	Robot(const char *device);
	~Robot();

	Motor *motors[MOTORS_NUM];
	Coilgun *coilgun;

	int getStall() const;

	void tick();
	void rotate(int speed);
	void drive(int velocity, double direction, int rotate);
	void rotateCurved(int rotate, double mod);
	void stop();
	void direction(int direction, int speed);
};

#endif
