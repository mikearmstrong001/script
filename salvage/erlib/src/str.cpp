
#include "str.h"
#include "align.h"

unsigned int StrId::Hash( const char *s ) {
	unsigned int h = 0;
	const char *c = s;
	while (*c ) {
		h = 31*h + *c;
		c++;
	}
	return h;
}
