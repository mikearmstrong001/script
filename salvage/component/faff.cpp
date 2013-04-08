#include "faff.h"
#include "bind.h"
#include "components_basic.h"
#include "ext/pugixml/src/pugixml.hpp"
#include "file_system.h"

ComponentConstructor *ComponentConstructor::root = NULL;




void ParseXML( std::vector< GameObject* > &gos, const char *filename, const Vectormath::Aos::Transform3 &transform, WorldDB *worlddb )
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
		pugi::xml_node scene = doc.child( "Scene" );
		if ( scene )
		{
			for (pugi::xml_node go = scene.child("GameObject"); go; go = go.next_sibling("GameObject"))
			{
				GameObject *gameObject = new GameObject(worlddb);
				gameObject->Bind( NULL, NULL );
				for (pugi::xml_node tag = go.first_child(); tag; tag = tag.next_sibling())
				{
					if ( stricmp( tag.name(), "Component" ) == 0 )
					{
						Component *c = gameObject->AddComponent( tag.attribute("name").value(), tag.attribute("type").value() );
						c->Bind(NULL,NULL);

						for (pugi::xml_node ctag = tag.first_child(); ctag; ctag = ctag.next_sibling())
						{
							c->Bind( ctag.name(), ctag.child_value() );
						}
						CTransform *t = dynamic_cast<CTransform*>( c );
						if ( t )
						{
							t->setLocal( transform * t->local() );
						}
					} else
					{
						gameObject->Bind( tag.name(), tag.child_value() );
					}
				}
				gameObject->Enter();
				gos.push_back( gameObject );
			}
		}
	}
	free( xml );
}

static char *FrameAllocMemory[2] = { NULL, NULL };

static int  FrameAllocBuffer = 0;
static char *FrameAllocPtr = NULL;

void  FrameAllocInit( int maxBufferSize )
{
	FrameAllocMemory[0] = (char*)_aligned_malloc( maxBufferSize, 16 );
	FrameAllocMemory[1] = (char*)_aligned_malloc( maxBufferSize, 16 );
	FrameAllocPtr = FrameAllocMemory[0];
}

void  FrameAllocDestroy()
{
	_aligned_free( FrameAllocMemory[0] );
	_aligned_free( FrameAllocMemory[1] );
}


void* FrameAlloc( int size )
{
	char *ret = FrameAllocPtr;
	FrameAllocPtr += ((size + 15) & ~15);
	return ret;
}

void  FrameAllocNext()
{
	FrameAllocBuffer = 1 - FrameAllocBuffer;
	FrameAllocPtr = FrameAllocMemory[FrameAllocBuffer];
}

uint64_t HashedString( const char *str )
{
	uint64_t id = 0;
	while ( *str )
	{
		id = (id * 65599) + str[0];
		str++;
	}
	return id;
}

uint64_t HashedString( uint64_t id, const char *str )
{
	while ( *str )
	{
		id = (id * 65599) + str[0];
		str++;
	}
	return id;
}
