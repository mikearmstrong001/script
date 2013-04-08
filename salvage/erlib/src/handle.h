#ifndef ER_HEADER_HANDLE_H
#define ER_HEADER_HANDLE_H

template <class T, int S>
class HandleManager {
	struct TableEntry {
		int count;
		T *ptr;
	};

	TableEntry entries[S];

public:
	struct Handle {
		unsigned int id;

		Handle() : id(0) {
		}

		bool operator==( const Handle &other ) const {
			return id == other.id;
		}
	};

	HandleManager() {
		for ( int i=0; i<S; i++) {
			entries[i].count = -1;
		}
	};

	bool GetHandle( Handle &h, T *ptr ) {
		for ( int i=0; i<S; i++) {
			if ( entries[i].count < 0 ) {
				entries[i].count = (-entries[i].count) + 1;
				if ( entries[i].count > 0xffff ) {
					entries[i].count = 1;
				}
				entries[i].ptr = ptr;
				h.id = ((unsigned int)entries[i].count<<16) | i;
				return true;
			}
		}
		return false;
	}

	void FreeHandle( const Handle &h ) {
		int idx = h.id & 0xffff;
		int uid = h.id >> 16;
		if ( entries[idx].count == uid ) {
			entries[idx].count = -entries[idx].count;
			entries[idx].ptr = NULL;
		}
	}

	T* GetPtr( const Handle &h ) {
		int idx = h.id & 0xffff;
		int uid = h.id >> 16;
		if ( entries[idx].count == uid ) {
			return entries[idx].ptr;
		} else {
			return NULL;
		}
	}

	void SetPtr( const Handle &h, T *ptr ) {
		int idx = h.id & 0xffff;
		int uid = h.id >> 16;
		if ( entries[idx].count == uid ) {
			entries[idx].ptr = ptr;
		}
	}
};

#endif