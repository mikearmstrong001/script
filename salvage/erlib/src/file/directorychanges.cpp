#include "directorychanges.h"
#include "..\erbase.h"
#define _WIN32_WINNT  0x0500
#include "windows.h"
#include "winbase.h"


#define FILE_SHARE_WIDEOPEN (FILE_SHARE_DELETE|FILE_SHARE_READ|FILE_SHARE_WRITE)


struct record {
	int next;
	int code;
	int len;
	short c[1];
};


static void cpywchar( char *d, short *s, int len ) {
	for (int i=0; i<len; i++) {
		d[i] = (char)s[i];
	}
	d[len] = 0;
}

static bool filterit( char *name ) {
	if ( strstr( name, ".svn" ) ) {
		return true;
	}
	if ( strstr( name, ".tmp" ) ) {
		return true;
	}
	if ( strstr( name, ".swp" ) ) {
		return true;
	}
	if ( strstr( name, "$" ) ) {
		return true;
	}

	return false;
}

struct threadInfo {
	char dir[256];
	void *(*cb)(void *, int t, const char *a, const char *b);
	void *user;
};


static void DirectoryChanges( void *data ) {
	int bufferLen = 20 * 1024;;
	char *dest = new char[bufferLen];
	DWORD ret;

	threadInfo *ti = (threadInfo*)data;
	//const char *dir;
	//void *(*cb)(void *, int t, const char *a, const char *b), void *user;

	char oldname[512] = {0};
	HANDLE h= CreateFileA( ti->dir, FILE_LIST_DIRECTORY, FILE_SHARE_WIDEOPEN, NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS /*| FILE_FLAG_OVERLAPPED*/, NULL );
	unsigned long filter= FILE_NOTIFY_CHANGE_LAST_WRITE | FILE_NOTIFY_CHANGE_FILE_NAME;

	while ( 1 ) {
		
		ReadDirectoryChangesW(h, dest, bufferLen, TRUE, filter, &ret, NULL, NULL);
		if ( ret ) {
			record *r = (record*)dest;
			while ( 1 ) {
				char filename[512];

				cpywchar( filename, r->c, r->len/2 );
				strlwr( filename );

				if ( !filterit( filename ) ) {
					if ( r->code == 1 ) {
						ti->cb( ti->user, r->code, filename, NULL );
//						printf( "add '%s'\n", filename);
					} else
					if ( r->code == 2 ) {
						ti->cb( ti->user, r->code, filename, NULL );
//						printf( "remove '%s'\n", filename);
					} else
					if ( r->code == 3 ) {
						ti->cb( ti->user, r->code, filename, NULL );
//						printf( "update '%s'\n", filename);
					} else
					if ( r->code == 4 ) {
						strcpy( oldname, filename );
					} else
					if ( r->code == 5 ) {
						if ( oldname[0] ) {
							ti->cb( ti->user, r->code, filename, oldname );
//							printf( "rename '%s' '%s'\n", oldname, filename);
						} else {
							ti->cb( ti->user, 3, filename, NULL );
//							printf( "update '%s'\n", filename);
						}
						oldname[0] = 0;
					}
				}

				ret -= r->next;
				if ( r->next ) {
					r = (record*)((char*)r + r->next);
				} else {
					break;
				}
			}
		}
		Sleep(10);
	}

	CloseHandle( h );
}

void FS_DirectoryChanges( const char *dir, void *(*cb)(void *, int t, const char *a, const char *b), void *user ) {

	threadInfo *ti = new threadInfo;
	ti->cb = cb;
	strcpy( ti->dir, dir );
	ti->user = user;

	CreateThread(
		   NULL,	// LPSECURITY_ATTRIBUTES lpsa,
		   0,		// DWORD cbStack,
		   (LPTHREAD_START_ROUTINE)DirectoryChanges,	// LPTHREAD_START_ROUTINE lpStartAddr,
		   (LPVOID)ti,	// LPVOID lpvThreadParm,
		   0,			//   DWORD fdwCreate,
		   NULL);
}
