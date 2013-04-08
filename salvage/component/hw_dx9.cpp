#include "dxut.h"
#include "hw.h"
#include "model_structs.h"
#include "file_system.h"

namespace hw
{

static LPDIRECT3DSURFACE9 defaultRT0 = NULL;
static LPDIRECT3DSURFACE9 defaultDS = NULL;
static hw::Texture *screenCopy = NULL;
static LPDIRECT3DVERTEXDECLARATION9 g_VertexDecl = NULL;
static LPDIRECT3DVERTEXDECLARATION9 g_WeightedVertexDecl = NULL;
static int g_width = 640;
static int g_height = 480;


static D3DVERTEXELEMENT9 g_VertexDesc[] = 
{
	{ 0, offsetof( ModelVert, p ), D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
	{ 0, offsetof( ModelVert, n ), D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0 },
	{ 0, offsetof( ModelVert, t ), D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
	{ 0, offsetof( ModelVert, tan ), D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 1 },
	{ 0, offsetof( ModelVert, t2 ), D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 2 },
	{ 0, offsetof( ModelVert, c ), D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 0 },
	D3DDECL_END()
};


static D3DVERTEXELEMENT9 g_WeightedVertexDesc[] = 
{
	{ 0, offsetof( ModelVert, p ), D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
	{ 0, offsetof( ModelVert, n ), D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0 },
	{ 0, offsetof( ModelVert, t ), D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
	{ 0, offsetof( ModelVert, tan ), D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 1 },
	{ 0, offsetof( ModelVert, t2 ), D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 2 },
	{ 0, offsetof( ModelVert, c ), D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 0 },
	{ 1, offsetof( ModelVertWeight, Index ), D3DDECLTYPE_UBYTE4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 3 },
	{ 1, offsetof( ModelVertWeight, Weight ), D3DDECLTYPE_UBYTE4N, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 4 },
	D3DDECL_END()
};

class PHYSFS_IncludeIFace : public ID3DXInclude
{
public:
    STDMETHOD(Open)(THIS_ D3DXINCLUDE_TYPE IncludeType, LPCSTR pFileName, LPCVOID pParentData, LPCVOID *ppData, UINT *pBytes)
	{
		*pBytes = PHYSFS_readFile( pFileName, (void**)ppData );
		if ( *ppData )
		{
			return S_OK;
		} else
		{
			return !S_OK;
		}
	}

    STDMETHOD(Close)(THIS_ LPCVOID pData)
	{
		free( (void*)pData );
		return S_OK;
	}
};


void Init( int w, int h )
{
	IDirect3DDevice9* dev = DXUTGetD3D9Device();

	dev->GetRenderTarget( 0, &defaultRT0 );
	dev->GetDepthStencilSurface( &defaultDS );
	screenCopy = hw::CreateRenderTexture2D( hw::GetBackbufferWidth(), hw::GetBackbufferHeight() );

	hw::SetTextureInSlot( hw::GetTextureSlot( "screenBuffer" ), screenCopy, SS_LINEAR_POINT );

	dev->CreateVertexDeclaration( g_VertexDesc, &g_VertexDecl );
	dev->CreateVertexDeclaration( g_WeightedVertexDesc, &g_WeightedVertexDecl );

	g_width = w;
	g_height = h;
}

void Deinit()
{
	FreeCache();
	g_VertexDecl->Release();
	g_VertexDecl = NULL;
	g_WeightedVertexDecl->Release();
	g_WeightedVertexDecl = NULL;
	defaultRT0->Release();
	defaultRT0 = NULL;
	defaultDS->Release();
	defaultDS = NULL;
	LPDIRECT3DTEXTURE9 screenTex = (LPDIRECT3DTEXTURE9)screenCopy;
	screenTex->Release();
	screenCopy = NULL;
}

void CopyScreen()
{
	IDirect3DDevice9* dev = DXUTGetD3D9Device();
	LPDIRECT3DTEXTURE9 tex = (LPDIRECT3DTEXTURE9)screenCopy;
	LPDIRECT3DSURFACE9 surf;
	tex->GetSurfaceLevel( 0, &surf );
	HRESULT hr = dev->StretchRect( defaultRT0, NULL, surf, NULL, D3DTEXF_NONE );
	//HRESULT hr = dev->GetRenderTargetData( defaultDS, surf );
	surf->Release();
}

DepthStencil *GetDefaultDepthStencil()
{
	return (DepthStencil*)defaultDS;
}


DepthStencil *CreateDepthStencil( int w, int h )
{
	IDirect3DDevice9* dev = DXUTGetD3D9Device();

	LPDIRECT3DSURFACE9 surf = NULL;
	HRESULT hr = dev->CreateDepthStencilSurface( w, h, D3DFMT_D24X8, D3DMULTISAMPLE_NONE, 0, TRUE, &surf, NULL );
	if ( hr == S_OK )
	{
		return (DepthStencil*)surf;
	} else
	{
		return NULL;
	}
}

Texture *CreateRenderTexture2D( int w, int h )
{
	IDirect3DDevice9* dev = DXUTGetD3D9Device();

	LPDIRECT3DTEXTURE9 tex = NULL;
	HRESULT hr = dev->CreateTexture( w, h, 1, D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &tex, NULL );
	if ( hr == S_OK )
	{
		return (Texture*)tex;
	} else
	{
		return NULL;
	}
}

Texture *CreateTexture2D( int w, int h, int mips, unsigned char *data )
{

	return NULL;
}

Texture *CreateCompressedTexture2D( int w, int h, int mips, unsigned char *data, bool alpha )
{
	IDirect3DDevice9* dev = DXUTGetD3D9Device();

	LPDIRECT3DTEXTURE9 tex = NULL;
	HRESULT hr = dev->CreateTexture( w, h, mips, 0, alpha ? D3DFMT_DXT5 : D3DFMT_DXT1, D3DPOOL_MANAGED, &tex, NULL );
	if ( hr == S_OK )
	{
		int numMips = mips;
		unsigned char *curPtr = (unsigned char*)data;
		int mw = w;
		int mh = h;
		for (int i=0; i<numMips; i++)
		{
			int mipSize = ((mw+3)/4) * ((mh+3)/4) * (alpha?16:8);

			D3DLOCKED_RECT rect;
			tex->LockRect( i, &rect, NULL, 0 );

			memcpy( rect.pBits, curPtr, mipSize );
			
			tex->UnlockRect( i );

			mw = (mw+1)/2;
			mh = (mh+1)/2;
			curPtr += mipSize;
		}
	}

	return (Texture*)tex;
}

Texture *CreateCompressedTextureCube( int w, int mips, unsigned char *data[6], bool alpha )
{
	IDirect3DDevice9* dev = DXUTGetD3D9Device();

	LPDIRECT3DCUBETEXTURE9 tex = NULL;
	HRESULT hr = dev->CreateCubeTexture( w, mips, 0, alpha ? D3DFMT_DXT5 : D3DFMT_DXT1, D3DPOOL_MANAGED, &tex, NULL );
	if ( hr == S_OK )
	{
		D3DCUBEMAP_FACES facetype [] = 
		{
			D3DCUBEMAP_FACE_POSITIVE_X,
			D3DCUBEMAP_FACE_NEGATIVE_X,
			D3DCUBEMAP_FACE_POSITIVE_Y,
			D3DCUBEMAP_FACE_NEGATIVE_Y,
			D3DCUBEMAP_FACE_POSITIVE_Z,
			D3DCUBEMAP_FACE_NEGATIVE_Z,
		};

		for ( int f=0; f<6; f++ )
		{
			int numMips = mips;
			unsigned char *curPtr = data[f];
			int mw = w;
			int mh = w;
			for (int i=0; i<numMips; i++)
			{
				int mipSize = ((mw+3)/4) * ((mh+3)/4) * (alpha?16:8);

				D3DLOCKED_RECT rect;
				tex->LockRect( facetype[f], i, &rect, NULL, 0 );

				memcpy( rect.pBits, curPtr, mipSize );
				
				tex->UnlockRect( facetype[f], i );

				mw = (mw+1)/2;
				mh = (mh+1)/2;
				curPtr += mipSize;
			}
		}
	}

	return (Texture*)tex;
}


void ReleaseTexture2D( Texture *tex )
{
	if ( tex == NULL )
	{
		return;
	}
	LPDIRECT3DBASETEXTURE9 d3dtex = (LPDIRECT3DBASETEXTURE9)tex;
	d3dtex->Release();
}


static void GenerateConstants( LPD3DXCONSTANTTABLE table, std::vector<VirtualToHwMap> &params, std::vector<VirtualToHwMap> &samplers )
{
	if ( table == NULL )
	{
		return;
	}
	D3DXCONSTANTTABLE_DESC desc;
	table->GetDesc( &desc );

	for (UINT i=0; i<desc.Constants; i++)
	{
		D3DXHANDLE h = table->GetConstant( NULL, i );

		D3DXCONSTANT_DESC constDesc;
		UINT count;
		HRESULT hr = table->GetConstantDesc( h, &constDesc, &count );

		if ( hr == S_OK )
		{
			if ( constDesc.RegisterSet == D3DXRS_SAMPLER )
			{
				VirtualToHwMap map;
				map.v = hw::GetTextureSlot( constDesc.Name );
				map.h = constDesc.RegisterIndex;
				map.num = 1;
				samplers.push_back( map );
			} else
			if ( constDesc.RegisterSet == D3DXRS_FLOAT4 )
			{
				VirtualToHwMap map;
				map.v = hw::GetParamSlot( constDesc.Name, constDesc.Rows * constDesc.Elements );
				map.h = constDesc.RegisterIndex;
				map.num = constDesc.RegisterCount;
				params.push_back( map );
			}
		}
	}
}

VertexProgram *CreateVertexProgramFromFile( const char *filename, const char *main, const char *defines, std::vector<VirtualToHwMap> &params, std::vector<VirtualToHwMap> &samplers )
{
	char *text;
	int len = PHYSFS_readFile( filename, (void**)&text );
	if ( len <= 0 )
	{
		return NULL;
	}

	D3DXMACRO macros[2] = { {NULL, NULL}, {NULL, NULL} };
	if ( defines )
	{
		macros[0].Name = defines;
		macros[0].Definition = "1";
	}

	PHYSFS_IncludeIFace includeIface;
	LPD3DXCONSTANTTABLE table = NULL;
	LPD3DXBUFFER vertexProgram = NULL, vertexErrors = NULL;
	HRESULT vertexHR = D3DXCompileShader( text, len, macros, &includeIface, "vpmain", "vs_3_0", D3DXSHADER_DEBUG|D3DXSHADER_SKIPOPTIMIZATION, &vertexProgram, &vertexErrors, &table );
	if ( vertexErrors )
	{
		char *text = (char*)vertexErrors->GetBufferPointer();
		OutputDebugStringA( text );
		OutputDebugStringA( "\n" );
		vertexErrors->Release();
		vertexErrors = NULL;
	}
	free( text );
	if ( vertexHR != S_OK )
	{
		if ( table )
		{
			table->Release();
		}
		return NULL;
	}

	LPDIRECT3DVERTEXSHADER9 vertexShader = NULL;
	IDirect3DDevice9* dev = DXUTGetD3D9Device();
	dev->CreateVertexShader( (DWORD*)vertexProgram->GetBufferPointer(), &vertexShader );

	GenerateConstants( table, params, samplers );
	if ( table )
	{
		table->Release();
	}
	if ( vertexProgram )
	{
		vertexProgram->Release();
	}

	return (VertexProgram*)vertexShader;
}

VertexProgram *CreateVertexProgram( const char *text, const char *main, const char *defines, std::vector<VirtualToHwMap> &params, std::vector<VirtualToHwMap> &samplers )
{
	PHYSFS_IncludeIFace includeIface;
	LPD3DXCONSTANTTABLE table = NULL;
	LPD3DXBUFFER vertexProgram = NULL, vertexErrors = NULL;
	D3DXMACRO macros[2] = { {NULL, NULL}, {NULL, NULL} };
	if ( defines )
	{
		macros[0].Name = defines;
		macros[0].Definition = "1";
	}

	HRESULT vertexHR = D3DXCompileShader( text, strlen(text), macros, &includeIface, "vpmain", "vs_3_0", D3DXSHADER_DEBUG|D3DXSHADER_SKIPOPTIMIZATION, &vertexProgram, &vertexErrors, &table );
	if ( vertexErrors )
	{
		char *text = (char*)vertexErrors->GetBufferPointer();
		OutputDebugStringA( text );
		OutputDebugStringA( "\n" );
		vertexErrors->Release();
		vertexErrors = NULL;
	}
	if ( vertexHR != S_OK )
	{
		if ( table )
		{
			table->Release();
		}
		return NULL;
	}

	LPDIRECT3DVERTEXSHADER9 vertexShader = NULL;
	IDirect3DDevice9* dev = DXUTGetD3D9Device();
	dev->CreateVertexShader( (DWORD*)vertexProgram->GetBufferPointer(), &vertexShader );

	GenerateConstants( table, params, samplers );
	if ( table )
	{
		table->Release();
	}
	if ( vertexProgram )
	{
		vertexProgram->Release();
	}

	return (VertexProgram*)vertexShader;
}

void ReleaseVertexProgram( VertexProgram *vp )
{
	if ( vp == NULL )
	{
		return;
	}

	LPDIRECT3DVERTEXSHADER9 d3dvp = (LPDIRECT3DVERTEXSHADER9)vp;
	d3dvp->Release();
}


FragmentProgram *CreateFragmentProgramFromFile( const char *filename, const char *main, const char *defines, std::vector<VirtualToHwMap> &params, std::vector<VirtualToHwMap> &samplers )
{
	char *text;
	int len = PHYSFS_readFile( filename, (void**)&text );
	if ( len <= 0 )
	{
		return NULL;
	}

	D3DXMACRO macros[2] = { {NULL, NULL}, {NULL, NULL} };
	if ( defines )
	{
		macros[0].Name = defines;
		macros[0].Definition = "1";
	}

	PHYSFS_IncludeIFace includeIface;
	LPD3DXCONSTANTTABLE table = NULL;
	LPD3DXBUFFER fragmentProgram = NULL, fragmentErrors = NULL;
	HRESULT fragmentHR = D3DXCompileShader( text, len, macros, &includeIface, "fpmain", "ps_3_0", D3DXSHADER_DEBUG|D3DXSHADER_SKIPOPTIMIZATION, &fragmentProgram, &fragmentErrors, &table );
	if ( fragmentErrors )
	{
		char *text = (char*)fragmentErrors->GetBufferPointer();
		OutputDebugStringA( text );
		OutputDebugStringA( "\n" );
		fragmentErrors->Release();
		fragmentErrors = NULL;
	}
	free( text );
	if ( fragmentHR != S_OK )
	{
		if ( table )
		{
			table->Release();
		}
		return NULL;
	}
	LPDIRECT3DPIXELSHADER9 fragmentShader = NULL;
	IDirect3DDevice9* dev = DXUTGetD3D9Device();
	dev->CreatePixelShader( (DWORD*)fragmentProgram->GetBufferPointer(), &fragmentShader );

	GenerateConstants( table, params, samplers );
	if ( table )
	{
		table->Release();
	}
	if ( fragmentProgram )
	{
		fragmentProgram->Release();
	}

	return (FragmentProgram*)fragmentShader;
}

FragmentProgram *CreateFragmentProgram( const char *text, const char *main, const char *defines, std::vector<VirtualToHwMap> &params, std::vector<VirtualToHwMap> &samplers )
{
	D3DXMACRO macros[2] = { {NULL, NULL}, {NULL, NULL} };
	if ( defines )
	{
		macros[0].Name = defines;
		macros[0].Definition = "1";
	}

	PHYSFS_IncludeIFace includeIface;
	LPD3DXCONSTANTTABLE table = NULL;
	LPD3DXBUFFER fragmentProgram = NULL, fragmentErrors = NULL;
	HRESULT fragmentHR = D3DXCompileShader( text, strlen(text), macros, &includeIface, "fpmain", "ps_3_0", D3DXSHADER_DEBUG|D3DXSHADER_SKIPOPTIMIZATION, &fragmentProgram, &fragmentErrors, &table );
	if ( fragmentErrors )
	{
		char *text = (char*)fragmentErrors->GetBufferPointer();
		OutputDebugStringA( text );
		OutputDebugStringA( "\n" );
		fragmentErrors->Release();
		fragmentErrors = NULL;
	}
	if ( fragmentHR != S_OK )
	{
		if ( table )
		{
			table->Release();
		}
		return NULL;
	}
	LPDIRECT3DPIXELSHADER9 fragmentShader = NULL;
	IDirect3DDevice9* dev = DXUTGetD3D9Device();
	dev->CreatePixelShader( (DWORD*)fragmentProgram->GetBufferPointer(), &fragmentShader );

	GenerateConstants( table, params, samplers );
	if ( table )
	{
		table->Release();
	}
	if ( fragmentProgram )
	{
		fragmentProgram->Release();
	}

	return (FragmentProgram*)fragmentShader;
}

void ReleaseFragmentProgram( FragmentProgram *fp )
{
	if ( fp == NULL )
	{
		return;
	}

	LPDIRECT3DPIXELSHADER9 d3dfp = (LPDIRECT3DPIXELSHADER9)fp;
	d3dfp->Release();
}


void BindVertexProgram( VertexProgram *prog, const std::vector<VirtualToHwMap> &params, const std::vector<VirtualToHwMap> &samplers )
{
	IDirect3DDevice9* dev = DXUTGetD3D9Device();
	dev->SetVertexShader( (IDirect3DVertexShader9*)prog );
	for (unsigned int i=0; i<params.size(); i++)
	{
		dev->SetVertexShaderConstantF( params[i].h, GetParamInSlot( params[i].v ), params[i].num );
	}
	for (unsigned int i=0; i<samplers.size(); i++)
	{
		dev->SetTexture( D3DVERTEXTEXTURESAMPLER0+samplers[i].h, (IDirect3DBaseTexture9*)GetTextureInSlot( samplers[i].v ) );

		unsigned int ss = GetSamplerStateInSlot( samplers[i].v );
		unsigned int filter = ss & SS_FILTER_MASK;
		unsigned int address = ss;

		switch ( filter )
		{
		case SS_POINT_POINT:
			dev->SetSamplerState( D3DVERTEXTEXTURESAMPLER0+samplers[i].h, D3DSAMP_MAGFILTER, D3DTEXF_POINT );
			dev->SetSamplerState( D3DVERTEXTEXTURESAMPLER0+samplers[i].h, D3DSAMP_MINFILTER, D3DTEXF_POINT );
			dev->SetSamplerState( D3DVERTEXTEXTURESAMPLER0+samplers[i].h, D3DSAMP_MIPFILTER, D3DTEXF_POINT );
			break;
		case SS_POINT_LINEAR:
			dev->SetSamplerState( D3DVERTEXTEXTURESAMPLER0+samplers[i].h, D3DSAMP_MAGFILTER, D3DTEXF_POINT );
			dev->SetSamplerState( D3DVERTEXTEXTURESAMPLER0+samplers[i].h, D3DSAMP_MINFILTER, D3DTEXF_POINT );
			dev->SetSamplerState( D3DVERTEXTEXTURESAMPLER0+samplers[i].h, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR );
			break;
		case SS_LINEAR_POINT:
			dev->SetSamplerState( D3DVERTEXTEXTURESAMPLER0+samplers[i].h, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
			dev->SetSamplerState( D3DVERTEXTEXTURESAMPLER0+samplers[i].h, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
			dev->SetSamplerState( D3DVERTEXTEXTURESAMPLER0+samplers[i].h, D3DSAMP_MIPFILTER, D3DTEXF_POINT );
			break;
		case SS_LINEAR_LINEAR:
		default:
			dev->SetSamplerState( D3DVERTEXTEXTURESAMPLER0+samplers[i].h, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
			dev->SetSamplerState( D3DVERTEXTEXTURESAMPLER0+samplers[i].h, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
			dev->SetSamplerState( D3DVERTEXTEXTURESAMPLER0+samplers[i].h, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR );
			break;
		};
		
		dev->SetSamplerState( D3DVERTEXTEXTURESAMPLER0+samplers[i].h, D3DSAMP_ADDRESSU, (address & SS_WRAP_S) ? D3DTADDRESS_WRAP : D3DTADDRESS_CLAMP );
		dev->SetSamplerState( D3DVERTEXTEXTURESAMPLER0+samplers[i].h, D3DSAMP_ADDRESSV, (address & SS_WRAP_T) ? D3DTADDRESS_WRAP : D3DTADDRESS_CLAMP );
		dev->SetSamplerState( D3DVERTEXTEXTURESAMPLER0+samplers[i].h, D3DSAMP_ADDRESSW, (address & SS_WRAP_U) ? D3DTADDRESS_WRAP : D3DTADDRESS_CLAMP );
	}
}

void BindFragmentProgram( FragmentProgram *prog, const std::vector<VirtualToHwMap> &params, const std::vector<VirtualToHwMap> &samplers )
{
	IDirect3DDevice9* dev = DXUTGetD3D9Device();
	dev->SetPixelShader( (IDirect3DPixelShader9*)prog );
	for (unsigned int i=0; i<params.size(); i++)
	{
		dev->SetPixelShaderConstantF( params[i].h, GetParamInSlot( params[i].v ), params[i].num );
	}
	for (unsigned int i=0; i<samplers.size(); i++)
	{
		dev->SetTexture( samplers[i].h, (IDirect3DBaseTexture9*)GetTextureInSlot( samplers[i].v ) );

		unsigned int ss = GetSamplerStateInSlot( samplers[i].v );
		unsigned int filter = ss & SS_FILTER_MASK;
		unsigned int address = ss;

		switch ( filter )
		{
		case SS_POINT_POINT:
			dev->SetSamplerState( samplers[i].h, D3DSAMP_MAGFILTER, D3DTEXF_POINT );
			dev->SetSamplerState( samplers[i].h, D3DSAMP_MINFILTER, D3DTEXF_POINT );
			dev->SetSamplerState( samplers[i].h, D3DSAMP_MIPFILTER, D3DTEXF_POINT );
			break;
		case SS_POINT_LINEAR:
			dev->SetSamplerState( samplers[i].h, D3DSAMP_MAGFILTER, D3DTEXF_POINT );
			dev->SetSamplerState( samplers[i].h, D3DSAMP_MINFILTER, D3DTEXF_POINT );
			dev->SetSamplerState( samplers[i].h, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR );
			break;
		case SS_LINEAR_LINEAR:
		default:
			dev->SetSamplerState( samplers[i].h, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
			dev->SetSamplerState( samplers[i].h, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
			dev->SetSamplerState( samplers[i].h, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR );
			break;
		};
		
		dev->SetSamplerState( samplers[i].h, D3DSAMP_ADDRESSU, (address & SS_WRAP_S) ? D3DTADDRESS_WRAP : D3DTADDRESS_CLAMP );
		dev->SetSamplerState( samplers[i].h, D3DSAMP_ADDRESSV, (address & SS_WRAP_T) ? D3DTADDRESS_WRAP : D3DTADDRESS_CLAMP );
		dev->SetSamplerState( samplers[i].h, D3DSAMP_ADDRESSW, (address & SS_WRAP_U) ? D3DTADDRESS_WRAP : D3DTADDRESS_CLAMP );
	}
}

void DrawIndexed( const ModelVert *verts, const unsigned short *indices, int numVerts, int numIndices )
{
	IDirect3DDevice9* dev = DXUTGetD3D9Device();
	dev->SetFVF( D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_TEX3|D3DFVF_TEXCOORDSIZE2(0)|D3DFVF_TEXCOORDSIZE4(1)|D3DFVF_TEXCOORDSIZE2(2) );
	HRESULT hr = dev->DrawIndexedPrimitiveUP( D3DPT_TRIANGLELIST, 0, numVerts, numIndices/3, indices, D3DFMT_INDEX16, verts, sizeof(ModelVert) );
}

void DrawGeometry( const ModelGeometry *geom )
{
	IDirect3DDevice9* dev = DXUTGetD3D9Device();

	Cache *vc = ResolveCache( geom->m_verts_VC, geom->m_verts, geom->m_numVerts*sizeof(geom->m_verts[0]), false );
	Cache *index = ResolveCache( geom->m_indices_VC, geom->m_indices, geom->m_numIndices*sizeof(geom->m_indices[0]), true );

	dev->SetStreamSource( 0, (LPDIRECT3DVERTEXBUFFER9)vc, 0, sizeof(ModelVert) );
	dev->SetIndices( (LPDIRECT3DINDEXBUFFER9)index );

	if ( geom->m_weights )
	{
		Cache *vcw = ResolveCache( geom->m_weights_VC, geom->m_weights, geom->m_numVerts*sizeof(geom->m_weights[0]), false );
		dev->SetStreamSource( 1, (LPDIRECT3DVERTEXBUFFER9)vcw, 0, sizeof(ModelVertWeight) );
		dev->SetVertexDeclaration( g_WeightedVertexDecl );
	} else
	{
		dev->SetVertexDeclaration( g_VertexDecl );
	}

	HRESULT hr = dev->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0, 0, geom->m_numVerts, 0, geom->m_numIndices/3 );
}

void SetDepthWrite( bool enabled )
{
	IDirect3DDevice9* dev = DXUTGetD3D9Device();
	dev->SetRenderState( D3DRS_ZENABLE, enabled ? D3DZB_TRUE : D3DZB_FALSE  );
}

void SetBlend( unsigned int src, unsigned int dst )
{
	DWORD blendMap[] = { D3DBLEND_ONE, D3DBLEND_ZERO, D3DBLEND_SRCALPHA, D3DBLEND_INVSRCALPHA, 
		D3DBLEND_DESTALPHA, D3DBLEND_INVDESTALPHA, D3DBLEND_SRCCOLOR, D3DBLEND_DESTCOLOR,
		D3DBLEND_INVSRCCOLOR, D3DBLEND_INVDESTCOLOR };

	IDirect3DDevice9* dev = DXUTGetD3D9Device();
	dev->SetRenderState( D3DRS_SRCBLEND, blendMap[src] );
	dev->SetRenderState( D3DRS_DESTBLEND, blendMap[dst] );
	dev->SetRenderState( D3DRS_ALPHABLENDENABLE, (src==0 && dst==1) ? FALSE : TRUE );
}



Vectormath::Aos::Matrix4 MakeHWProjection( float fovyRadians, float aspect, float zNear, float zFar )
{
    float f, rangeInv;
    f = tanf( ( (float)( _VECTORMATH_PI_OVER_2 ) - ( 0.5f * fovyRadians ) ) );
    rangeInv = ( 1.0f / ( zFar - zNear ) );
	return Vectormath::Aos::Matrix4(
        Vectormath::Aos::Vector4( ( f / aspect ), 0.0f, 0.0f, 0.0f ),
        Vectormath::Aos::Vector4( 0.0f, f, 0.0f, 0.0f ),
        Vectormath::Aos::Vector4( 0.0f, 0.0f, ( ( zFar ) * rangeInv ), 1.0f ),
        Vectormath::Aos::Vector4( 0.0f, 0.0f, ( ( -( zNear * zFar ) * rangeInv ) ), 0.0f )
    );
}

void SetDefaultRT()
{
	IDirect3DDevice9* dev = DXUTGetD3D9Device();
	dev->SetRenderTarget( 0, defaultRT0 );
	dev->SetDepthStencilSurface( defaultDS );
}

void SetRT( Texture *rt0, DepthStencil *ds )
{
	IDirect3DDevice9* dev = DXUTGetD3D9Device();
	LPDIRECT3DTEXTURE9 tex = (LPDIRECT3DTEXTURE9)rt0;
	LPDIRECT3DSURFACE9 rt0surf;
	tex->GetSurfaceLevel( 0, &rt0surf );
	dev->SetRenderTarget( 0, rt0surf );
	rt0surf->Release();

	dev->SetDepthStencilSurface( (LPDIRECT3DSURFACE9)ds );
}

void ClearRT( bool rt0, unsigned int colour, bool depth, float depthValue )
{
	IDirect3DDevice9* dev = DXUTGetD3D9Device();
	DWORD flags = 0;
	flags |= rt0 ? D3DCLEAR_TARGET : 0;
	flags |= depth ? D3DCLEAR_ZBUFFER : 0;
	dev->Clear( 0, NULL, flags, colour, depthValue, 127 );
}

void ColourWrite( bool r, bool g, bool b, bool a )
{
	IDirect3DDevice9* dev = DXUTGetD3D9Device();
	DWORD cw = 0;
	cw |= r ? D3DCOLORWRITEENABLE_RED : 0;
	cw |= g ? D3DCOLORWRITEENABLE_GREEN : 0;
	cw |= b ? D3DCOLORWRITEENABLE_BLUE : 0;
	cw |= a ? D3DCOLORWRITEENABLE_ALPHA : 0;
	dev->SetRenderState( D3DRS_COLORWRITEENABLE, cw );
}


int GetBackbufferWidth()
{
	return g_width;
}

int GetBackbufferHeight()
{
	return g_height;
}

static unsigned int cacheId = 1;
static std::map< unsigned int, LPDIRECT3DRESOURCE9 > cache;

Cache* ResolveCache( unsigned int &input, void *data, int byteSize, bool indexBuffer )
{
	if ( input != 0 )
	{
		std::map<unsigned int, LPDIRECT3DRESOURCE9 >::const_iterator it = cache.find( input );
		if ( it != cache.end() )
		{
			return (Cache*)it->second;
		}
	}

	if ( data != NULL )
	{
		IDirect3DDevice9* dev = DXUTGetD3D9Device();
		LPDIRECT3DRESOURCE9 resource;
		if ( indexBuffer )
		{
			LPDIRECT3DINDEXBUFFER9 indexBuffer;
			dev->CreateIndexBuffer( byteSize, 0, D3DFMT_INDEX16, D3DPOOL_MANAGED, &indexBuffer, NULL );
			void *cdata;
			indexBuffer->Lock( 0, 0, &cdata, 0 );
			memcpy( cdata, data, byteSize );
			indexBuffer->Unlock();
			resource = indexBuffer;
		} else
		{
			LPDIRECT3DVERTEXBUFFER9 vertexBuffer;
			dev->CreateVertexBuffer( byteSize, 0, 0, D3DPOOL_MANAGED, &vertexBuffer, NULL );
			void *cdata;
			vertexBuffer->Lock( 0, 0, &cdata, 0 );
			memcpy( cdata, data, byteSize );
			vertexBuffer->Unlock();
			resource = vertexBuffer;
		}
		cache[cacheId] = resource;
		input = cacheId;
		cacheId++;
		return (Cache*)resource;
	}

	return NULL;
}

void InvalidateCache( unsigned int &input )
{
	input = 0;
}

void FreeCache()
{
	std::map< unsigned int, LPDIRECT3DRESOURCE9 >::iterator b = cache.begin();

	while ( b != cache.end() )
	{
		b->second->Release();
		b++;
	}

	cache.clear();
}


};
