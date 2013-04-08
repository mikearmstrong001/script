#include "dxut.h"
#include "lightmapbinding.h"
#include "file_system.h"
#include "texture_manager.h"
#include "hw.h"
#include "ext/pugixml/src/pugixml.hpp"

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

LightMapBinding::LightMapBinding()
{
}

LightMapBinding::~LightMapBinding()
{
}

static char *stripws( char *l )
{
	while ( *l && (*l <= 32 || *l == '\t' ) )
	{
		l++;
	}
	return l;
}

static bool ParseMesh( MeshLightmapBindSet &bindset, PHYSFS_File *f, const char *path )
{
	char line[512];
	while ( PHYSFS_gets( line, sizeof(line), f ) )
	{
		char *tok = stripws( line );
		if ( tok == NULL )
		{
			continue;
		}
		char str0[64];
		char str1[256];
		if ( sscanf( tok, "bind %s %s", str0, str1 ) == 2 )
		{
			LightmapBind bind;
			bind.bind = str0;
			bind.bindIndex = hw::GetTextureSlot( str0 );
			bind.tex = textureManager()->Load( (std::string(path) + std::string(str1)).c_str() );
			bindset.binds.push_back( bind );
		} else
		{
			tok = strtok( tok, " \t\n\r" );
			if ( tok && _stricmp( tok, "endmesh" ) == 0 )
			{
				return true;
			}
		}
	}
	return false;
}

static void ParseNovaOutputmap( std::map<std::string,MeshLightmapBindSet> &bindings, const char *filename, const char *path )
{
	char *xml;
	if ( !PHYSFS_readFile( filename, (void**)&xml ) )
	{
		return;
	}
	pugi::xml_document doc;
	pugi::xml_parse_result result = doc.parse( xml );//load_file(filename);

	if ( result )
	{
		pugi::xml_node nvo = doc.child("novaoutputmap");
		if ( nvo )
		{
			for (pugi::xml_node grp = nvo.child("lightmapgroup"); grp; grp = grp.next_sibling("lightmapgroup"))
			{
				pugi::xml_node mesh = grp.child("mesh");
				if ( mesh )
				{
					MeshLightmapBindSet bindSet;
					for (pugi::xml_node lmap = grp.child("lightmap"); lmap; lmap = lmap.next_sibling("lightmap"))
					{
						LightmapBind bind;
						bind.bind = std::string("lightmap_") + std::string(lmap.attribute("channel").value());
						bind.bindIndex = hw::GetTextureSlot( bind.bind.c_str() );
						bind.tex = textureManager()->Load( (std::string(path) + std::string(lmap.attribute("name").value())).c_str() );
						bindSet.binds.push_back( bind );
					}
					bindings[mesh.attribute("path").value()] = bindSet;
				}
			}
		}
	}
	free( xml );
}

void LightMapBinding::Load( const char *filename )
{
	char path[256];
	SplitPath( path, filename );

	if ( strstr( filename, ".nova.outputmap" ) )
	{
		char buffer[512];
		ParseNovaOutputmap( bindings, fs::MakeCanonicalForm( buffer, filename ), path );
	} else
	{
		char buffer[512];
		PHYSFS_File *f = PHYSFS_openRead( fs::MakeCanonicalForm( buffer, filename ) );
		if ( f )
		{
			char line[512];
			while ( PHYSFS_gets( line, sizeof(line), f ) )
			{
				char str[256];
				if ( sscanf( line, "mesh %s", str ) == 1 )
				{
					MeshLightmapBindSet bindset;
					if ( ParseMesh( bindset, f, path ) )
					{
						bindings[str] = bindset;
					}
				}
			}
			PHYSFS_close(f);
		}
	}
}


void LightMapBinding::Purge()
{
	bindings.clear();
}

const MeshLightmapBindSet *LightMapBinding::FindBind( const char *name )
{
	std::map<std::string,MeshLightmapBindSet>::iterator f = bindings.find( name );
	if ( f == bindings.end() )
	{
		return NULL;
	} else
	{
		return &f->second;
	}
}

void MeshLightmapBindSet::Bind() const
{
	for (unsigned int i=0; i<binds.size(); i++)
	{
		hw::SetTextureInSlot( binds[i].bindIndex, binds[i].tex->HW(), hw::SS_LINEAR_LINEAR );
	}
}