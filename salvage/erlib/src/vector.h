#ifndef ER_HEADER_VECTOR_H
#define ER_HEADER_VECTOR_H

#include "assert.h"
#include "align.h"
#include "sort.h"
#include "search.h"

template <class T>
class Vector {

	T *data;
	int size;
	int allocated;

	public:

	Vector() : data(NULL), size(0), allocated(0) {
	}

	Vector( Vector<T> const &o ) : data(NULL), size(0), allocated(0) {
		EnsureAllocated( o.Size() );
		size = o.Size();
		for (int i=0; i<size; i++) {
			data[i] = o[i];
		}
	}

	~Vector() {
		Free();
	}

	void EnsureAllocated( int num, bool copy = true ) {
		int anum = AlignUp( num, 16 );
		if ( anum > allocated ) {
			T *od = data;
			data = new T[anum];
			if ( copy ) {
				for (int i=0; i<size; i++) {
					data[i]  = od[i];
				}
			}
			delete []od;
			allocated = anum;
		}
	}

	void PushBack( const T &item ) {
		EnsureAllocated( size+1 );
		data[size++] = item;
	}

	T & PushBack( void ) {
		EnsureAllocated( size+1 );
		size++;
		return data[size-1];
	}

	Vector<T> & operator=( Vector<T> const &o ) {
		EnsureAllocated( o.Size(), false );
		size = o.Size();
		for (int i=0; i<size; i++) {
			data[i] = o[i];
		}
		return *this;
	}

	T & operator[]( int idx ) {
		ASSERT( idx >= 0 );
		ASSERT( idx < size );
		return data[idx];
	}

	const T &operator[]( int idx ) const {
		ASSERT( idx >= 0 );
		ASSERT( idx < size );
		return data[idx];
	}

	T *Begin( void ) {
		return data;
	}

	const T * Begin( void ) const {
		return data;
	}

	int Size( void ) const {
		return size;
	}

	int Find( const T &thing ) const {
		for (int i=0; i<size; i++) {
			if ( data[i] == thing ) {
				return i;
			}
		}
		return -1;
	}

	void Erase( int idx ) {
		ASSERT( idx >= 0 );
		ASSERT( idx < size );
		size--;
		for (int i=idx; i<size; i++) {
			data[i] = data[i+1];
		}
	}

	void EraseQuick( int idx ) {
		ASSERT( idx >= 0 );
		ASSERT( idx < size );
		size--;
		if ( idx < size ) {
			data[idx] = data[size];
		}
	}

	void Clear( void ) {
		size = 0;
	}

	void Limit( int max ) {
		if ( size > max ) {
			size = max;
		}
	}

	void Free( void ) {
		Clear();
		allocated = 0;
		delete []data;
		data = NULL;
	}
};


template <class T, class S>
class VectorS {

	T *data;
	int size;
	int allocated;
	S compare;

	public:

	VectorS() : data(NULL), size(0), allocated(0) {
	}

	VectorS( VectorS<T,S> const &o ) : data(NULL), size(0), allocated(0) {
		EnsureAllocated( o.Size() );
		size = o.Size();
		for (int i=0; i<size; i++) {
			data[i] = o[i];
		}
	}

	~VectorS() {
		delete []data;
	}

	void EnsureAllocated( int num, bool copy = true ) {
		int anum = AlignUp( num, 16 );
		if ( anum > allocated ) {
			T *od = data;
			data = new T[anum];
			if ( copy ) {
				for (int i=0; i<size; i++) {
					data[i]  = od[i];
				}
			}
			delete []od;
			allocated = anum;
		}
	}

	void PushBack( const T &item ) {
		EnsureAllocated( size+1 );
		data[size++] = item;

		BubbleSort( data, size, compare );
	}

	template <class C>
	int Find( const C &thing ) const {
		return BinarySearch( data, size, thing, compare );
	}

	const T &operator[]( int idx ) const {
		ASSERT( idx >= 0 );
		ASSERT( idx < size );
		return data[idx];
	}

	T *Begin( void ) {
		return data;
	}

	const T * Begin( void ) const {
		return data;
	}

	int Size( void ) const {
		return size;
	}

	void Erase( int idx ) {
		ASSERT( idx >= 0 );
		ASSERT( idx < size );
		size--;
		for (int i=idx; i<size; i++) {
			data[i] = data[i+1];
		}
	}

	void Clear( void ) {
		size = 0;
	}

	VectorS<T,S> & operator=( VectorS<T,S> const &o ) {
		EnsureAllocated( o.Size(), false );
		size = o.Size();
		for (int i=0; i<size; i++) {
			data[i] = o[i];
		}
		return *this;
	}
};

template <class T, int count>
class Array {
	T data[count];
	int size;

	public:

	Array() : size(0) {
	}

	void PushBack( const T &item ) {
		ASSERT( size < count );
		data[size++] = item;
	}


	T & operator[]( int idx ) {
		ASSERT( idx >= 0 );
		ASSERT( idx < size );
		return data[idx];
	}

	const T &operator[]( int idx ) const {
		ASSERT( idx >= 0 );
		ASSERT( idx < size );
		return data[idx];
	}

	T *Begin( void ) {
		return data;
	}

	const T * Begin( void ) const {
		return data;
	}

	int Size( void ) const {
		return size;
	}
};

#endif
