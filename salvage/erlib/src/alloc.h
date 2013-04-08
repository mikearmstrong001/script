#ifndef ER_HEADER_ALLOC_H
#define ER_HEADER_ALLOC_H

template <class T, int chunk>
class PoolAllocator {

   struct block {
       char data[chunk * sizeof(T)];
       unsigned short free[chunk];
       int numFree;
       block *next;

       block() : numFree( chunk ) {
           for (int i=0; i<chunk; i++) {
               free[i] = i;
           }
       }

       ~block() {
           bool used[chunk];
           memset( used, 0, sizeof( used ) );
           for (int i=0; i<numFree; i++) {
               used[free[i]] = true;
           }
           for (int i=0; i<chunk; i++) {
               if ( !used[i] ) {
                   T *p = (T*)&data[ i * sizeof(T) ];
                   p->~T();
               }
           }
       }

       T *Alloc( void ) {
           if ( numFree ) {
               numFree--;

               int idx = free[numFree];
               char *ptr = &data[ sizeof(T) * idx ];

               return new(ptr) T;
           }
           return NULL;
       }

       bool Free( T *ptr ) {
           char *cptr = (char*)ptr;
           if ( cptr >= data && cptr < &data[ chunk * sizeof(T) ] ) {
               ptr->~T();
               int idx = ptr - (T*)data;
               free[numFree++] = idx;
               return true;
           } else {
               return false;
           }
       }
   };

   block *blocks;

public:

   PoolAllocator() : blocks(NULL) {
   }

   ~PoolAllocator() {
       block *c = blocks;
       while ( c ) {
           block *n = c->next;
           delete c;
           c = n;
       }
   }

   T *Alloc( void ) {
       block *c = blocks;
       while ( c ) {
           T *p = c->Alloc();
           if ( p != NULL ) {
               return p;
           }
           c = c->next;
       }

       c = new block;
       c->next = blocks;
       blocks = c;

       return c->Alloc();
   }

   void Free( T*p ) {
       block *c = blocks;
       while ( c ) {
           if ( c->Free( p ) ) {
               return;
           }
           c = c->next;
       }
   }
};

template <int S, int MAX>
class BlockAllocate {

   char *data;
   unsigned char free[ MAX ];
   int  lastFreeIndex;

public:

   BlockAllocate() : lastFreeIndex(0) {
       data = new char[ S * MAX ];
       memset( free, 0xff, sizeof(free) );
   }

   ~BlockAllocate() {
       delete []data;
   }

   void *Alloc( void ) {
       for (int i=lastFreeIndex; i<MAX; i++) {
           if ( free[ i/8 ] & (1<<(i&7)) ) {
               free[i/8] &= ~(1<<(i&7));
               lastFreeIndex = i+1;
               return &data[ i * MAX ];
           }
       }
       for (int i=0; i<lastFreeIndex; i++) {
           if ( free[ i/8 ] & (1<<(i&7)) ) {
               free[i/8] &= ~(1<<(i&7));
               lastFreeIndex = i;
               return &data[ i * MAX ];
           }
       }
       return NULL;
   }

   void Free( void *p ) {
       char *cp = (char*)p;
       int i = (cp - data) / MAX;
       free[i/8] |= (1<<(i&7));
       if ( i < lastFreeIndex ) {
           lastFreeIndex = i;
       }
   }
};

class HeapAllocator {
	unsigned char *mem;
	int size;
	unsigned char *ptr;
public:

	HeapAllocator() : mem(NULL), ptr(NULL), size(0) {
	}

	HeapAllocator( int _size ) {
		Init( _size );
	}

	~HeapAllocator() {
		delete []mem;
	}

	void Init( int _size ) {
		mem = new unsigned char[ _size ];
		ptr = mem;
		size = _size;
	}

	void *Alloc( int s ) {
		s = AlignUp( s, 16 );

		int c = ptr - mem;
		if ( (c + s) > size ) {
			return NULL;
		}

		void *ret = ptr;
		ptr += s;

		return ret;
	}

	void *Calloc( int s ) {
		void *mem = Alloc( s );
		if ( mem ) {
			memset( mem, 0, s );
		}
		return mem;
	}

	void Reset( void ) {
		ptr = mem;
	}
};

#endif