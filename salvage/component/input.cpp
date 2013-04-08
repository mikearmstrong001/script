#include "dxut.h"
#include "input.h"
#include <map>

namespace in
{

static int keys[ KEY_MAX ] = {0};
static uint32_t keyToBind[ KEY_MAX ] = {0};
static std::map< uint32_t, int > binds;

static const char *keyNames[ KEY_MAX ] =
{
	"", //KEY_NOPRESS = 0,

	"A", //KEY_A,
	"B", //KEY_B,
	"C", //KEY_C,
	"D", //KEY_D,
	"E", //KEY_E,
	"F", //KEY_F,
	"G", //KEY_G,
	"H", //KEY_H,
	"I", //KEY_I,
	"J", //KEY_J,
	"K", //KEY_K,
	"L", //KEY_L,
	"M", //KEY_M,
	"N", //KEY_N,
	"O", //KEY_O,
	"P", //KEY_P,
	"Q", //KEY_Q,
	"R", //KEY_R,
	"S", //KEY_S,
	"T", //KEY_T,
	"U", //KEY_U,
	"V", //KEY_V,
	"W", //KEY_W,
	"X", //KEY_X,
	"Y", //KEY_Y,
	"Z", //KEY_Z,
	"Up", //KEY_UP,
	"Down", //KEY_DOWN,
	"Left", //KEY_LEFT,
	"Right", //KEY_RIGHT,

};

int  Translate( const char *key )
{
	for (int i=KEY_NOPRESS+1; i<KEY_MAX; i++)
	{
		if ( stricmp( key, keyNames[i] ) == 0 )
		{
			return i;
		}
	}
	return KEY_NOPRESS;
}


void SetKeyState( int k, int state )
{
	keys[k] = state;
	binds[ keyToBind[k] ] = state;
}

int  GetKeyState( int k )
{
	return keys[k];
}

void Bind( int k, uint32_t b )
{
	keyToBind[k] = b;
}

void Unbind( uint32_t k )
{
	keyToBind[k] = 0;
}

int  GetBindState( uint32_t b )
{
	return binds[b];
}

void ResetKeys()
{
	for (int i=0; i<KEY_MAX; i++)
	{
		SetKeyState( i, 0 );
	}
}


};