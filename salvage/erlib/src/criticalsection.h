#ifndef ER_HEADER_CRITICALSECTION_H
#define ER_HEADER_CRITICALSECTION_H

#include "erbase.h"

#if defined(ERLIB_OS_WIN)

#include "windows.h"

class CriticalSection {
	CRITICAL_SECTION handle;
public:
	CriticalSection();
	~CriticalSection();
	
	void Enter(void);
	void Leave(void);
};

#endif

#endif

