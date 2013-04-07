#include "cexception.h"
#include <assert.h>
#include <string.h>

extern SetJmpChain_s *g_cexception_chain = 0;

void cexception_error( const char *why )
{
	strncpy( g_cexception_chain->errortxt, why, sizeof(g_cexception_chain->errortxt) );
	longjmp( g_cexception_chain->env, -1 );
}
void cexception_error( bool ok, const char *why )
{
	if ( !ok )
	{
		cexception_error( why );
	}
}

void cexception_push( SetJmpChain_s  *chain )
{
	chain->next = g_cexception_chain;
	g_cexception_chain = chain;
}

void cexception_pop()
{
	assert( g_cexception_chain );
	g_cexception_chain = g_cexception_chain->next;
}
