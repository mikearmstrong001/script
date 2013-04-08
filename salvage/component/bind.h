#ifndef TESTING_BIND_H
#define TESTING_BIND_H

#define _CRT_SECURE_NO_WARNINGS
#define _USE_MATH_DEFINES
#define _CRT_NONSTDC_NO_WARNINGS

#include <string>
#include "vectormath/cpp/vectormath_aos.h"


inline void Bind( const char* &out, const char *data ) { out = data; }
inline void Bind( std::string &out, const char *data ) { out = data; }
inline void Bind( int &out, const char *data ) { out = atoi(data); }
inline void Bind( unsigned int &out, const char *data ) { out = atoi(data); }
inline void Bind( float &out, const char *data ) { out = (float)atof(data); }
inline void Bind( bool &out, const char *data )
{
	out = ( strcmp( data, "1" ) == 0 ) | (strcmp( data, "true" ) == 0 );
}
inline void Bind( Vectormath::Aos::Transform3 &out, const char *data )
{
	float input[4*3];
	int c = sscanf( data, "%f %f %f %f %f %f %f %f %f %f %f %f", 
		&input[0], &input[1], &input[2],
		&input[3], &input[4], &input[5],
		&input[6], &input[7], &input[8],
		&input[9], &input[10], &input[11] );

	if ( c == 3 )
	{
		out = Vectormath::Aos::Transform3().identity();
		out.setTranslation( Vectormath::Aos::Vector3( input[0], input[1], input[2] ) );
	}
	else
	if ( c == 9 )
	{
		out = Vectormath::Aos::Transform3().identity();
		out.setCol0( Vectormath::Aos::Vector3( input[0], input[1], input[2] ) );
		out.setCol1( Vectormath::Aos::Vector3( input[3], input[4], input[5] ) );
		out.setCol2( Vectormath::Aos::Vector3( input[6], input[7], input[8] ) );
	}
	else
	if ( c == 12 )
	{
		out = Vectormath::Aos::Transform3().identity();
		out.setCol0( Vectormath::Aos::Vector3( input[0], input[1], input[2] ) );
		out.setCol1( Vectormath::Aos::Vector3( input[3], input[4], input[5] ) );
		out.setCol2( Vectormath::Aos::Vector3( input[6], input[7], input[8] ) );
		out.setCol3( Vectormath::Aos::Vector3( input[9], input[10], input[11] ) );
	} 
}

inline void Bind( float *v, int numElements, const char *data )
{
	char *work = strdup( data );
	char *tok = strtok( work, " ,\t\n\r" );
	for (int i=0; i<numElements; i++)
	{
		v[i] = (float)atof( tok );
		tok = strtok( NULL, " ,\t\n\r" );
	}
	free(work);
}

inline void Bind( Vectormath::Aos::Vector3 &out, const char *data )
{
	float input[3];
	int c = sscanf( data, "%f %f %f", 
		&input[0], &input[1], &input[2] );

	if ( c == 3 )
	{
		out = Vectormath::Aos::Vector3( input[0], input[1], input[2] );
	}
}

inline void Bind( Vectormath::Aos::Vector4 &out, const char *data )
{
	float input[4];
	int c = sscanf( data, "%f %f %f %f", 
		&input[0], &input[1], &input[2], &input[3] );

	if ( c == 4 )
	{
		out = Vectormath::Aos::Vector4( input[0], input[1], input[2], input[3] );
	}
}


template<typename T>
inline void Bind( const char *name, T &out, const char *tag, const char *data, const char *def )
{
	if ( tag )
	{
		if ( _strcmpi( name, tag ) == 0 )
		{
			Bind( out, data );
		}
	} else
	{
		Bind( out, def );
	}
}

template<typename T, class C, typename func>
inline void Bind( const char *name, C *object, func Set, const char *tag, const char *data )
{
	if ( tag )
	{
		if ( strcmpi( name, tag ) == 0 )
		{
			T temp;
			Bind( temp, data );
			(object->*Set)( temp );
		}
	} else
	{
		//Bind( out, def );
	}
}


#endif
