#ifndef TESTING_HW_H
#define TESTING_HW_H

#include <vector>
#include "shader.h"
#include "vectormath/cpp/vectormath_aos.h"

struct ModelVert;
struct ModelVertWeight;
struct ModelGeometry;

namespace hw
{

const unsigned int SS_POINT_POINT	= 1<<0;
const unsigned int SS_LINEAR_LINEAR	= 2<<0;
const unsigned int SS_POINT_LINEAR	= 3<<0;
const unsigned int SS_LINEAR_POINT	= 4<<0;
const unsigned int SS_FILTER_MASK	= 0xf;

const unsigned int SS_WRAP_S = 1<<8;
const unsigned int SS_WRAP_T = 1<<9;
const unsigned int SS_WRAP_U = 1<<10;

const unsigned int BLEND_ONE  = 0;
const unsigned int BLEND_ZERO = 1;
const unsigned int BLEND_SRCALPHA  = 2;
const unsigned int BLEND_INVSRCALPHA  = 3;
const unsigned int BLEND_DSTALPHA  = 4;
const unsigned int BLEND_INVDSTALPHA  = 5;
const unsigned int BLEND_SRCCOLOUR  = 6;
const unsigned int BLEND_DSTCOLOUR  = 7;
const unsigned int BLEND_INVSRCCOLOUR  = 8;
const unsigned int BLEND_INVDSTCOLOUR  = 9;

struct DepthStencil;
struct Texture;
struct VertexBuffer;
struct IndexBuffer;
struct VertexProgram;
struct FragmentProgram;
struct Cache;

void Init( int w, int h );
void Deinit();

void CopyScreen();

DepthStencil *GetDefaultDepthStencil();
DepthStencil *CreateDepthStencil( int w, int h );
Texture *CreateRenderTexture2D( int w, int h );

Texture *CreateRenderTexture2D( int w, int h );
Texture *CreateTexture2D( int w, int h, int mips, unsigned char *data );
Texture *CreateCompressedTexture2D( int w, int h, int mips, unsigned char *data, bool alpha );
void ReleaseTexture2D( Texture * );

Texture *CreateCompressedTextureCube( int w, int mips, unsigned char *data[6], bool alpha );
void ReleaseTexture2D( Texture * );

VertexProgram *CreateVertexProgram( const char *text, const char *main, const char *defines, std::vector<VirtualToHwMap> &params, std::vector<VirtualToHwMap> &samplers );
VertexProgram *CreateVertexProgramFromFile( const char *filename, const char *main, const char *defines, std::vector<VirtualToHwMap> &params, std::vector<VirtualToHwMap> &samplers );
void ReleaseVertexProgram( VertexProgram * );

FragmentProgram *CreateFragmentProgram( const char *text, const char *main, const char *defines, std::vector<VirtualToHwMap> &params, std::vector<VirtualToHwMap> &samplers );
FragmentProgram *CreateFragmentProgramFromFile( const char *filename, const char *main, const char *defines, std::vector<VirtualToHwMap> &params, std::vector<VirtualToHwMap> &samplers );
void ReleaseFragmentProgram( FragmentProgram * );

void BindVertexProgram( VertexProgram *prog, const std::vector<VirtualToHwMap> &params, const std::vector<VirtualToHwMap> &samplers );
void BindFragmentProgram( FragmentProgram *prog, const std::vector<VirtualToHwMap> &params, const std::vector<VirtualToHwMap> &samplers );

void DrawIndexed( const ModelVert *verts, const unsigned short *indices, int numVerts, int numIndices );
void DrawGeometry( const ModelGeometry *geom );

void SetDepthWrite( bool enabled );
void SetBlend( unsigned int src, unsigned int dst );
void ColourWrite( bool r, bool g, bool b, bool a );

void SetDefaultRT();
void SetRT( Texture *rt0, DepthStencil *ds );
void ClearRT( bool rt0, unsigned int colour, bool depth, float depthValue );
inline void ClearRT( bool rt0, bool depth )
{
	ClearRT( rt0, 0, depth, 1.f );
}

int GetBackbufferWidth();
int GetBackbufferHeight();

Vectormath::Aos::Matrix4 MakeHWProjection( const Vectormath::Aos::Matrix4 &in );
Vectormath::Aos::Matrix4 MakeHWProjection( float fov, float aspect, float znear, float zfar );

int GetTextureSlot( const char *name );
void SetTextureInSlot( int slot, Texture *t, unsigned int samplerState );
Texture *GetTextureInSlot( int slot );
unsigned int GetSamplerStateInSlot( int slot );

int GetParamSlot( const char *name, int numQuads );
void SetParamInSlot( int slot, const float *v, int numQuads );
void SetParamInSlot( int slot, float f0, float f1, float f2, float f3 );
void SetParamInSlot( int slot, const Vectormath::Aos::Matrix4 &v );
void SetParamInSlot( int slot, const Vectormath::Aos::Transform3 *v, int numTransforms );
float *GetParamInSlot( int slot );

Cache* ResolveCache( unsigned int &input, void *data, int byteSize, bool indexBuffer );
void InvalidateCache( unsigned int &input );
void FreeCache();

};


#endif
