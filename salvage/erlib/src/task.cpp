#include "task.h"
#include <setjmp.h>
#include <stdio.h>


namespace tsk {

static jmp_buf root;
static TASK *list = NULL;

static TASK *running = NULL;

void taskAdd( TASK *t, void *stack, void (*taskfunc)( TASK *t, void *user ), void *user ) {
	t->next = list;
	list = t;

	if ( setjmp( root ) == 0 ) {
		running = t;
		t->running = true;
		__asm mov esp, stack;
		taskfunc( t, user );
		running->running = false;
		running = NULL;
		longjmp( root, 1 );
	}
}

void taskSleep( int ms ) {
	if ( setjmp( running->state ) == 0 ) {
		running = NULL;
		longjmp( root, 1 );
	}
}

void taskUpdate( int dt ) {

	TASK *cur = list;
	TASK *prev = NULL;
	while ( cur ) {
		if ( cur->running ) {
			if ( setjmp( root ) == 0 ) {
				running = cur;
				longjmp( cur->state, 1 );
			}
		} else {
			if ( prev ) {
				prev->next = cur->next;
			} else {
				list = cur->next;
			}
		}
		cur = cur->next;
	}
}

}