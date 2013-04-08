#include "erbase.h"
#include "tokeniser.h"


static char *mystrpbrk (const char *s, const char *accept, const char *singleTokens) {
	bool inQuote = false;
	while (*s != '\0') {
		if ( *s == '"' ) {
			inQuote = !inQuote;
		}

		if ( inQuote == false ) {
			const char *a = accept;
			while (*a != '\0') {
				if (*a++ == *s) {
					return (char *) s;
				}
			}
			const char *st = singleTokens;
			while (*st != '\0') {
				if (*st++ == *s) {
					return (char *) s;
				}
			}
		}
		++s;
	}

	return NULL;
}


void GetToken(Str<char> &str, char **stringp, const char *delim, const char *singleTokens) {
	char *start = *stringp;
	char *ptr;

	str.Empty();

	if (start == NULL) {
		return;
	}

	/* Optimize the case of no delimiters.  */
	if (delim[0] == '\0') {
		*stringp = NULL;
		str = start;
		return;
	}

	const char *st = singleTokens;
	while (*st != '\0') {
		if (*st++ == *start) {
			*stringp = start+1;
			str.Append( *start );
			return;
		}
	}

	/* The general case.  */
	ptr = mystrpbrk (start, delim, singleTokens);

	if (ptr == NULL) {
		*stringp = NULL;
		str = start;
		return;
	}

	*stringp = ptr;
	str.Insert( start, (int)(intptr_t)(ptr - start) );

	return;
}


