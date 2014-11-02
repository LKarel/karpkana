#ifndef UTIL_H
#define UTIL_H

#define FORCE_RANGE(x, min, max) do {\
	if ((x) < (min)) { (x) = (min); }\
	if ((x) > (max)) { (x) = (max); }\
} while (0);

long microtime();
bool env_has(const char *env);
bool env_is(const char *env, const char *test);

#endif
