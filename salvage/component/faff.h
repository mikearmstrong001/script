#ifndef TESTING_FAFF_H
#define TESTING_FAFF_H

#define _CRT_SECURE_NO_WARNINGS
#define _USE_MATH_DEFINES
#pragma warning(disable:4996)

//#define __SSE__
#include "vectormath/cpp/vectormath_aos.h"
#include <vector>
#include "stdint.h"


class GameObject;
class WorldDB;

void ParseXML( std::vector< GameObject* > &gos, const char *filename, const Vectormath::Aos::Transform3 &transform, WorldDB *worlddb );

void  FrameAllocInit( int maxBufferSize );
void* FrameAlloc( int size );
void  FrameAllocNext();
void  FrameAllocDestroy();

uint64_t HashedString( const char *str );
uint64_t HashedString( uint64_t id, const char *str );

#endif
