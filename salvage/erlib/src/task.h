#ifndef ER_HEADER_TASK_H
#define ER_HEADER_TASK_H

#include <setjmp.h>


namespace tsk {

struct TASK {
	int id;
	int nextMS;

	jmp_buf state;

	bool running;
	TASK *next;
};


void taskAdd( TASK *t, void *stack, void (*taskfunc)( TASK *t, void *user ), void *user );
void taskRemove( TASK *t );
void taskSleep( int ms );

void taskUpdate( int time );

};

#endif