#ifndef ER_HEADER_TIME_H
#define ER_HEADER_TIME_H

#include "erbase.h"

uint32_t TIME_GetMS();
uint32_t TIME_GetDeltaMS();
void	 TIME_Update();
void	 TIME_Init();

class Timer {
	uint32_t lastTime;
	uint32_t dt;
	bool running;
public:

	Timer() : running(true) {
		Reset();
	}

	void Reset( void ) {
		lastTime = 0;
		dt = 0;
	}

	void Start( void ) {
		running = true;
	}

	void Pause( void ) {
		running = false;
	}

	void UpdateMS( uint32_t indt ) {
		if ( running ) {
			lastTime += indt;
			dt = indt;
		} else {
			dt = 0;
		}
	}

	uint32_t Update() {
		uint32_t delta = TIME_GetDeltaMS();
		UpdateMS( delta );
		return DtMS();
	}


	float Dt( void ) {
		return dt * 0.001f;
	}

	uint32_t DtMS( void ) {
		return dt;
	}
};


#endif