#pragma once

#include "cexception.h"
#include <string.h>

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
	OPC_PUSHSTR,
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
	OPC_CALLTYPED,
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
	OPC_MAKESTRUCT,
	OPC_MAKESTRUCTG,

	OPC_MAX
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

	void EnsureAllocated( int allocsize )
	{
		int copycount = allocsize < numEntries ? allocsize : numEntries;
		T *newvector = new T[allocsize];
		for (int i=0; i<copycount; i++)
		{
			newvector[i] = vector[i];
		}
		delete []vector;
		vector = newvector;
		allocatedSize = allocsize;
	}

public:

	GrowingArray() : vector(0), allocatedSize(0), numEntries(0)
	{
	}

	~GrowingArray()
	{
	}

	const T &operator[](int index) const
	{
		CEXCEPTION_ERROR_CONDITION( (index >= 0 && index < allocatedSize), "bad array access" );
		return vector[index];
	}
	T &operator[](int index)
	{
		CEXCEPTION_ERROR_CONDITION( (index >= 0 && index < allocatedSize), "bad array access" );
		return vector[index];
	}

	int size() const
	{
		return numEntries;
	}

	void resize( int newsize )
	{
		EnsureAllocated( newsize );
		numEntries = newsize;
	}

	void push_back( const T &i )
	{
		if ( numEntries >= allocatedSize )
		{
			EnsureAllocated( allocatedSize + 16 );
		}
		vector[numEntries++] = i;
	}
};

template <class V>
class Map
{
	int hashStart[256];
	GrowingArray<V> hashValue;
	GrowingArray<int> hashKey;
	GrowingArray<int> hashNext;
public:

	struct Iterator
	{
		V *second;

		inline operator bool()
		{
			return second ? true : false;
		}
	};

	struct ConstIterator
	{
		const V *second;

		inline operator bool()
		{
			return second ? true : false;
		}
	};

	Map()
	{
		memset( hashStart, -1, sizeof(hashStart) );
	}

	~Map()
	{
	}

	V &add( unsigned int key )
	{
		unsigned int index = key % 256;
		int entry = hashValue.size();
		hashValue.push_back( V() );
		hashKey.push_back( key );
		hashNext.push_back( hashStart[index] );
		hashStart[index] = entry;
		return hashValue[entry];
	}

	Iterator find( unsigned int key )
	{
		Iterator nullit = { 0 };
		int index = hashStart[key % 256];
		while ( index != -1 )
		{
			if ( hashKey[index] == key )
			{
				Iterator it;
				it.second = &hashValue[index];
				return it;
			}
			index = hashNext[index];
		}
		return nullit;
	}

	ConstIterator cfind( unsigned int key ) const
	{
		ConstIterator nullit = { 0 };
		int index = hashStart[key % 256];
		while ( index != -1 )
		{
			if ( hashKey[index] == key )
			{
				ConstIterator it;
				it.second = &hashValue[index];
				return it;
			}
			index = hashNext[index];
		}
		return nullit;
	}

	const V &operator[](unsigned int key) const
	{
		ConstIterator f = find(key);
		if ( !f )
		{
			cexecption_error( "failed to find map entry" );
		}
		return *f.second;
	}

	V &operator[](unsigned int key)
	{
		Iterator f = find(key);
		if ( !f )
		{
			return add( key );
		}
		else
		{
			return *f.second;
		}
	}
};


class vmstring
{
	wchar_t m_chars[16];
	int m_len;

public:

	vmstring( wchar_t *s, int len )
	{
		if ( len < 15 )
		{
			wcsncpy( m_chars, s, len );
			m_chars[len] = 0;
		}
		else
		{
			m_chars[0] = 0;
		}
		
		m_len = len;
	}
};

enum VARTYPE
{
	UNKNOWN,
	VOID,
	VMFUNCTION,
	CFUNCTION,

	VARTYPE_SIMPLETYPEBASE,

	INTEGER = VARTYPE_SIMPLETYPEBASE,
	FLOATINGPOINT,
	USERPTR,
	STRING,
	STRUCT,

	VARTYPE_ARRAYBASE,

	INTEGERARRAY = VARTYPE_ARRAYBASE,
	FLOATINGPOINTARRAY,
	USERPTRARRAY,
	STRINGARRAY,
	STRUCTARRAY,

	MAX_VARTYPE,
};

#define VARTYPETOARRAY(x) ((VARTYPE)(((x)-VARTYPE_SIMPLETYPEBASE)+VARTYPE_ARRAYBASE))

template<class T>
struct vmarrayvar
{
	GrowingArray<T> m_items;
};

struct vmstruct;

struct var
{
	VARTYPE type;
	union
	{
		int i;
		float f;
		void *u;
		vmstring *str;
		vmstruct *s;
		vmarrayvar<int> *iArrayPtr;
		vmarrayvar<float> *fArrayPtr;
		vmarrayvar<void*> *uArrayPtr;
		vmarrayvar<vmstring*> *strArrayPtr;
		vmarrayvar<vmstruct*> *sArrayPtr;
		bool (*cfunc)( struct vmstate &state );
	};
};

#pragma warning(push)
#pragma warning(disable: 4200)
struct vmstruct
{
	int m_type;
	int m_size;
	var m_data[0];
};
#pragma warning(pop)


inline int Hash( const wchar_t *v )
{
	unsigned int h = 2166136261;
	while ( *v )
	{
		h = (16777619 * h) ^ (*v);
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

struct vminterface
{
	GrowingArray<int> interfaceFunctions;
};

struct vmelement
{
	int itemName;
	int itemType;
};

struct vmstructprops
{
	GrowingArray<vmelement> properties;
};

struct vmstate
{
	FixedStack<var,1024> stack;
	FixedStack<int,1024> envStack;
	FixedStack<int,32> pcStack;
	Map<var> globals;
	Map<vminterface> ifaces;
	Map<vmstructprops*> structProps;
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


void RunVM( int const *ops, int numOps, int loc, vmstate &state );
bool RunVMExp( int const *ops, int numOps, int loc, vmstate &state );


