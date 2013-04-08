#ifndef ER_HEADER_BASE_H
#define ER_HEADER_BASE_H

#include "stdint.h"
#include <string.h>
#include <memory.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdarg.h>
#include <stddef.h>
#include <ctype.h>
#include <io.h>
#include <malloc.h>
#include <sys/stat.h>

#pragma warning( disable : 4201 ) // nonstandard extension used : nameless struct/union

/******************************************************************/

#if defined(_WIN32) || defined(WIN32) || defined(__NT__) || defined(__WIN32__)
	#define ERLIB_OS_WIN
#endif

/******************************************************************/

#if defined(__X86__) || defined(__i386__) || defined(i386) || defined(_M_IX86) || defined(__386__)
	#define ERLIB_CPU_X86
#endif

/******************************************************************/

#if defined(ERLIB_CPU_X86) 
	#define ERLIB_ENDIAN_LITTLE
#else
	#define ERLIB_ENDIAN_BIG
#endif

/******************************************************************/

#include "erassert.h"

// public interface
#define _countarray(x)  (                                           \
  0 * sizeof( reinterpret_cast<const ::Bad_arg_to_COUNTOF*>(x) ) +  \
  0 * sizeof( ::Bad_arg_to_COUNTOF::check_type((x), &(x))      ) +  \
  sizeof(x) / sizeof((x)[0])  )                                  
// implementation details
class Bad_arg_to_COUNTOF
{
public:
   class Is_pointer;  // intentionally incomplete type
   class Is_array {};  
   template<typename T>
   static Is_pointer check_type(const T*, const T* const*);
   static Is_array check_type(const void*, const void*);
};


// same_size_bit_cast casts the bits in memory
//	eg. it's not a value cast
template < typename t_to, typename t_fm >
t_to same_size_bit_cast( const t_fm & from )
{
	STATIC_ASSERT( sizeof(t_to) == sizeof(t_fm) );
	
	union
	{
		t_fm	fm;
		t_to	to;
	} temp;
	
	temp.fm = from;
	return temp.to;
}

extern char *va( const char *fmt, ... );
unsigned int stb_hash_fast(void *p, int len);

#define ERINLINE		_inline
#define ERFORCEINLINE	__forceinline
#define ERTLS			__declspec( thread )


template <class T>
ERFORCEINLINE const T & Min( const T &a, const T &b ) {
	return (a<b) ? a : b;
}

template <class T>
ERFORCEINLINE const T &Max( const T &a, const T &b ) {
	return (a>b) ? a : b;
}

template< typename To, typename From >
ERFORCEINLINE To union_cast(From from)
{
	STATIC_ASSERT(sizeof(To) == sizeof(From));
	union
	{
		From f;
		To t;
	} const u = { from };

	return u.t;
}

#endif

