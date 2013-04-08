
#include "fileio.h"
#include <direct.h>

static char s_searchpath[1204] = ".";

void FILE_SetBase( const char *searchpath ) {
	strcpy( s_searchpath, searchpath );
}

static const char* fomodes[] = {
	"rb",
	"wb"
};

static const int fsmodes[] = {
	SEEK_SET,
	SEEK_CUR,
	SEEK_END
};

bool FILE_Open( FILE_s *f, const char *name, FILE_OPEN_MODE m ) {
	if ( FO_WRITE == m ) {
		char drive[8] = {0};
		char dir[128] = {0};
		char filename[128] = {0};
		char ext[32] = {0};
		_splitpath( va( "%s/%s", s_searchpath, name ), drive, dir, filename, ext );
		FILE_Mkdir( va( "%s%s", drive, dir ) );
	}

	f->user = (intptr_t*)fopen( va( "%s/%s", s_searchpath, name ), fomodes[m] );
	return f->user != NULL;
}

void FILE_Close( FILE_s *f ) {
	if ( f->user != NULL ) {
		fclose( (FILE*)f->user );
	}
	f->user = NULL;
}

int  FILE_Read ( FILE_s *f, void *data, int len ) {
	if ( f->user == NULL ) {
		return -1;
	}

	return (int)fread( data, 1, len, (FILE*)f->user );
}


int  FILE_Write( FILE_s *f, const void *data, int len ) {
	if ( f->user == NULL ) {
		return -1;
	}

	return (int)fwrite( data, 1, len, (FILE*)f->user );
}

void  FILE_Seek	( FILE_s *f, int offset, FILE_SEEK_MODE m ) {
	if ( f->user == NULL ) {
		return;
	}

	fseek( (FILE*)f->user, offset, fsmodes[ m ] );
}

int  FILE_Tell	( FILE_s *f ) {
	if ( f->user == NULL ) {
		return -1;
	}

	return ftell( (FILE*)f->user );
}

int  FILE_Length( FILE_s *f ) {
	if ( f->user == NULL ) {
		return -1;
	}

	int t = FILE_Tell( f );
	FILE_Seek( f, 0, FS_END );
	int l = FILE_Tell( f );
	FILE_Seek( f, t, FS_SET );

	return l;
}


void FILE_Mkdir	( const char *dir ) {
	char temp[1024];
	strcpy( temp, dir );

	char *pos = temp;
	while ( *pos ) {
		char c = *pos;
		if ( c == '\\' || c == '/' ) {
			*pos = 0;
			int err = _mkdir( temp );
			if ( err != 0 ) {
				printf( "err %d\n", err );
			}
			*pos = c;
		}
		pos++;
	}
}
