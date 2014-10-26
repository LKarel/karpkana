#ifndef MM_H
#define MM_H

#include <mutex>

#define MM_INIT(x) do {\
	(x)->_mm.mtx = new std::mutex();\
	(x)->_mm.refc = 1;\
} while(0);

#define MM_INC(x) do {\
	std::lock_guard<std::mutex> lock(*(x)->_mm.mtx);\
	(x)->_mm.refc++;\
} while(0);

#define MM_DEC(x) do {\
	(x)->_mm.mtx->lock();\
	if (--((x)->_mm.refc) == 0) {\
		(x)->_mm.mtx->unlock();\
		delete (x)->_mm.mtx;\
		delete frame;\
	} else {\
		(x)->_mm.mtx->unlock();\
	}\
} while(0);

struct mm_refdata
{
	std::mutex *mtx;
	int refc;
};

#endif
