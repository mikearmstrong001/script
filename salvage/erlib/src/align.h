#ifndef ER_HEADER_ALIGN_H
#define ER_HEADER_ALIGN_H

template <class T>
inline T AlignUp( T v, int a ) {
	return ( v + (a-1) ) & ~(a-1);
}

template <class T>
inline T AlignDown( T v, int a ) {
	return ( v ) & ~(a-1);
}


#endif