#ifndef TRAIN_HASHNAME_H
#define TRAIN_HASHNAME_H

#include "stdint.h"
#include <string>

class HashName
{
	uint32_t m_id;
	std::string m_debugName;
public:

	inline HashName() : m_id(0) {}

	inline explicit HashName( uint32_t id ) : m_id(id) {}
	inline explicit HashName( uint32_t id, const char *dn ) : m_id(id), m_debugName(dn) {}

	inline explicit HashName( const char (&a)[1] );
	inline explicit HashName( const char (&a)[2] );
	inline explicit HashName( const char (&a)[3] );
	inline explicit HashName( const char (&a)[4] );
	inline explicit HashName( const char (&a)[5] );
	inline explicit HashName( const char (&a)[6] );
	inline explicit HashName( const char (&a)[7] );
	inline explicit HashName( const char (&a)[8] );
	inline explicit HashName( const char (&a)[9] );
	inline explicit HashName( const char (&a)[10] );
	inline explicit HashName( const char (&a)[11] );
	inline explicit HashName( const char (&a)[12] );
	inline explicit HashName( const char (&a)[13] );
	inline explicit HashName( const char (&a)[14] );
	inline explicit HashName( const char (&a)[15] );
	inline explicit HashName( const char (&a)[16] );
	inline explicit HashName( const char (&a)[17] );
	inline explicit HashName( const char (&a)[18] );
	inline explicit HashName( const char (&a)[19] );
	inline explicit HashName( const char (&a)[20] );
	inline explicit HashName( const char (&a)[21] );
	inline explicit HashName( const char (&a)[22] );
	inline explicit HashName( const char (&a)[23] );
	inline explicit HashName( const char (&a)[24] );
	inline explicit HashName( const char (&a)[25] );
	inline explicit HashName( const char (&a)[26] );
	inline explicit HashName( const char (&a)[27] );
	inline explicit HashName( const char (&a)[28] );
	inline explicit HashName( const char (&a)[29] );
	inline explicit HashName( const char (&a)[30] );
	inline explicit HashName( const char (&a)[31] );
	inline explicit HashName( const char (&a)[32] );

	inline uint32_t get() const { return m_id; }

	bool operator==( const HashName &o ) const
	{
		return get() == o.get();
	}

	bool operator!=( const HashName &o ) const
	{
		return get() != o.get();
	}

	bool operator<( const HashName &o ) const
	{
		return get() < o.get();
	}

	static HashName FromCStr( const char *str )
	{
		uint32_t id = 0;
		while ( *str )
		{
			id = (id * 65599) + str[0];
			str++;
		}
		return HashName(id);
	}
};

