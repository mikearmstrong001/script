#include "material.h"
#include "texture_manager.h"
#include "technique_manager.h"
#include "hw.h"
#include "file_system.h"

static char* SplitPath( char *out, const char *in )
{
	char drive[_MAX_DRIVE];
	char dir[_MAX_DIR];
	char filename[_MAX_FNAME];
	char ext[_MAX_EXT];
	_splitpath( in, drive, dir, filename, ext );

	sprintf( out, "%s%s", drive, dir );
	return out;
}

static int interpretBlend( const char *value )
{
	if ( _stricmp( value, "one" ) == 0 )
	{
		return hw::BLEND_ONE;
	} else
	if ( _stricmp( value, "zero" ) == 0 )
	{
		return hw::BLEND_ZERO;
	} else
	if ( _stricmp( value, "srcalpha" ) == 0 )
	{
		return hw::BLEND_SRCALPHA;
	} else
	if ( _stricmp( value, "invsrcalpha" ) == 0 )
	{
		return hw::BLEND_INVSRCALPHA;
	} else
	if ( _stricmp( value, "dstalpha" ) == 0 )
	{
		return hw::BLEND_DSTALPHA;
	} else
	if ( _stricmp( value, "invdstalpha" ) == 0 )
	{
		return hw::BLEND_INVDSTALPHA;
	} else
	if ( _stricmp( value, "srccolour" ) == 0 )
	{
		return hw::BLEND_SRCCOLOUR;
	} else
	if ( _stricmp( value, "dstcolour" ) == 0 )
	{
		return hw::BLEND_DSTCOLOUR;
	} else
	if ( _stricmp( value, "invsrccolour" ) == 0 )
	{
		return hw::BLEND_INVSRCCOLOUR;
	} else
	if ( _stricmp( value, "invdstcolour" ) == 0 )
	{
		return hw::BLEND_INVDSTCOLOUR;
	} else
	{
		return hw::BLEND_ONE;
	}
}

Material::Material() :
	blendSrc(hw::BLEND_ONE), blendDst(hw::BLEND_ZERO), diffuseSlot(-1), normalSlot(-1), diffuse(NULL), normal(NULL), technique(NULL), depthWrite(NULL)
{
}

Material::~Material()
{
	Purge();
}

void Material::Purge()
{
	blendSrc = hw::BLEND_ONE;
	blendDst = hw::BLEND_ZERO;
	diffuseSlot = -1;
	normalSlot = -1;
	diffuse = NULL;
	normal = NULL;
	technique = NULL;
	depthWrite = true;
	copyScreen = false;

	SetLoaded( false );
	SetDefaulted( false );
}

void Material::Load(const char *filename)
{
	Purge();

	char buffer[512];
	PHYSFS_File *f = PHYSFS_openRead( fs::MakeCanonicalForm( buffer, filename ) );
	if ( f )
	{
		char path[256];
		SplitPath( path, filename );

		char line[512];
		while ( PHYSFS_gets( line, sizeof(line), f ) )
		{
			int i;
			char str[256];
			if ( sscanf( line, "map_Kd %s", str ) == 1 )
			{
				diffuseSlot = hw::GetTextureSlot( "diffuse" );
				diffuse = textureManager()->Load( std::string(str).c_str() );
			} else
			if ( sscanf( line, "map_Nrm %s", str ) == 1 )
			{
				normalSlot = hw::GetTextureSlot( "normal" );
				normal = textureManager()->Load( std::string(str).c_str() );
			} else
			if ( sscanf( line, "technique %s", str ) == 1 )
			{
				technique = techniqueManager()->Load( str );
			} else
			if ( sscanf( line, "depthWrite %d", &i ) == 1 )
			{
				depthWrite = (i != 0);
			} else
			if ( sscanf( line, "copyScreen %d", &i ) == 1 )
			{
				copyScreen = (i != 0);
			}
			else if ( sscanf( line, "blendSrc %s", str ) == 1 )
			{
				blendSrc = interpretBlend( str );
			}
			else if ( sscanf( line, "blendDst %s", str ) == 1 )
			{
				blendDst = interpretBlend( str ); 
			}
		}
		PHYSFS_close(f);
	}

	if ( technique == NULL )
	{
		technique = techniqueManager()->Load( "_default" );
		SetDefaulted( true );
	}
}

void Material::Bind() const
{
	if ( copyScreen )
	{
		hw::CopyScreen();
	}
	if ( diffuseSlot != -1 )
	{
		hw::SetTextureInSlot( diffuseSlot, diffuse->HW(), hw::SS_LINEAR_LINEAR|hw::SS_WRAP_S|hw::SS_WRAP_T );
	}
	if ( normalSlot != -1 )
	{
		hw::SetTextureInSlot( normalSlot, normal->HW(), hw::SS_LINEAR_LINEAR|hw::SS_WRAP_S|hw::SS_WRAP_T );
	}
	hw::SetBlend( blendSrc, blendDst );
	hw::SetDepthWrite( depthWrite );
};
