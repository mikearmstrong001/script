#ifndef ER_HEADER_RANDOM_H
#define ER_HEADER_RANDOM_H

#include "erbase.h"

class Random {
	unsigned int low;
	unsigned int high;
public:

	Random( unsigned int seed ) : high(seed), low(0) {
	}

	Random() : high( 250301 ), low(1312313) {
	}

	unsigned int Rand( void ) {
		high = (high << 16) + (high >> 16);

		high += low;

		low += high;

		return high;
	}

	float Randf( void ) {
		float v = Rand() / (float)(0xffffffff);
		return v;
	}

	unsigned int RandCount( int num ) {
		unsigned int r = Rand();
		return (unsigned int) ( ( (uint64_t) r * num) >> 32 );
	}
};

#endif