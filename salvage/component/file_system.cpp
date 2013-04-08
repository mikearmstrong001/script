
#define _CRT_SECURE_NO_WARNINGS
#include "dxut.h"
#include <stdio.h>
#include <ctype.h>
#include "file_system.h"
#include <malloc.h>
#include <direct.h>
#include <string>
#include <vector>
#include <algorithm>
#include <io.h>

#pragma comment(lib, "ext/physfs-2.0.1/Debug/physfs.lib" )

PHYSFS_sint64 PHYSFS_readFile( const char *filename,
                                     void **buffer )
{
	PHYSFS_File *file = PHYSFS_openRead( filename );
	if ( file )
	{
		PHYSFS_sint64 len = PHYSFS_fileLength( file );
		*buffer = malloc( (size_t)len + 1 );
		PHYSFS_read( file, *buffer, 1, len );		
		PHYSFS_close( file );
		((char*)*buffer)[len] = 0;
		return len;
	} else
	{
		*buffer = NULL;
		return 0;
	}
}

PHYSFS_sint64 PHYSFS_readFileAligned( const char *filename,
                                     void **buffer, int align )
{
	PHYSFS_File *file = PHYSFS_openRead( filename );
	if ( file )
	{
		PHYSFS_sint64 len = PHYSFS_fileLength( file );
		*buffer = _aligned_malloc( (size_t)len + 1, align );
		PHYSFS_read( file, *buffer, 1, len );		
		PHYSFS_close( file );
		((char*)*buffer)[len] = 0;
		return len;
	} else
	{
		*buffer = NULL;
		return 0;
	}
}
PHYSFS_sint32 PHYSFS_gets( char *buffer, int size, PHYSFS_File *file )
{
	bool started = false;
	char *c = buffer;
	while ( size )
	{
		PHYSFS_sint8 in;
		if ( PHYSFS_read(file, &in, sizeof (in), 1) != 1 )
		{
			return c != buffer;
		}
		if ( in != 0 && in != '\n' && in != '\r' )
		{
			started = true;
		} else
		if ( started )
		{
			break;
		}

		if ( started && (c-buffer) < (size-1) )
		{
			*c++ = in;
		}
	}
	*c = 0;

    return(c != buffer);
}

PHYSFS_sint32 PHYSFS_writeCStr( PHYSFS_File *file, const char *str )
{
	int l = strlen( str );
	PHYSFS_sint32 t = 0;
	t += PHYSFS_writeSLE32( file, l );
	t += PHYSFS_write( file, str, l, 1 );
	return t;
}

PHYSFS_sint32 PHYSFS_readCStr( PHYSFS_File *file, std::string &str )
{
	PHYSFS_sint32 l;
	PHYSFS_sint32 t = 0;
	t += PHYSFS_readSLE32( file, &l );
	str.resize( l );
	t += PHYSFS_read( file, &str[0], l, 1 );
	return t;
}



namespace fs
{

	static const char *baseDir = "media/";

	char *MakeCanonicalForm( char *buffer, const char *in )
	{
		sprintf( buffer, "%s", in );

		char *c = buffer;
		while ( *c ) {
			*c = (char)tolower( *c );
			if ( *c == '\\' ) {
				*c = '/';
			}
			c++;
		}

		return buffer;
	}

	struct fileList
	{
		std::vector< std::string > mounts;
	};

	static void filesCB(void *data, const char *origdir, const char *fname)
	{
	}
 
	void Init()
	{
		char buffer[512];
		_getcwd( buffer, sizeof(buffer) );
		int len = strlen( buffer );
		buffer[len] = '/';
		buffer[len+1] = 0;
		//strcat( buffer, "\\" );
		PHYSFS_init( buffer );//MakeCanonicalForm( buffer2, buffer ) );
		PHYSFS_setWriteDir( "generated/" );
		PHYSFS_mount( "generated\\", NULL, true );

		std::vector< std::string > zipfiles;
		struct _finddata_t fd;
		intptr_t h = _findfirst( "*.zip", &fd );
		if ( h != -1 )
		{
			do
			{
				zipfiles.push_back( fd.name );
			} while ( _findnext( h, &fd ) != -1 );
			_findclose( h );
			std::sort( zipfiles.begin(), zipfiles.end() );
		}
		for (int i=zipfiles.size()-1; i>=0; i--)
		{
			PHYSFS_mount( zipfiles[i].c_str(), NULL, true );
		}

		PHYSFS_mount( "media\\", NULL, true );


	}

	void Deinit()
	{
		PHYSFS_deinit();
	}


};
