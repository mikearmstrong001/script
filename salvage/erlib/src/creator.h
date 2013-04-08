#ifndef ER_HEADER_CREATOR_H
#define ER_HEADER_CREATOR_H

template <class T>
class Creator {

	static Creator	*s_head;

	const char *name;
	Creator *next;
public:
	Creator( const char *n ) {
		name = n;
		next = s_head;
		s_head = this;
	}

	virtual T *Create() = 0;
	virtual T *Create( void *ptr, int &size ) = 0;

	static T *Create( const char *name ) {
		Creator *c = s_head;
		while ( c ) {
			if ( strcmp( c->name, name ) == 0 ) {
				return c->Create();
			}
			c = c->next;
		}
		return NULL;
	}

	static T *Create( void *ptr, int &size, const char *name ) {
		Creator *c = s_head;
		while ( c ) {
			if ( strcmp( c->name, name ) == 0 ) {
				return c->Create( ptr, size );
			}
			c = c->next;
		}
		return NULL;
	}
};

template <class T>
Creator<T>	*Creator<T>::s_head = NULL;


#endif

