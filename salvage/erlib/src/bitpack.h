#ifndef ER_HEADER_BITPACK_H
#define ER_HEADER_BITPACK_H

#include "erbase.h"
#include "vector.h"

class BitPack {

	Vector< unsigned char > bits;
	int numBits;

public:

	BitPack() : numBits(0) {
	};

	int WriteBit( int v ) {
		bits.EnsureAllocated( (numBits + 8) / 8 );
		
		int idx = numBits / 8;
		int bit = numBits & 7;

		if ( v ) {
			bits[idx] |= ~( 1 << bit );
		} else {
			bits[idx] &= ~( 1 << bit );
		}

		numBits ++;
	}

	void WriteBits( void *ptr, int num ) {
		unsigned char *c = (unsigned char*)ptr;
		for (int i=0; i<num; i++) {
			int idx = i / 8;
			int bit = i & 7;

			WriteBit( c[idx] & ( 1 << bit ) );
		}
	}

};

class BitPackWriterArray {

	uint8_t *bits;
	int numBits;

public:

	BitPackWriterArray( uint8_t *b ) : bits(b), numBits(0) {
	};

	void WriteBit( int v ) {
		
		int idx = numBits / 8;
		int bit = numBits & 7;

		if ( v ) {
			bits[idx] |= ~( 1 << bit );
		} else {
			bits[idx] &= ~( 1 << bit );
		}

		numBits ++;
	}

	void WriteBits( void *ptr, int num ) {
		unsigned char *c = (unsigned char*)ptr;
		for (int i=0; i<num; i++) {
			int idx = i / 8;
			int bit = i & 7;

			WriteBit( c[idx] & ( 1 << bit ) );
		}
	}

	void WriteUShort( uint16_t a ) {
		WriteBits( &a, sizeof(a)*8 );
	}

	int ByteCount() {
		return (numBits+7)/8;
	}
};

class BitPackReaderArray {

	uint8_t *bits;
	int numBits;

public:

	BitPackReaderArray( uint8_t *b ) : bits(b), numBits(0) {
	};

	int ReadBit() {
		
		int idx = numBits / 8;
		int bit = numBits & 7;
		numBits ++;

		return (bits[idx] & ( 1 << bit )) ? 1 : 0;
	}

	void ReadBits( void *ptr, int num ) {
		unsigned char *c = (unsigned char*)ptr;
		for (int i=0; i<num; i++) {
			int idx = i / 8;
			int bit = i & 7;

			c[idx] |= ( ReadBit() << bit );
		}
	}
};


#endif