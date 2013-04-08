#ifndef ER_HEADER_ANY_H
#define ER_HEADER_ANY_H

class any {
	struct container {
	};

	template< class T > 
	struct containerT : public container {
		T v;

		containerT( T &_v ) : v(_v) {
		}
	};

	container *c;
public:

	any() : c(NULL) {
	}

	void Clean( void ) {
		delete c;
		c = NULL;
	}

	void Set( float v ) {
		Clean();
		c = new containerT<float>( v );
	}
	void Set( int v ) {
		Clean();
		c = new containerT<int>( v );
	}

	template <class T>
	void Set( T &v ) {
		Clean();
		c = new containerT<T>( v );
	}

};

#endif
