#include "dxut.h"
#include "flare.h"
#include "file_system.h"
#include "bind.h"
#include "ext/pugixml/src/pugixml.hpp"

Flare::Flare()
{
}

Flare::~Flare()
{
}

void Flare::Purge()
{
	items.clear();
}

void Flare::Load( const char *filename )
{
	Purge();

	char *xml;
	int len = (int)PHYSFS_readFile( filename, (void**)&xml );
	if ( !len )
	{
		return;
	}
	pugi::xml_document doc;
	pugi::xml_parse_result result = doc.load_buffer_inplace( xml, len );

	if ( result )
	{
		pugi::xml_node nvo = doc.child("FlareDef");
		if ( nvo )
		{
			for (pugi::xml_node item = nvo.child("Item"); item; item = item.next_sibling("Item"))
			{
				FlareItem fi;
				::Bind( fi.uv, 4, item.child_value("uv") );
				::Bind( fi.factor, item.child_value("factor") );
				::Bind( fi.scale, item.child_value("scale") );
				items.push_back( fi );
			}
		}
	}

	free( xml );
}


Flare* FlareManager::CreateEmpty( const char *filename )
{
	return new Flare;
}

Flare* FlareManager::LoadResource( const char *filename )
{
	Flare *mtr = new Flare;
	mtr->Load( filename );
	return mtr;
}

FlareManager::FlareManager()
{
}


FlareManager *flareManager()
{
	static FlareManager mgr;
	return &mgr;
}
