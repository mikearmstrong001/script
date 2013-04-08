#include "../erbase.h"
#include "cvar.h"
#include "../vector.h"
#include "../str.h"


class CVarInternal {

	Str<char> n;
	CVar::Type type;
	union {
		float		f;
		int			i;
	};
	Str<char> s;

public:

	CVarInternal( const char *name, CVar::Type t ) {
		n = name;
		type = t;
	}

	const char *	GetName( void ) {
		return n.c_str();
	}

	void			Set( const char *value ) {
		if ( type == CVar::TYPE_FLOAT ) {
			f = (float)atof( value );
		} else
		if ( type == CVar::TYPE_INT ) {
			i = atoi( value );
		} else
		if ( type == CVar::TYPE_STRING ) {
			s = value;
		}
	}

	float			GetFloat( void ) const {
		return f;
	}

	int				GetInt( void ) const {
		return i;
	}

	bool			GetBool( void ) const {
		return i != 1;
	}

	const char *	GetString( void ) const {
		return s.c_str();
	}
};

static Vector< CVarInternal* > internalCVars;

CVar::CVar( const char *name, Type t, const char *value ) {

	internalCVar = NULL;
	for (int i=0; i<internalCVars.Size(); i++) {
		if ( stricmp( name, internalCVars[i]->GetName() ) == 0 ) {
			internalCVar = internalCVars[i];
			break;
		}
	}

	if ( internalCVar == NULL ) {
		internalCVar = new CVarInternal( name, t );
		internalCVars.PushBack( internalCVar );
	}
	
	internalCVar->Set( value );
}

void			CVar::Set( const char *value ) {
	internalCVar->Set( value );
}

float			CVar::GetFloat( void ) const{
	return internalCVar->GetFloat();
}

int				CVar::GetInt( void ) const{
	return internalCVar->GetInt();
}

bool			CVar::GetBool( void ) const {
	return internalCVar->GetBool();
}

const char *	CVar::GetString( void ) const {
	return internalCVar->GetString();
}
