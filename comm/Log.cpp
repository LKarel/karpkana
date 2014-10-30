#include "comm/Log.h"

void Log::printf(const char *fmt, ...)
{
	size_t len = strlen(fmt) + 18;
	char myfmt[len];
	snprintf(myfmt, len, "[%ld] %s\n", microtime() / 1000, fmt);

	va_list args;
	va_start(args, fmt);
	vprintf(myfmt, args);
	va_end(args);
}

void Log::perror(const char *fmt, ...)
{
	size_t len = strlen(fmt) + 32;
	char myfmt[len];
	char errname[len];

	snprintf(myfmt, len, "[%ld] error: %s", microtime() / 1000, fmt);

	va_list args;
	va_start(args, fmt);
	vsnprintf(errname, len, myfmt, args);
	va_end(args);

	::perror(errname);
}
