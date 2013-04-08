#ifndef ER_HEADER_REFCOUNT_H
#define ER_HEADER_REFCOUNT_H

class RefCount {
	int ref;

public:
	RefCount() : ref(0) {
	}

	virtual ~RefCount() {
	}

	virtual void Free() = 0;

	int IncRef( void ) {
		ref++;
		return ref;
	}

	int DecRef( void ) {
		ref--;
		if ( ref == 0 ) {
			Free();
			return 0;
		} else {
			return ref;
		}
	}
};

#endif