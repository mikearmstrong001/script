#include "criticalsection.h"


CriticalSection::CriticalSection() {
	InitializeCriticalSection( &handle );
}

CriticalSection::~CriticalSection() {
	DeleteCriticalSection( &handle );
}

void CriticalSection::Enter(void) {
	EnterCriticalSection( &handle );
}

void CriticalSection::Leave(void) {
	LeaveCriticalSection( &handle );
}
