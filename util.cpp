#include <cstddef>
#include <cstdlib>
#include <string.h>
#include <sys/time.h>
#include "util.h"

long microtime()
{
    timeval time;
    gettimeofday(&time, NULL);
    return ((unsigned long long) time.tv_sec * 1000000) + time.tv_usec;
}

bool env_has(const char *env)
{
	return getenv(env) != NULL;
}

bool env_is(const char *env, const char *test)
{
	if (!env_has(env))
	{
		return false;
	}

	return strcmp(getenv(env), test) == 0;
}

double relPositionDistance(const RelPosition &pos1, const RelPosition &pos2)
{
	return sqrt(
		pow(RELPOS_X(pos1) - RELPOS_X(pos2), 2) +
		pow(RELPOS_Y(pos1) - RELPOS_Y(pos2), 2)
	);
}

double speedForRotation(double angle, double time)
{
	return (ROBOT_RADIUS * angle) / (ROTATION_CONST * WHEEL_CIRCUMFERENCE * time);
}
