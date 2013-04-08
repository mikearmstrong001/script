#include "time.h"

#if defined( ERLIB_OS_WIN ) 
#include        "windows.h"
#pragma comment(lib,"winmm.lib")

static bool setPeriod = false;
static uint32_t s_dt = 0;
static uint32_t s_lastMS = 0;

uint32_t TIME_GetMS() {
	if ( !setPeriod ) {
		timeBeginPeriod(1);
		setPeriod = true;
	}

	return timeGetTime();
}
#endif

uint32_t TIME_GetDeltaMS() {
	return s_dt;
}

void TIME_Init() {
	TIME_Update();
	TIME_Update();
}

void TIME_Update() {
	uint32_t ms = TIME_GetMS();

	if ( ms < 0x7ffffff && s_lastMS >= 0x7ffffff ) {
		s_dt = ms + (0xffffffff - s_lastMS);
	} else {
		s_dt = ms - s_lastMS;
	}
	
	s_lastMS = ms;
}



