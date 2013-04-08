#include "dxut.h"
#include "cvar.h"

CVarInternal* CVarManager::Find( const char *name, const char *def )
{
	std::map<std::string,CVarInternal*>::iterator f = cvars.find( name );
	if ( f == cvars.end() )
	{
		CVarInternal *cv = new CVarInternal();
		cv->SetValue( def );
		cvars[name] = cv;
		return cv;
	} else
	{
		return f->second;
	}
}

void CVarManager::SetValue( const char *name, const char *value )
{
	std::map<std::string,CVarInternal*>::iterator f = cvars.find( name );
	if ( f != cvars.end() )
	{
		f->second->SetValue( value );
	}
}

const char* CVarManager::AutoComplete( const char *name )
{
	unsigned int len = strlen(name);
	std::map<std::string,CVarInternal*>::iterator b = cvars.begin();
	while ( b != cvars.end() )
	{
		if ( len <= b->first.size() && strstr( b->first.c_str(), name ) )//b->first.find_first_of( name ) == 0 )
		{
			return b->first.c_str();
		}
		b++;
	}
	return name;
}

void CVarManager::Purge()
{
	std::map<std::string,CVarInternal*>::iterator b = cvars.begin();
	while ( b != cvars.end() )
	{
		delete b->second;
		b++;
	}
	cvars.clear();
}



CVarManager* cvarManager()
{
	static CVarManager mgr;
	return &mgr;
}
