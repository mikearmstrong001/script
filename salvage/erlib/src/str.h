#ifndef ER_HEADER_STR_H
#define ER_HEADER_STR_H

#pragma warning( disable : 4996 )

#include "erbase.h"
#include "align.h"

template <class T>
class Str {
	T *d;
	int len;
	int alloced;
public:

	Str() : d(NULL), len(0), alloced(0) {
	}

	Str( Str<T> const &o ) : d(NULL), len(0), alloced(0) {
		Insert( o.c_str(), o.Length() );
	}

	Str( const T *s ) : d(NULL), len(0), alloced(0) {
		int l = Length( s );
		EnsureAlloced( l, false );
		memcpy( d, s, (l+1) * sizeof(T) );
		len = l;
	}

	~Str() {
		delete []d;
	}

	const T *c_str() const {
		if ( d == NULL ) {
			return Empty();
		} else {
			return d;
		}
	}

	int Length( void ) const {
		return len;
	}

	void Append( T c ) {
		EnsureAlloced( len+1, true );
		d[len] = c;
		len++;
		d[len] = 0;
	}

	void EnsureAlloced( int l, bool copy ) {
		int al = AlignUp( l+1, 16 );

		if ( alloced < al ) {
			T *od = d;
			d = new T[al];
			if ( copy && od != NULL ) {
				memcpy( d, od, (len+1) * sizeof(T) );
			}
			delete []od;
			alloced = al;
		}
	}

	void Clear( void ) {
		len = 0;
		if ( d != NULL ) {
			d[len] = 0;
		}
	}

	void Insert( const T *c, int l ) {
		EnsureAlloced( l, false );
		memcpy( d, c, (l) * sizeof(T) );
		len = l;
		d[len] = 0;
	}

	inline const T *Empty( void ) const;

	Str<T> &operator=( Str<T> const &o ) {
		Insert( o.c_str(), o.Length() );
		return *this;
	}

	void Replace( const T *what, const T *with ) {
		int lwhat = Length( what );
		int lwith = Length( with );
		int diff = lwith - lwhat;
		T *cd = d;
		while ( true ) {		
			T *pos = Strstr( cd, what );
			if ( pos ) {
				intptr_t ofs = pos - d;
				EnsureAlloced( len + diff, true );
				memmove( &d[ofs+lwith], &d[ofs+lwhat], (char*)&d[len+1] - (char*)&d[ofs+lwhat] );
				memcpy( &d[ofs], with, lwith * sizeof(T) );
				len += diff;

				cd = d + ofs + lwith;
			} else {
				break;
			}
		}
	}

	int Find( int pos, const T &v ) {
		int l = Length();
		for (int i=pos; i<l; i++ ) {
			if ( d[i] == v ) {
				return i;
			}
		}
		return -1;
	}

	int FindAny( int pos, const T *values ) {
		int l = Length();
		for (int i=pos; i<l; i++ ) {
			const T *c = values;
			while ( *c ) {
				if ( *c == d[i] ) {
					return i;
				}
				c++;
			}
		}
		return -1;
	}

	void RemoveExtension( Str<T> &out ) const {
		out.Clear();

		int idx = Length();
		while ( d[idx] != '.' && idx > 0 ) {
			idx--;
		}

		for (int i=0; i<idx; i++) {
			out.Append( d[i] );
		}
	}

	void SetExtension( const T *newext ) {

		int idx = Length();
		while ( d[idx] != '.' && idx > 0 ) {
			idx--;
		}

		if ( d[idx] != '.' ) {
			return;
		}

		Str<T> oldext = &d[idx];

		Replace( oldext.c_str(), newext );
	}

	void Lower( void ) {
		T *c = d;
		while ( *c ) {
			if ( *c >= 'A' && *c <= 'Z' ) {
				*c = (*c - 'A') + 'a';
			}
			c++;
		}
	}

	static int Length( const T *c ) {
		int l = 0;
		while ( *c ) {
			l++;
			c++;
		}
		return l;
	}

	static T * Strstr( T *haystack, const T *needle ) {
		if ( needle == NULL || haystack == NULL || needle[0] == 0 ) {
			return NULL;
		}

		T *s = haystack;
		while ( *s ) {
			if ( *s == needle[0] ) {
				T *ns = &s[1];
				const T *n = needle+1;
				while ( *ns && *n && *ns == *n ) {
					ns++;
					n++;
				}
				if ( *n == 0 ) {
					return s;
				}
			}
			s++;
		}

		return NULL;
	}

	static void StrnCpy( T *d, const T *s, int maxlen ) {
		maxlen--;
		while ( maxlen && *s ) {
			*d++ = *s++;
			maxlen--;
		}
		*d = 0;
	}
};



template <>
inline const char* Str<char>::Empty(void) const {
	return "";
}


class StrId {
	unsigned int id;

public:

	explicit StrId() : id(0) {
	}

	explicit StrId( const char *s ) {
		id = Hash( s );
	}

	unsigned int GetId( void ) {
		return id;
	}

	bool operator==( const StrId &o ) const {
		return id == o.id;
	}

	bool operator<( const StrId &o ) const {
		return id < o.id;
	}

	static unsigned int Hash( const char *s );
};


#endif
