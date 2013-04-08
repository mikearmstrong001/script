#ifndef ER_HEADER_ENDIAN_H
#define ER_HEADER_ENDIAN_H

#include "erbase.h"

ERINLINE void ENDIAN_Swap( void *ptr, int size ) {
	int sd2 = size / 2;
	uint8_t *c = (uint8_t *)ptr;
	for (int i=0; i<sd2; i++) {
		uint8_t swap = c[i];
		c[i] = c[size-i-1];
		c[size-i-1] = swap;
	}
}

ERINLINE void ENDIAN_Swap( void *dst, void *src, int size ) {
	uint8_t *s = (uint8_t *)src;
	uint8_t *d = (uint8_t *)dst;
	for (int i=0; i<size; i++) {
		d[size-i-1] = s[i];
	}
}

template <class T>
ERINLINE void ENDIAN_Swap( T *v ) {
	ENDIAN_Swap( v, sizeof(T) );
}

template <class T>
ERINLINE void ENDIAN_Swap( T* d, T *s ) {
	ENDIAN_Swap( d, s, sizeof(T) );
}

template <class T>
ERINLINE void ENDIAN_Noswap( T* d, T *s ) {
	memcpy( d, s, sizeof(T) );
}


#if defined( ERLIB_ENDIAN_LITTLE )
#define ENDIAN_FromLE(x) 
#define ENDIAN_CopyFromLE(s,d) ENDIAN_Noswap(s,d)
#define ENDIAN_FromBE(x) ENDIAN_Swap( (x) );
#define ENDIAN_CopyFromBE(s,d) ENDIAN_Swap(s,d);
#elif defined( ERLIB_ENDIAN_BIG )
#define ENDIAN_FromBE(x) 
#define ENDIAN_CopyFromBE(s,d) ENDIAN_Noswap(s,d)
#define ENDIAN_FromLE(x) ENDIAN_Swap( (x) );
#define ENDIAN_CopyFromLE(s,d) ENDIAN_Swap(s,d);
#endif

#endif
