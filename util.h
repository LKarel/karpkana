#ifndef UTIL_H
#define UTIL_H

#include <cmath>

#define PI 3.14159265359
#define LIMIT(x, min, max) ((x) < (min) ? (min) : ((x) > (max) ? (max) : (x)))
#define IN_RANGE(x, min, max) ((x) >= (min) && (x) <= (max))

#define RELPOS_X(pos) ((pos).radius * sin((pos).angle))
#define RELPOS_Y(pos) ((pos).radius * cos((pos).angle))

typedef struct {
	double x;
	double y;
} Point2d;

typedef struct {
	double radius;
	double angle;
} RelPosition;

long microtime();
bool env_has(const char *env);
bool env_is(const char *env, const char *test);
double relPositionDistance(const RelPosition &pos1, const RelPosition &pos2);

#endif
