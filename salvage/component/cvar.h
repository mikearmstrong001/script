#ifndef TESTING_CVAR_H
#define TESTING_CVAR_H

#include <string>
#include <map>

class CVarInternal
{
	float fValue;
	int   iValue;
	std::string sValue;
public:
	inline float GetFloat() const
	{
		return fValue;
	}
	inline int GetInt() const
	{
		return iValue;
	}
	inline const char *GetString() const
	{
		return sValue.c_str();
	}
	void SetValue( const char *v )
	{
		sValue = v;
		fValue = (float)atof( v );
		iValue = atoi( v );
	}
};

class CVarManager
{
	std::map< std::string, CVarInternal * > cvars;

public:

	CVarInternal* Find( const char *name, const char *def );
	void SetValue( const char *name, const char *value );
	const char* AutoComplete( const char *name );

	void Purge();

};

CVarManager* cvarManager();


class CVar
{
	CVarInternal *cvar;

public:

	CVar( const char *name, const char *def )
	{
		cvar = cvarManager()->Find( name, def );
	}

	inline float GetFloat() const
	{
		return cvar->GetFloat();
	}
	inline int GetInt() const
	{
		return cvar->GetInt();
	}
	inline const char *GetString() const
	{
		return cvar->GetString();
	}
	inline void SetValue( const char *v )
	{
		cvar->SetValue(v);
	}
};


#endif
