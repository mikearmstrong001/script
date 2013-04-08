#include "DXUT.h"
#include "malloc.h"
#include "tokeniser.h"
#include "stdio.h"
#include "stdlib.h"

static char *skipCppComment( char *cursor, bool &newLine ) {
	newLine = false;
	while ( cursor[0] && cursor[0] != '*' && cursor[1] != '/' ) {
		if ( cursor[0] == '\n' ) {
			newLine = true;
		}
		cursor++;
	}

	if ( cursor[0] == '*' ) {
		return cursor+2;
	} else {
		return cursor;
	}
}

static char *skipCComment( char *cursor ) {
	while ( cursor[0] && cursor[0] != '\n' ) {
		cursor++;
	}

	if ( cursor[0] == '\n' ) {
		return cursor+1;
	} else {
		return cursor;
	}
}

static bool isAnyOf( char c, const char *charlist ) {
	const char *clp = charlist;
	while ( *clp ) {
		if ( c == *clp ) {
			return true;
		}
		clp++;
	}
	return false;
}

static bool isWhiteSpace( char c ) {
	return isAnyOf( c, " \t\n\r" );
}

static bool isSingleCharToken( char c, const char *singleCharList ) {
	if ( singleCharList == NULL ) {
		return false;
	}
	return isAnyOf( c, singleCharList );
}

bool tokIsEndOfLine( char *cursor ) {
	if ( cursor[0] == '/' ) {
		cursor++;
		if ( cursor[0] == '*' ) {
			bool newLine = false;
			cursor = skipCppComment( cursor, newLine );
			if ( newLine ) {
				return true;
			}
		} else
		if ( cursor[0] == '/' ) {
			return true;
		}
		return tokIsEndOfLine( cursor );
	}

	if ( isAnyOf( cursor[0], " \t\r" ) ) {
		cursor++;
		while ( isAnyOf( cursor[0], " \t\r" ) ) {
			cursor++;
		}
		return tokIsEndOfLine( cursor );
	}

	return cursor[0] == '\n';
}

bool tokGetToken( token &t, char *&cursor, const char *singleCharList ) {
	if ( cursor[0] == '/' ) {
		if ( cursor[1] == '*' ) {
			bool newLine;
			cursor++;
			cursor = skipCppComment( cursor, newLine );
			return tokGetToken( t, cursor, singleCharList );
		} else
		if ( cursor[1] == '/' ) {
			cursor++;
			cursor = skipCComment( cursor );
			return tokGetToken( t, cursor, singleCharList );
		}
	}

	if ( isWhiteSpace( cursor[0] ) ) {
		cursor++;
		while ( isWhiteSpace( cursor[0] ) ) {
			cursor++;
		}
		return tokGetToken( t, cursor, singleCharList );
	}

	if ( isSingleCharToken( cursor[0], singleCharList ) ) {
		t.start = cursor;
		t.len = 1;
		t.isQuoted = false;
		cursor++;
		return true;
	}

	if ( cursor[0] == '"' ) {
		cursor++;
		t.start = cursor;
		t.len = 0;
		t.isQuoted = true;
		while ( cursor[0] && cursor[0] != '"' ) {
			if ( cursor[0] == '\\' ) {
				cursor++;
			}
			cursor++;
			t.len++;
		}
		cursor++;
	} else {
		t.start = cursor;
		t.len = 0;
		t.isQuoted = false;
		while ( cursor[0] && !isWhiteSpace( cursor[0] ) && !isSingleCharToken( cursor[0], singleCharList ) ) {
			t.len++;
			cursor++;
		}
	}

	return t.len > 0;
}

char tokNextChar( token &t, int &ofs ) {
	if ( ofs >= t.len ) {
		return 0;
	}

	char c = *(t.start + ofs);
	ofs++;
	if ( t.isQuoted && c == '\\' ) {
		c = *(t.start + ofs);
		ofs++;
		if ( c == 'n' ) {
			c = '\n';
		} else
		if ( c == 't' ) {
			c = '\t';
		}
	}
	return c;
}

bool tokTest( token &t, const char *value ) {
	int ofs = 0;
	char c0 = tokNextChar( t, ofs );
	const char *c1 = value;

	while ( c0 && *c1 && c0==*c1 ) {
		c0 = tokNextChar( t, ofs );
		c1++;
	}

	return c0 == 0 && *c1 == 0;
}

bool tokExpect( char *&cursor, const char *singleCharList, const char *expect ) {
	token t;
	if ( !tokGetToken( t, cursor, singleCharList ) ) {
		return false;
	}
	if ( tokTest( t, expect ) ) {
		return true;
	}
	tokPrint( t );
	return false;
}

void tokSkip( char *&cursor, const char *singleCharList, const char *expect ) {
	token t;
	if ( !tokGetToken( t, cursor, singleCharList ) ) {
		return;
	}
	while ( !tokTest( t, expect ) ) {
		if ( !tokGetToken( t, cursor, singleCharList ) ) {
			return;
		}
	}
}


void tokGetString( char *tok, int toklen, token &t ) {
	int ofs = 0;
	char c0 = tokNextChar( t, ofs );
	char *wp = tok;
	while ( c0 ) {
		if ( (wp-tok) >= toklen ) {
			break;
		}
		*wp++ = c0;
		c0 = tokNextChar( t, ofs );
	}
	tok[toklen-1] = 0;
	*wp = 0;
}

void tokGetString( std::string &s, token &t ) {
	int ofs = 0;
	char c0 = tokNextChar( t, ofs );
	while ( c0 ) {
		s.push_back( c0 );
		c0 = tokNextChar( t, ofs );
	}
}

void tokPrint( token &t ) {
	char *txt = (char*)alloca( t.len + 1 );
	tokGetString( txt, t.len+1, t );
	printf( "tok: %s\n", txt );
}


float tokGetFloat( token &t ) {
	char *tok = (char*)alloca( t.len + 1 );
	tokGetString( tok, t.len+1, t );
	return (float)atof( tok );
}

int tokGetInt( token &t ) {
	char *tok = (char*)alloca( t.len + 1 );
	tokGetString( tok, t.len+1, t );
	return atoi( tok );
}

