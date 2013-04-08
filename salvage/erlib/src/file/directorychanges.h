#ifndef ER_HEADER_DIRECTORYCHANGES_H
#define ER_HEADER_DIRECTORYCHANGES_H


void FS_DirectoryChanges( const char *dir, void *(*cb)(void *, int t, const char *a, const char *b), void *user );


#endif
