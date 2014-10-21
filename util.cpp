#include <cstddef>
#include <sys/time.h>
#include "util.h"

long microtime()
{
    timeval time;
    gettimeofday(&time, NULL);
    return ((unsigned long long) time.tv_sec * 1000000) + time.tv_usec;
}
