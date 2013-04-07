#pragma once

#include <setjmp.h>

struct SetJmpChain_s
{
	jmp_buf env;
	SetJmpChain_s *next;
	char errortxt[64];
};

extern SetJmpChain_s *g_cexception_chain;

extern void cexception_error( const char *why );
extern void cexception_error( bool ok, const char *why );
extern void cexception_push( SetJmpChain_s  *chain );
extern void cexception_pop();
