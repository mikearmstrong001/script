#ifndef TESTING_FILESYS_H
#define TESTING_FILESYS_H

#include "ext/physfs-2.0.1/physfs.h"
#include <string>

PHYSFS_sint64 PHYSFS_readFile( const char *filename,
                                     void **buffer );
PHYSFS_sint64 PHYSFS_readFileAligned( const char *filename,
                                     void **buffer, int align );

PHYSFS_sint32 PHYSFS_gets( char *buffer, int size, PHYSFS_File *f );

PHYSFS_sint32 PHYSFS_writeCStr( PHYSFS_File *, const char * );
PHYSFS_sint32 PHYSFS_readCStr( PHYSFS_File *, std::string &str );

namespace fs
{

char *MakeCanonicalForm( char *buffer, const char *in );
void Init();
void Deinit();

};

#endif

