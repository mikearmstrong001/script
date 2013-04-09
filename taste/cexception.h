#pragma once

#include <setjmp.h>

struct SetJmpChain_s
{
	jmp_buf env;
	SetJmpChain_s *next;
	char errortxt[64];
	const char *file;
	int line;
};

extern SetJmpChain_s *g_cexception_chain;

extern void cexception_error( const char *why, const char *file, int line );
extern void cexception_error( bool ok, const char *why, const char *file, int line );
extern void cexception_push( SetJmpChain_s  *chain );
extern void cexception_pop();

#define CEXCEPTION_ERROR(why) cexception_error(why,__FILE__,__LINE__)
#define CEXCEPTION_ERROR_CONDITION(test,why) cexception_error((test),why,__FILE__,__LINE__)
