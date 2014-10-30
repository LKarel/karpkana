#ifndef COMM_LOG_H
#define COMM_LOG_H

#include <cstdarg>
#include <cstdio>
#include <string.h>
#include "util.h"

class Log
{
public:
	static void printf(const char *fmt, ...);
	static void perror(const char *fmt, ...);
};

#endif
