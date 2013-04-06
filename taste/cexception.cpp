#include "cexception.h"
#include <assert.h>

extern void cexception_error( const char *why )
{
	assert( 0 );
}
extern void cexception_error( bool ok, const char *why )
{
	assert(ok);
}
