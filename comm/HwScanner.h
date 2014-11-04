#ifndef COMM_HWSCANNER_H
#define COMM_HWSCANNER_H

#include <dirent.h>
#include "main.h"
#include "comm/Coilgun.h"
#include "comm/Hwlink.h"
#include "comm/Log.h"
#include "comm/Motor.h"

class HwScanner
{
public:
	HwScanner(const char *device);
	~HwScanner();

	Motor *motors[MOTORS_NUM];
	Coilgun *coilgun;
};

#endif
