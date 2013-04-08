#ifndef ER_HEADER_JOBMANAGER_H
#define ER_HEADER_JOBMANAGER_H

typedef void (*jobfunc)( void * );

enum JM_PRIORITY_e {
	JMP_IMMEDIATE,
	JMP_MAX
};

class JobManager {
public:
	volatile bool shutdown;

	void Init( void );
	void Deinit( void );
	unsigned int Add( jobfunc func, void *user, JM_PRIORITY_e priority, int *counter );
	void Wait( int *count );
};

#endif
