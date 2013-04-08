#include "jobmanager.h"
#include "math/maths.h"
#include <intrin.h>
#include "windows.h"

static CRITICAL_SECTION		crit;

#define MAX_THREADS 32
static int numThreads = 1;
static DWORD threadID[MAX_THREADS];
static HANDLE threadHandle[MAX_THREADS];
static unsigned int producerID = 0;
static volatile long dummy = 0;

struct worklist {
	struct workitem *head;
	struct workitem *tail;

	worklist() : head(NULL), tail(NULL) {
	}
};

static worklist workitems[ JMP_MAX ];

struct workitem {
	jobfunc func;
	void *user;
	unsigned int id;
	JM_PRIORITY_e priority;
	volatile long *counter;

	workitem *next;
};

bool GetWork( workitem* &item ) {
	EnterCriticalSection (&crit);
	for (int i=0; i<JMP_MAX; i++) {
		if ( workitems[i].head ) {
			item = workitems[i].head;
			workitems[i].head = workitems[i].head->next;
			LeaveCriticalSection (&crit);
			return true;
		}
	}
	LeaveCriticalSection (&crit);
	return false;
};

bool GetWork( workitem* &item, JM_PRIORITY_e p ) {
	EnterCriticalSection (&crit);
	if ( workitems[p].head ) {
		item = workitems[p].head;
		workitems[p].head = workitems[p].head->next;
		LeaveCriticalSection (&crit);
		return true;
	}
	LeaveCriticalSection (&crit);
	return false;
};

static void Worker( void *user ) {
	JobManager *jm = (JobManager*)user;
	while ( !jm->shutdown ) {
		workitem *item;
		if ( GetWork( item ) ) {
			item->func( item->user );
			_InterlockedDecrement( item->counter );
			delete item;
		} else {
			Sleep(0);
		}
	}
}

void JobManager::Init( void ) {
	SYSTEM_INFO info;

	InitializeCriticalSection (&crit);

	GetSystemInfo (&info);
	numThreads = info.dwNumberOfProcessors;
	if ( numThreads < 1 ) {
		numThreads = 1;
	}
	if ( numThreads > MAX_THREADS ) {
		numThreads = MAX_THREADS;
	}

	shutdown = false;
	for (int i=0 ; i<numThreads ; i++)
	{
		threadHandle[i] = CreateThread(
		   NULL,	// LPSECURITY_ATTRIBUTES lpsa,
		   0,		// DWORD cbStack,
		   (LPTHREAD_START_ROUTINE)Worker,	// LPTHREAD_START_ROUTINE lpStartAddr,
		   (LPVOID)this,	// LPVOID lpvThreadParm,
		   0,			//   DWORD fdwCreate,
		   &threadID[i]);
	}

}

void JobManager::Deinit( void ) {
	shutdown = true;
	for (int i=0 ; i<numThreads ; i++) {
		WaitForSingleObject (threadHandle[i], INFINITE);
	}
	DeleteCriticalSection (&crit);
}

unsigned int JobManager::Add( jobfunc func, void *user, JM_PRIORITY_e priority, int *counter ) {
	if ( counter == NULL ) {
		counter = (int*)&dummy;
	}

	workitem *item = new workitem;
	item->func = func;
	item->user = user;
	item->priority = priority;
	item->id = producerID++;
	item->next = NULL;
	item->counter = (volatile long*)counter;
	_InterlockedIncrement( item->counter );
	EnterCriticalSection (&crit);
	if ( workitems[priority].head == NULL ) {
		workitems[priority].head = workitems[priority].tail = item;
	} else {
		workitems[priority].tail->next = item;
		workitems[priority].tail = item;
	}
	LeaveCriticalSection (&crit);
	return item->id;
}

void JobManager::Wait( int *count ) {
	volatile int *c = count;
	while ( *c ) {
		workitem *item;
		if ( GetWork( item, JMP_IMMEDIATE ) ) {
			item->func( item->user );
			_InterlockedDecrement( item->counter );
			delete item;
		} else {
			Sleep(0);
		}
	}
}
