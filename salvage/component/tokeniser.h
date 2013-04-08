#ifndef TRAIN_TOKENISER_H
#define TRAIN_TOKENISER_H

#include <string>

struct tokeniser
{
	char *cursor;
	const char* singleCharList;
};

struct token {
	const char *start;
	int len;
	bool isQuoted;
};

bool tokIsEndOfLine( char *cursor );
bool tokGetToken( token &t, char *&cursor, const char *singleCharList );
char tokNextChar( token &t, int &ofs );
bool tokTest( token &t, const char *value );
bool tokExpect( char *&cursor, const char *singleCharList, const char *expect );
void tokSkip( char *&cursor, const char *singleCharList, const char *expect );
void tokGetString( char *tok, int toklen, token &t );
void tokGetString( std::string &s, token &t );
void tokPrint( token &t );
float tokGetFloat( token &t );
int tokGetInt( token &t );

inline float tokGetTokenFloat( char *&cursor, const char *singleCharList )
{
	token t;
	if ( tokGetToken( t, cursor, singleCharList ) )
	{
		return tokGetFloat( t );
	} else {
		return 0.f;
	}
}

inline int tokGetTokenInt( char *&cursor, const char *singleCharList )
{
	token t;
	if ( tokGetToken( t, cursor, singleCharList ) )
	{
		return tokGetInt( t );
	} else {
		return 0;
	}
}

inline std::string tokGetTokenString( char *&cursor, const char *singleCharList )
{
	token t;
	if ( tokGetToken( t, cursor, singleCharList ) )
	{
		std::string s;
		tokGetString( s, t );
		return s;
	} else {
		return "";
	}
}

#endif
