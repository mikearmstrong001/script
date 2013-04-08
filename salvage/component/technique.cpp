#include "dxut.h"
#include "technique.h"
#include "hw.h"
#include "file_system.h"
#include "shader_manager.h"
#include "model_structs.h"

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

static int interpretPassName( const char *name )
{
	if ( _stricmp( name, "LightPrepass" ) == 0 )
	{
		return RP_LightPrepass;
	}
	else if ( _stricmp( name, "LightPrepass_Lights" ) == 0 )
	{
		return RP_LightPrepass_Lights;
	}
	else if ( _stricmp( name, "LightPrepass_LitOpaque" ) == 0 )
	{
		return RP_LightPrepass_LitOpaque;
	}
	else if ( _stricmp( name, "LitOpaque" ) == 0 )
	{
		return RP_LitOpaque;
	}
	else if ( _stricmp( name, "Opaque" ) == 0 )
	{
		return RP_Opaque;
	}
	else if ( _stricmp( name, "Transparent" ) == 0 )
	{
		return RP_Transparent;
	}
	else if ( _stricmp( name, "Postprocess" ) == 0 )
	{
		return RP_Postprocess;
	}
	else if ( _stricmp( name, "Debug" ) == 0 )
	{
		return RP_Debug;
	}
	else if ( _stricmp( name, "Debug2D" ) == 0 )
	{
		return RP_Debug2D;
	}
	else 
	{
		return RP_LitOpaque;
	}
}

void TechniquePass::Bind() const
{
	if ( blendSrc != -1 && blendDst != -1 )
	{
		hw::SetBlend( blendSrc, blendDst );
	}
}



Technique::Technique()
{
}

Technique::~Technique()
{
}

void Technique::Purge()
{
	passes.clear();
}

static char *stripws( char *l )
{
	while ( *l && (*l <= 32 || *l == '\t' ) )
	{
		l++;
	}
	return l;
}

static bool ParsePass( TechniquePass &pass, PHYSFS_File *f, const char *passName )
{
	pass.pass = interpretPassName( passName );
	char line[512];
	while ( PHYSFS_gets( line, sizeof(line), f ) )
	{
		char *tok = stripws( line );
		if ( tok == NULL )
		{
			continue;
		}
		char str[256];
		if ( sscanf( tok, "blendSrc %s", str ) == 1 )
		{
			pass.blendSrc = interpretBlend( str );
		}
		else if ( sscanf( tok, "blendDst %s", str ) == 1 )
		{
			pass.blendDst = interpretBlend( str );
		}
		else if ( sscanf( tok, "shader %s", str ) == 1 )
		{
			pass.shader = shaderManager()->Load( str );
		}
		else {
			tok = strtok( tok, " \t\n\r" );
			if ( tok && _stricmp( tok, "endpass" ) == 0 )
			{
				return true;
			}
		}
	}
	return false;
}

void Technique::Load( const char *filename )
{
	char buffer[512];
	PHYSFS_File *f = PHYSFS_openRead( fs::MakeCanonicalForm( buffer, filename ) );
	if ( f )
	{
		char line[512];
		while ( PHYSFS_gets( line, sizeof(line), f ) )
		{
			char str[256];
			if ( sscanf( line, "pass %s", str ) == 1 )
			{
				TechniquePass pass;
				if ( ParsePass( pass, f, str ) )
				{
					passes.push_back( pass );
				}
			}
		}
		PHYSFS_close(f);
	} else
	{
		TechniquePass pass;
		pass.pass = RP_Opaque;
		pass.shader = shaderManager()->Load( "_default" );
		pass.blendSrc = hw::BLEND_ONE;
		pass.blendDst = hw::BLEND_ZERO;
		passes.push_back( pass );
	}
}

