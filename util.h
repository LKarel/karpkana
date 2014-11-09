#ifndef UTIL_H
#define UTIL_H

#define LIMIT(x, min, max) ((x) < (min) ? (min) : ((x) > (max) ? (max) : (x)))

long microtime();
bool env_has(const char *env);
bool env_is(const char *env, const char *test);

#endif