HashName::HashName( const char (&a)[2] )
{
	m_id = a[0];
}
HashName::HashName( const char (&a)[3] )
{
	m_id = a[0];
	m_id = m_id * 65599 + a[1];
	m_debugName = a;
}
HashName::HashName( const char (&a)[4] )
{
	m_id = a[0];
	m_id = m_id * 65599 + a[1];
	m_id = m_id * 65599 + a[2];
	m_debugName = a;
}
HashName::HashName( const char (&a)[5] )
{
	m_id = a[0];
	m_id = m_id * 65599 + a[1];
	m_id = m_id * 65599 + a[2];
	m_id = m_id * 65599 + a[3];
	m_debugName = a;
}
HashName::HashName( const char (&a)[6] )
{
	m_id = a[0];
	m_id = m_id * 65599 + a[1];
	m_id = m_id * 65599 + a[2];
	m_id = m_id * 65599 + a[3];
	m_id = m_id * 65599 + a[4];
	m_debugName = a;
}
HashName::HashName( const char (&a)[7] )
{
	m_id = a[0];
	m_id = m_id * 65599 + a[1];
	m_id = m_id * 65599 + a[2];
	m_id = m_id * 65599 + a[3];
	m_id = m_id * 65599 + a[4];
	m_id = m_id * 65599 + a[5];
	m_debugName = a;
}
HashName::HashName( const char (&a)[8] )
{
	m_id = a[0];
	m_id = m_id * 65599 + a[1];
	m_id = m_id * 65599 + a[2];
	m_id = m_id * 65599 + a[3];
	m_id = m_id * 65599 + a[4];
	m_id = m_id * 65599 + a[5];
	m_id = m_id * 65599 + a[6];
	m_debugName = a;
}
HashName::HashName( const char (&a)[9] )
{
	m_id = a[0];
	m_id = m_id * 65599 + a[1];
	m_id = m_id * 65599 + a[2];
	m_id = m_id * 65599 + a[3];
	m_id = m_id * 65599 + a[4];
	m_id = m_id * 65599 + a[5];
	m_id = m_id * 65599 + a[6];
	m_id = m_id * 65599 + a[7];
	m_debugName = a;
}
HashName::HashName( const char (&a)[10] )
{
	m_id = a[0];
	m_id = m_id * 65599 + a[1];
	m_id = m_id * 65599 + a[2];
	m_id = m_id * 65599 + a[3];
	m_id = m_id * 65599 + a[4];
	m_id = m_id * 65599 + a[5];
	m_id = m_id * 65599 + a[6];
	m_id = m_id * 65599 + a[7];
	m_id = m_id * 65599 + a[8];
	m_debugName = a;
}
HashName::HashName( const char (&a)[11] )
{
	m_id = a[0];
	m_id = m_id * 65599 + a[1];
	m_id = m_id * 65599 + a[2];
	m_id = m_id * 65599 + a[3];
	m_id = m_id * 65599 + a[4];
	m_id = m_id * 65599 + a[5];
	m_id = m_id * 65599 + a[6];
	m_id = m_id * 65599 + a[7];
	m_id = m_id * 65599 + a[8];
	m_id = m_id * 65599 + a[9];
	m_debugName = a;
}
HashName::HashName( const char (&a)[12] )
{
	m_id = a[0];
	m_id = m_id * 65599 + a[1];
	m_id = m_id * 65599 + a[2];
	m_id = m_id * 65599 + a[3];
	m_id = m_id * 65599 + a[4];
	m_id = m_id * 65599 + a[5];
	m_id = m_id * 65599 + a[6];
	m_id = m_id * 65599 + a[7];
	m_id = m_id * 65599 + a[8];
	m_id = m_id * 65599 + a[9];
	m_id = m_id * 65599 + a[10];
	m_debugName = a;
}
HashName::HashName( const char (&a)[13] )
{
	m_id = a[0];
	m_id = m_id * 65599 + a[1];
	m_id = m_id * 65599 + a[2];
	m_id = m_id * 65599 + a[3];
	m_id = m_id * 65599 + a[4];
	m_id = m_id * 65599 + a[5];
	m_id = m_id * 65599 + a[6];
	m_id = m_id * 65599 + a[7];
	m_id = m_id * 65599 + a[8];
	m_id = m_id * 65599 + a[9];
	m_id = m_id * 65599 + a[10];
	m_id = m_id * 65599 + a[11];
	m_debugName = a;
}
HashName::HashName( const char (&a)[14] )
{
	m_id = a[0];
	m_id = m_id * 65599 + a[1];
	m_id = m_id * 65599 + a[2];
	m_id = m_id * 65599 + a[3];
	m_id = m_id * 65599 + a[4];
	m_id = m_id * 65599 + a[5];
	m_id = m_id * 65599 + a[6];
	m_id = m_id * 65599 + a[7];
	m_id = m_id * 65599 + a[8];
	m_id = m_id * 65599 + a[9];
	m_id = m_id * 65599 + a[10];
	m_id = m_id * 65599 + a[11];
	m_id = m_id * 65599 + a[12];
	m_debugName = a;
}
HashName::HashName( const char (&a)[15] )
{
	m_id = a[0];
	m_id = m_id * 65599 + a[1];
	m_id = m_id * 65599 + a[2];
	m_id = m_id * 65599 + a[3];
	m_id = m_id * 65599 + a[4];
	m_id = m_id * 65599 + a[5];
	m_id = m_id * 65599 + a[6];
	m_id = m_id * 65599 + a[7];
	m_id = m_id * 65599 + a[8];
	m_id = m_id * 65599 + a[9];
	m_id = m_id * 65599 + a[10];
	m_id = m_id * 65599 + a[11];
	m_id = m_id * 65599 + a[12];
	m_id = m_id * 65599 + a[13];
	m_debugName = a;
}
HashName::HashName( const char (&a)[16] )
{
	m_id = a[0];
	m_id = m_id * 65599 + a[1];
	m_id = m_id * 65599 + a[2];
	m_id = m_id * 65599 + a[3];
	m_id = m_id * 65599 + a[4];
	m_id = m_id * 65599 + a[5];
	m_id = m_id * 65599 + a[6];
	m_id = m_id * 65599 + a[7];
	m_id = m_id * 65599 + a[8];
	m_id = m_id * 65599 + a[9];
	m_id = m_id * 65599 + a[10];
	m_id = m_id * 65599 + a[11];
	m_id = m_id * 65599 + a[12];
	m_id = m_id * 65599 + a[13];
	m_id = m_id * 65599 + a[14];
	m_debugName = a;
}
HashName::HashName( const char (&a)[17] )
{
	m_id = a[0];
	m_id = m_id * 65599 + a[1];
	m_id = m_id * 65599 + a[2];
	m_id = m_id * 65599 + a[3];
	m_id = m_id * 65599 + a[4];
	m_id = m_id * 65599 + a[5];
	m_id = m_id * 65599 + a[6];
	m_id = m_id * 65599 + a[7];
	m_id = m_id * 65599 + a[8];
	m_id = m_id * 65599 + a[9];
	m_id = m_id * 65599 + a[10];
	m_id = m_id * 65599 + a[11];
	m_id = m_id * 65599 + a[12];
	m_id = m_id * 65599 + a[13];
	m_id = m_id * 65599 + a[14];
	m_id = m_id * 65599 + a[15];
	m_debugName = a;
}
HashName::HashName( const char (&a)[18] )
{
	m_id = a[0];
	m_id = m_id * 65599 + a[1];
	m_id = m_id * 65599 + a[2];
	m_id = m_id * 65599 + a[3];
	m_id = m_id * 65599 + a[4];
	m_id = m_id * 65599 + a[5];
	m_id = m_id * 65599 + a[6];
	m_id = m_id * 65599 + a[7];
	m_id = m_id * 65599 + a[8];
	m_id = m_id * 65599 + a[9];
	m_id = m_id * 65599 + a[10];
	m_id = m_id * 65599 + a[11];
	m_id = m_id * 65599 + a[12];
	m_id = m_id * 65599 + a[13];
	m_id = m_id * 65599 + a[14];
	m_id = m_id * 65599 + a[15];
	m_id = m_id * 65599 + a[16];
	m_debugName = a;
}
HashName::HashName( const char (&a)[19] )
{
	m_id = a[0];
	m_id = m_id * 65599 + a[1];
	m_id = m_id * 65599 + a[2];
	m_id = m_id * 65599 + a[3];
	m_id = m_id * 65599 + a[4];
	m_id = m_id * 65599 + a[5];
	m_id = m_id * 65599 + a[6];
	m_id = m_id * 65599 + a[7];
	m_id = m_id * 65599 + a[8];
	m_id = m_id * 65599 + a[9];
	m_id = m_id * 65599 + a[10];
	m_id = m_id * 65599 + a[11];
	m_id = m_id * 65599 + a[12];
	m_id = m_id * 65599 + a[13];
	m_id = m_id * 65599 + a[14];
	m_id = m_id * 65599 + a[15];
	m_id = m_id * 65599 + a[16];
	m_id = m_id * 65599 + a[17];
	m_debugName = a;
}
HashName::HashName( const char (&a)[20] )
{
	m_id = a[0];
	m_id = m_id * 65599 + a[1];
	m_id = m_id * 65599 + a[2];
	m_id = m_id * 65599 + a[3];
	m_id = m_id * 65599 + a[4];
	m_id = m_id * 65599 + a[5];
	m_id = m_id * 65599 + a[6];
	m_id = m_id * 65599 + a[7];
	m_id = m_id * 65599 + a[8];
	m_id = m_id * 65599 + a[9];
	m_id = m_id * 65599 + a[10];
	m_id = m_id * 65599 + a[11];
	m_id = m_id * 65599 + a[12];
	m_id = m_id * 65599 + a[13];
	m_id = m_id * 65599 + a[14];
	m_id = m_id * 65599 + a[15];
	m_id = m_id * 65599 + a[16];
	m_id = m_id * 65599 + a[17];
	m_id = m_id * 65599 + a[18];
	m_debugName = a;
}
HashName::HashName( const char (&a)[21] )
{
	m_id = a[0];
	m_id = m_id * 65599 + a[1];
	m_id = m_id * 65599 + a[2];
	m_id = m_id * 65599 + a[3];
	m_id = m_id * 65599 + a[4];
	m_id = m_id * 65599 + a[5];
	m_id = m_id * 65599 + a[6];
	m_id = m_id * 65599 + a[7];
	m_id = m_id * 65599 + a[8];
	m_id = m_id * 65599 + a[9];
	m_id = m_id * 65599 + a[10];
	m_id = m_id * 65599 + a[11];
	m_id = m_id * 65599 + a[12];
	m_id = m_id * 65599 + a[13];
	m_id = m_id * 65599 + a[14];
	m_id = m_id * 65599 + a[15];
	m_id = m_id * 65599 + a[16];
	m_id = m_id * 65599 + a[17];
	m_id = m_id * 65599 + a[18];
	m_id = m_id * 65599 + a[19];
	m_debugName = a;
}
HashName::HashName( const char (&a)[22] )
{
	m_id = a[0];
	m_id = m_id * 65599 + a[1];
	m_id = m_id * 65599 + a[2];
	m_id = m_id * 65599 + a[3];
	m_id = m_id * 65599 + a[4];
	m_id = m_id * 65599 + a[5];
	m_id = m_id * 65599 + a[6];
	m_id = m_id * 65599 + a[7];
	m_id = m_id * 65599 + a[8];
	m_id = m_id * 65599 + a[9];
	m_id = m_id * 65599 + a[10];
	m_id = m_id * 65599 + a[11];
	m_id = m_id * 65599 + a[12];
	m_id = m_id * 65599 + a[13];
	m_id = m_id * 65599 + a[14];
	m_id = m_id * 65599 + a[15];
	m_id = m_id * 65599 + a[16];
	m_id = m_id * 65599 + a[17];
	m_id = m_id * 65599 + a[18];
	m_id = m_id * 65599 + a[19];
	m_id = m_id * 65599 + a[20];
	m_debugName = a;
}
HashName::HashName( const char (&a)[23] )
{
	m_id = a[0];
	m_id = m_id * 65599 + a[1];
	m_id = m_id * 65599 + a[2];
	m_id = m_id * 65599 + a[3];
	m_id = m_id * 65599 + a[4];
	m_id = m_id * 65599 + a[5];
	m_id = m_id * 65599 + a[6];
	m_id = m_id * 65599 + a[7];
	m_id = m_id * 65599 + a[8];
	m_id = m_id * 65599 + a[9];
	m_id = m_id * 65599 + a[10];
	m_id = m_id * 65599 + a[11];
	m_id = m_id * 65599 + a[12];
	m_id = m_id * 65599 + a[13];
	m_id = m_id * 65599 + a[14];
	m_id = m_id * 65599 + a[15];
	m_id = m_id * 65599 + a[16];
	m_id = m_id * 65599 + a[17];
	m_id = m_id * 65599 + a[18];
	m_id = m_id * 65599 + a[19];
	m_id = m_id * 65599 + a[20];
	m_id = m_id * 65599 + a[21];
	m_debugName = a;
}
HashName::HashName( const char (&a)[24] )
{
	m_id = a[0];
	m_id = m_id * 65599 + a[1];
	m_id = m_id * 65599 + a[2];
	m_id = m_id * 65599 + a[3];
	m_id = m_id * 65599 + a[4];
	m_id = m_id * 65599 + a[5];
	m_id = m_id * 65599 + a[6];
	m_id = m_id * 65599 + a[7];
	m_id = m_id * 65599 + a[8];
	m_id = m_id * 65599 + a[9];
	m_id = m_id * 65599 + a[10];
	m_id = m_id * 65599 + a[11];
	m_id = m_id * 65599 + a[12];
	m_id = m_id * 65599 + a[13];
	m_id = m_id * 65599 + a[14];
	m_id = m_id * 65599 + a[15];
	m_id = m_id * 65599 + a[16];
	m_id = m_id * 65599 + a[17];
	m_id = m_id * 65599 + a[18];
	m_id = m_id * 65599 + a[19];
	m_id = m_id * 65599 + a[20];
	m_id = m_id * 65599 + a[21];
	m_id = m_id * 65599 + a[22];
	m_debugName = a;
}
HashName::HashName( const char (&a)[25] )
{
	m_id = a[0];
	m_id = m_id * 65599 + a[1];
	m_id = m_id * 65599 + a[2];
	m_id = m_id * 65599 + a[3];
	m_id = m_id * 65599 + a[4];
	m_id = m_id * 65599 + a[5];
	m_id = m_id * 65599 + a[6];
	m_id = m_id * 65599 + a[7];
	m_id = m_id * 65599 + a[8];
	m_id = m_id * 65599 + a[9];
	m_id = m_id * 65599 + a[10];
	m_id = m_id * 65599 + a[11];
	m_id = m_id * 65599 + a[12];
	m_id = m_id * 65599 + a[13];
	m_id = m_id * 65599 + a[14];
	m_id = m_id * 65599 + a[15];
	m_id = m_id * 65599 + a[16];
	m_id = m_id * 65599 + a[17];
	m_id = m_id * 65599 + a[18];
	m_id = m_id * 65599 + a[19];
	m_id = m_id * 65599 + a[20];
	m_id = m_id * 65599 + a[21];
	m_id = m_id * 65599 + a[22];
	m_id = m_id * 65599 + a[23];
	m_debugName = a;
}
HashName::HashName( const char (&a)[26] )
{
	m_id = a[0];
	m_id = m_id * 65599 + a[1];
	m_id = m_id * 65599 + a[2];
	m_id = m_id * 65599 + a[3];
	m_id = m_id * 65599 + a[4];
	m_id = m_id * 65599 + a[5];
	m_id = m_id * 65599 + a[6];
	m_id = m_id * 65599 + a[7];
	m_id = m_id * 65599 + a[8];
	m_id = m_id * 65599 + a[9];
	m_id = m_id * 65599 + a[10];
	m_id = m_id * 65599 + a[11];
	m_id = m_id * 65599 + a[12];
	m_id = m_id * 65599 + a[13];
	m_id = m_id * 65599 + a[14];
	m_id = m_id * 65599 + a[15];
	m_id = m_id * 65599 + a[16];
	m_id = m_id * 65599 + a[17];
	m_id = m_id * 65599 + a[18];
	m_id = m_id * 65599 + a[19];
	m_id = m_id * 65599 + a[20];
	m_id = m_id * 65599 + a[21];
	m_id = m_id * 65599 + a[22];
	m_id = m_id * 65599 + a[23];
	m_id = m_id * 65599 + a[24];
	m_debugName = a;
}
HashName::HashName( const char (&a)[27] )
{
	m_id = a[0];
	m_id = m_id * 65599 + a[1];
	m_id = m_id * 65599 + a[2];
	m_id = m_id * 65599 + a[3];
	m_id = m_id * 65599 + a[4];
	m_id = m_id * 65599 + a[5];
	m_id = m_id * 65599 + a[6];
	m_id = m_id * 65599 + a[7];
	m_id = m_id * 65599 + a[8];
	m_id = m_id * 65599 + a[9];
	m_id = m_id * 65599 + a[10];
	m_id = m_id * 65599 + a[11];
	m_id = m_id * 65599 + a[12];
	m_id = m_id * 65599 + a[13];
	m_id = m_id * 65599 + a[14];
	m_id = m_id * 65599 + a[15];
	m_id = m_id * 65599 + a[16];
	m_id = m_id * 65599 + a[17];
	m_id = m_id * 65599 + a[18];
	m_id = m_id * 65599 + a[19];
	m_id = m_id * 65599 + a[20];
	m_id = m_id * 65599 + a[21];
	m_id = m_id * 65599 + a[22];
	m_id = m_id * 65599 + a[23];
	m_id = m_id * 65599 + a[24];
	m_id = m_id * 65599 + a[25];
	m_debugName = a;
}
HashName::HashName( const char (&a)[28] )
{
	m_id = a[0];
	m_id = m_id * 65599 + a[1];
	m_id = m_id * 65599 + a[2];
	m_id = m_id * 65599 + a[3];
	m_id = m_id * 65599 + a[4];
	m_id = m_id * 65599 + a[5];
	m_id = m_id * 65599 + a[6];
	m_id = m_id * 65599 + a[7];
	m_id = m_id * 65599 + a[8];
	m_id = m_id * 65599 + a[9];
	m_id = m_id * 65599 + a[10];
	m_id = m_id * 65599 + a[11];
	m_id = m_id * 65599 + a[12];
	m_id = m_id * 65599 + a[13];
	m_id = m_id * 65599 + a[14];
	m_id = m_id * 65599 + a[15];
	m_id = m_id * 65599 + a[16];
	m_id = m_id * 65599 + a[17];
	m_id = m_id * 65599 + a[18];
	m_id = m_id * 65599 + a[19];
	m_id = m_id * 65599 + a[20];
	m_id = m_id * 65599 + a[21];
	m_id = m_id * 65599 + a[22];
	m_id = m_id * 65599 + a[23];
	m_id = m_id * 65599 + a[24];
	m_id = m_id * 65599 + a[25];
	m_id = m_id * 65599 + a[26];
	m_debugName = a;
}
HashName::HashName( const char (&a)[29] )
{
	m_id = a[0];
	m_id = m_id * 65599 + a[1];
	m_id = m_id * 65599 + a[2];
	m_id = m_id * 65599 + a[3];
	m_id = m_id * 65599 + a[4];
	m_id = m_id * 65599 + a[5];
	m_id = m_id * 65599 + a[6];
	m_id = m_id * 65599 + a[7];
	m_id = m_id * 65599 + a[8];
	m_id = m_id * 65599 + a[9];
	m_id = m_id * 65599 + a[10];
	m_id = m_id * 65599 + a[11];
	m_id = m_id * 65599 + a[12];
	m_id = m_id * 65599 + a[13];
	m_id = m_id * 65599 + a[14];
	m_id = m_id * 65599 + a[15];
	m_id = m_id * 65599 + a[16];
	m_id = m_id * 65599 + a[17];
	m_id = m_id * 65599 + a[18];
	m_id = m_id * 65599 + a[19];
	m_id = m_id * 65599 + a[20];
	m_id = m_id * 65599 + a[21];
	m_id = m_id * 65599 + a[22];
	m_id = m_id * 65599 + a[23];
	m_id = m_id * 65599 + a[24];
	m_id = m_id * 65599 + a[25];
	m_id = m_id * 65599 + a[26];
	m_id = m_id * 65599 + a[27];
	m_debugName = a;
}
HashName::HashName( const char (&a)[30] )
{
	m_id = a[0];
	m_id = m_id * 65599 + a[1];
	m_id = m_id * 65599 + a[2];
	m_id = m_id * 65599 + a[3];
	m_id = m_id * 65599 + a[4];
	m_id = m_id * 65599 + a[5];
	m_id = m_id * 65599 + a[6];
	m_id = m_id * 65599 + a[7];
	m_id = m_id * 65599 + a[8];
	m_id = m_id * 65599 + a[9];
	m_id = m_id * 65599 + a[10];
	m_id = m_id * 65599 + a[11];
	m_id = m_id * 65599 + a[12];
	m_id = m_id * 65599 + a[13];
	m_id = m_id * 65599 + a[14];
	m_id = m_id * 65599 + a[15];
	m_id = m_id * 65599 + a[16];
	m_id = m_id * 65599 + a[17];
	m_id = m_id * 65599 + a[18];
	m_id = m_id * 65599 + a[19];
	m_id = m_id * 65599 + a[20];
	m_id = m_id * 65599 + a[21];
	m_id = m_id * 65599 + a[22];
	m_id = m_id * 65599 + a[23];
	m_id = m_id * 65599 + a[24];
	m_id = m_id * 65599 + a[25];
	m_id = m_id * 65599 + a[26];
	m_id = m_id * 65599 + a[27];
	m_id = m_id * 65599 + a[28];
	m_debugName = a;
}
HashName::HashName( const char (&a)[31] )
{
	m_id = a[0];
	m_id = m_id * 65599 + a[1];
	m_id = m_id * 65599 + a[2];
	m_id = m_id * 65599 + a[3];
	m_id = m_id * 65599 + a[4];
	m_id = m_id * 65599 + a[5];
	m_id = m_id * 65599 + a[6];
	m_id = m_id * 65599 + a[7];
	m_id = m_id * 65599 + a[8];
	m_id = m_id * 65599 + a[9];
	m_id = m_id * 65599 + a[10];
	m_id = m_id * 65599 + a[11];
	m_id = m_id * 65599 + a[12];
	m_id = m_id * 65599 + a[13];
	m_id = m_id * 65599 + a[14];
	m_id = m_id * 65599 + a[15];
	m_id = m_id * 65599 + a[16];
	m_id = m_id * 65599 + a[17];
	m_id = m_id * 65599 + a[18];
	m_id = m_id * 65599 + a[19];
	m_id = m_id * 65599 + a[20];
	m_id = m_id * 65599 + a[21];
	m_id = m_id * 65599 + a[22];
	m_id = m_id * 65599 + a[23];
	m_id = m_id * 65599 + a[24];
	m_id = m_id * 65599 + a[25];
	m_id = m_id * 65599 + a[26];
	m_id = m_id * 65599 + a[27];
	m_id = m_id * 65599 + a[28];
	m_id = m_id * 65599 + a[29];
	m_debugName = a;
}

#endif
