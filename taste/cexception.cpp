#include "cexception.h"
#include <assert.h>
#include <string.h>

extern SetJmpChain_s *g_cexception_chain = 0;

void cexception_error( const char *why, const char *file, int line )
{
	strncpy( g_cexception_chain->errortxt, why, sizeof(g_cexception_chain->errortxt) );
	g_cexception_chain->file = file;
	g_cexception_chain->line = line;
	longjmp( g_cexception_chain->env, -1 );
}
void cexception_error( bool ok, const char *why, const char *file, int line )
{
	if ( !ok )
	{
		cexception_error( why, file, line );
	}
}

void cexception_push( SetJmpChain_s  *chain )
{
	chain->next = g_cexception_chain;
	g_cexception_chain = chain;
	g_cexception_chain->errortxt[0] = 0;
	g_cexception_chain->file = 0;
	g_cexception_chain->line = 0;
}

void cexception_pop()
{
	assert( g_cexception_chain );
	g_cexception_chain = g_cexception_chain->next;
}
