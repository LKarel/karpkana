#ifndef ROBOT_H
#define ROBOT_H

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

	void motorsSpeed(int speed);
};

#endif
