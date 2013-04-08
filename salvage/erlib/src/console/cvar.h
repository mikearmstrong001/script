#ifndef ER_HEADER_CVAR_H
#define ER_HEADER_CVAR_H

class CVar {

	class CVarInternal *internalCVar;

public:

	enum Type {
		TYPE_INT,
		TYPE_STRING,
		TYPE_FLOAT,
	};

	CVar( const char *name, Type t, const char *value );

	void			Set( const char *value );

	float			GetFloat( void ) const;
	int				GetInt( void ) const;
	bool			GetBool( void ) const;
	const char *	GetString( void ) const;
};


#endif

