#include "hw.h"
#include <map>
#include <string>
#include "assert.h"

namespace hw
{

const int MAX_PARAMS = 1024;
const int MAX_TEXTURES = 128;

static Texture *Textures[ MAX_TEXTURES ] = { 0 };
static unsigned int SamplerState[ MAX_TEXTURES ] = { 0 };
static int numTextures = 0;
static std::map< std::string, int > nameToTextureSlot;

static float Params[ MAX_PARAMS ][4] = { 0.f };
static int numParams = 0;
static std::map< std::string, int > nameToParamSlot;
static std::map< std::string, int > nameToParamQuads;

int GetTextureSlot( const char *name )
{
	std::map< std::string, int >::iterator f = nameToTextureSlot.find( name );
	if ( f != nameToTextureSlot.end() )
	{
		return f->second;
	}

	int r = numTextures;
	numTextures++;
	nameToTextureSlot[name] = r;
	return r;
}

void SetTextureInSlot( int slot, Texture *t, unsigned int samplerState )
{
	Textures[slot] = t;
	SamplerState[slot] = samplerState;
}

Texture *GetTextureInSlot( int slot )
{
	return Textures[slot];
}

unsigned int GetSamplerStateInSlot( int slot )
{
	return SamplerState[ slot ];
}

int GetParamSlot( const char *name, int numQuads )
{
	std::map< std::string, int >::iterator f = nameToParamSlot.find( name );
	if ( f != nameToParamSlot.end() )
	{
		assert( numQuads <= nameToParamQuads[name] );
		return f->second;
	}

	int r = numParams;
	numParams+=numQuads;
	nameToParamSlot[name] = r;
	nameToParamQuads[name] = numQuads;
	return r;
}


void SetParamInSlot( int slot, const float *v, int numQuads )
{
	memcpy( Params[slot], v, numQuads*4*sizeof(float) );
}

void SetParamInSlot( int slot, float f0, float f1, float f2, float f3 )
{
	Params[slot][0] = f0;
	Params[slot][1] = f1;
	Params[slot][2] = f2;
	Params[slot][3] = f3;
}


void SetParamInSlot( int slot, const Vectormath::Aos::Matrix4 &v )
{
	Params[slot+0][0] = v[0][0];
	Params[slot+0][1] = v[0][1];
	Params[slot+0][2] = v[0][2];
	Params[slot+0][3] = v[0][3];
	Params[slot+1][0] = v[1][0];
	Params[slot+1][1] = v[1][1];
	Params[slot+1][2] = v[1][2];
	Params[slot+1][3] = v[1][3];
	Params[slot+2][0] = v[2][0];
	Params[slot+2][1] = v[2][1];
	Params[slot+2][2] = v[2][2];
	Params[slot+2][3] = v[2][3];
	Params[slot+3][0] = v[3][0];
	Params[slot+3][1] = v[3][1];
	Params[slot+3][2] = v[3][2];
	Params[slot+3][3] = v[3][3];
}

void SetParamInSlot( int slot, const Vectormath::Aos::Transform3 *v, int numTransforms )
{
	for (int i=0; i<numTransforms; i++)
	{
		Params[slot+0][0] = v[i][0][0];
		Params[slot+0][1] = v[i][1][0];
		Params[slot+0][2] = v[i][2][0];
		Params[slot+0][3] = v[i][3][0];
		Params[slot+1][0] = v[i][0][1];
		Params[slot+1][1] = v[i][1][1];
		Params[slot+1][2] = v[i][2][1];
		Params[slot+1][3] = v[i][3][1];
		Params[slot+2][0] = v[i][0][2];
		Params[slot+2][1] = v[i][1][2];
		Params[slot+2][2] = v[i][2][2];
		Params[slot+2][3] = v[i][3][2];
		slot += 3;
	}
}



float *GetParamInSlot( int slot )
{
	return Params[slot];
}

Vectormath::Aos::Matrix4 MakeHWProjection( const Vectormath::Aos::Matrix4 &in )
{
	Vectormath::Aos::Matrix4 translate( 
		Vectormath::Aos::Vector4( 1.f, 0.f, 0.f, 0.f ),
		Vectormath::Aos::Vector4( 0.f, 1.f, 0.f, 0.f ),
		Vectormath::Aos::Vector4( 0.f, 0.f, 1.f, 0.f ),
		Vectormath::Aos::Vector4( 0.f, 0.f, 1.f, 1.f ) );
	Vectormath::Aos::Matrix4 scale( 
		Vectormath::Aos::Vector4( 1.f, 0.f, 0.f, 0.f ),
		Vectormath::Aos::Vector4( 0.f, 1.f, 0.f, 0.f ),
		Vectormath::Aos::Vector4( 0.f, 0.f, 0.5f, 0.f ),
		Vectormath::Aos::Vector4( 0.f, 0.f, 0.f, 0.5f ) );

	Vectormath::Aos::Matrix4 out = scale * translate * in;
	return out;
}



};
