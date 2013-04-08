#ifndef TESTING_INPUT_H
#define TESTING_INPUT_H

#include "faff.h"

namespace in
{

enum
{
	KEY_NOPRESS = 0,

	KEY_A,
	KEY_B,
	KEY_C,
	KEY_D,
	KEY_E,
	KEY_F,
	KEY_G,
	KEY_H,
	KEY_I,
	KEY_J,
	KEY_K,
	KEY_L,
	KEY_M,
	KEY_N,
	KEY_O,
	KEY_P,
	KEY_Q,
	KEY_R,
	KEY_S,
	KEY_T,
	KEY_U,
	KEY_V,
	KEY_W,
	KEY_X,
	KEY_Y,
	KEY_Z,
	KEY_UP,
	KEY_DOWN,
	KEY_LEFT,
	KEY_RIGHT,

	KEY_MAX,
};

int  Translate( const char *key );

void SetKeyState( int k, int state );
int  GetKeyState( int k );

void Bind( int k, uint32_t b );
void Unbind( int k );
int  GetBindState( uint32_t b );

void ResetKeys();

};

#endif
