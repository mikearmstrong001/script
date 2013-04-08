#ifndef ER_HEADER_TOKENISER_H
#define ER_HEADER_TOKENISER_H


#include "str.h"
void GetToken(Str<char> &str, char **stringp, const char *delim, const char *singleTokens);

#endif