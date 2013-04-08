#include "erbase.h"
#include "findfiles.h"
#include "vector.h"
#include "str.h"
#include "wildcard.h"


void Findfiles( Vector<Str<char>> &filelist, const char *basedir, const char *wc, bool recursive ) {
	_finddata_t fi;
	intptr_t h = _findfirst( va( "%s\\%s", basedir, wc ), &fi  );

	if ( h != -1 ) {
		do {
			if ( !(recursive && (fi.attrib & _A_SUBDIR)) ) {
				filelist.PushBack( va( "%s\\%s", basedir, fi.name ) );
			}
		} while ( _findnext( h, &fi ) == 0 );
		_findclose( h );
	}

	if ( recursive ) {
		h = _findfirst( va( "%s\\*.*", basedir ), &fi  );

		if ( h != -1 ) {
			do {
				if ( recursive && (fi.attrib & _A_SUBDIR) ) {
					if ( strcmp( fi.name, "." ) == 0 ) {
						continue;
					}
					if ( strcmp( fi.name, ".." ) == 0 ) {
						continue;
					}
					char subdir[256];
					sprintf( subdir, "%s\\%s", basedir, fi.name );
					Findfiles( filelist, subdir, wc, recursive );
				}
			} while ( _findnext( h, &fi ) == 0 );
			_findclose( h );
		}
	}
}

