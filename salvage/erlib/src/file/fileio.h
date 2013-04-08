#ifndef ER_HEADER_FILEIO_H
#define ER_HEADER_FILEIO_H

#include "../erbase.h"

struct FILE_s {
	intptr_t *user;
};

enum FILE_OPEN_MODE {
	FO_READ,
	FO_WRITE,
};

enum FILE_SEEK_MODE {
	FS_SET,
	FS_CUR,
	FS_END,
};

void FILE_SetBase( const char *searchpath );


bool FILE_Open	( FILE_s *f, const char *name, FILE_OPEN_MODE m );
void FILE_Close	( FILE_s *f );

int  FILE_Read	( FILE_s *f, void *data, int len );
int  FILE_Write	( FILE_s *f, const void *data, int len );

void FILE_Seek	( FILE_s *f, int offset, FILE_SEEK_MODE m );
int  FILE_Tell	( FILE_s *f );

int  FILE_Length( FILE_s *f );

void FILE_Mkdir	( const char *dir );


#endif
