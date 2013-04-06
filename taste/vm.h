#pragma once

#include "cexception.h"

enum OPCODES
{
	OPC_ADD,
	OPC_SUB,
	OPC_MUL,
	OPC_DIV,
	OPC_EQU,
	OPC_LSS,
	OPC_GTR,
	OPC_PUSHI,
	OPC_PUSHF,
	OPC_NEG,
	OPC_MAKEVAR,
	OPC_MAKEVARG,
	OPC_CONSTRUCTVAR,
	OPC_CONSTRUCTVARFROMGLOBAL,
	OPC_PUSHTOP,
	OPC_PUSHENV,
	OPC_POPENV,
	OPC_RET,
	OPC_CALL,
	OPC_CALLG,
	OPC_JF,
	OPC_JMP,
	OPC_POP,
	OPC_POPR,
	OPC_PUSHR,
	OPC_PUSHITEM,
	OPC_PUSHITEMG,
	OPC_PUSHITEMARRAY,
	OPC_PUSHITEMGARRAY,
	OPC_POPITEM,
	OPC_POPITEMG,
	OPC_POPITEMARRAY,
	OPC_POPITEMGARRAY,
	OPC_PUSHSUPER,
};

extern const char *opnames[];

template <class T, int maxSize>
class FixedStack
{
	T stack[maxSize];
	int pos;
public:
	FixedStack() : pos(0) {}

	inline T &top() { return stack[pos-1]; }
	inline T const &top() const { return stack[pos-1]; }
	inline int size() { return pos; }
	inline void push( T const &item ) { stack[pos++] = item; }
	inline T& push() { pos++; return stack[pos-1]; }
	inline void pop( int num=1 ) { pos-=num; }
	inline void reserve( int r ) { pos += r; }
	inline void reduce( int r ) { pos -= r; }
	inline void reset( int r ) { pos = r; }
	inline bool empty() { return pos == 0; }
	inline T &operator[]( int index ) { return stack[index]; }
	inline const T &operator[]( int index ) const { return stack[index]; }
};

template <class T> 
class GrowingArray
{
	T *vector;
	int allocatedSize;
	int numEntries;
public:

	GrowingArray() : vector(0), allocatedSize(0), numEntries(0)
	{
	}

	~GrowingArray()
	{
	}

	const T &operator[](int index) const;
	T &operator[](int index);

	int size() const;
	void resize( int newsize );
};

template <class V>
class Map
{
	int hashStart[256];
	GrowingArray<V> hashValue;
	GrowingArray<int> hashNext;
public:

	struct Iterator
	{
		V *second;

		Iterator( V *o ) : second(o) {}

		operator bool()
		{
			return second;
		}
	};

	Map()
	{
		memcpy( hashStart, -1, sizeof(hashStart) );
	}

	~Map()
	{
	}

	Iterator Find( unsigned int key ) const;
	const V &Get( unsigned int key ) const;
	V &Get( unsigned int key );
	V &Add( unsigned int key );
	V &FindOrAdd( unsigned int key );

	void Set( unsigned int key, V &o );
	//const V &operator[](int index) const;
	//V &operator[](int index);
};

enum VARTYPE
{
	INTEGER,
	FLOATINGPOINT,
	OBJECT,
	VMFUNCTION,
	CFUNCTION,
	INTEGERARRAY,
	FLOATINGPOINTARRAY,
	OBJECTARRAY,
};

struct object;

template<class T>
struct arrayvar
{
	GrowingArray<T> m_items;
};


struct var
{
	VARTYPE type;
	union
	{
		int i;
		float f;
		object *o;
		arrayvar<int> *iArrayPtr;
		arrayvar<float> *fArrayPtr;
		arrayvar<object*> *oArrayPtr;
		bool (*cfunc)( struct vmstate &state );
	};
};

struct object
{
	int prototype;
	Map<var> m_tbl;

	object() : prototype(-1) {}
};

inline unsigned int Hash( const wchar_t *v )
{
	unsigned int h = 0x2901381;
	while ( *v )
	{
		h *= (*v) + 101283;
		v++;
	}
	return h;
}


inline int ToInt( var const &v )
{
	switch ( v.type )
	{
	case INTEGER:
		return v.i;
	case FLOATINGPOINT:
		return (int)v.f;
	default:
		return 0;
	};
}

inline float ToFloat( var const &v )
{
	switch ( v.type )
	{
	case INTEGER:
		return (float)v.i;
	case FLOATINGPOINT:
		return v.f;
	default:
		return 0.f;
	};
}

inline VARTYPE CombinedType( VARTYPE t0, VARTYPE t1 )
{
	return ( t0 > t1 ) ? t0 : t1;
}



struct vmstate
{
	FixedStack<var,1024> stack;
	FixedStack<int,1024> envStack;
	FixedStack<int,32> pcStack;
	Map<var> globals;
//	std::vector<object> objects;
//	std::vector<arrayvar<int>*> integerarrays;
//	std::vector<arrayvar<float>*> floatingpointarrays;
//	std::vector<arrayvar<int>*> objectarrays;
	var rv;

	var const &GetArg( int index ) const
	{
		return stack[envStack.top()+index];
	}

	int GetArgAsInt( int index ) const
	{
		var const &v = GetArg(index);
		return ToInt( v );
	}

	float GetArgAsFloat( int index ) const
	{
		var const &v = GetArg(index);
		return ToFloat( v );
	}

	void SetReturn( int v )
	{
		rv.type = INTEGER;
		rv.i = v;
	}

	void SetReturn( float v )
	{
		rv.type = FLOATINGPOINT;
		rv.f = v;
	}
};

