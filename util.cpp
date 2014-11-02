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
