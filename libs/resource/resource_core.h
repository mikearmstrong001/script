#pragma once

#include <map>
#include <string>

class Resource
{
	std::string m_name;
	unsigned int m_touchedCounter;

	bool m_loaded;
	bool m_defaulted;

public:

	Resource() : m_touchedCounter(0), m_loaded(false), m_defaulted(false) { }
	virtual ~Resource() {}

	virtual void Reload() = 0;
	virtual void Purge() = 0;

	void SetLoaded( bool v ) { m_loaded = v; }
	void SetDefaulted( bool v ) { m_defaulted = v; }

	bool IsLoaded() const { return m_loaded; }
	bool IsDefaulted() const { return m_defaulted; }

	void SetName( const char *n ) { m_name = n; }
	const char *Name() const { return m_name.c_str(); }

	void Touch( unsigned int counter ) { m_touchedCounter = counter; }
	bool IsTouched( unsigned int counter ) const { return m_touchedCounter == counter; }
};

class ResourceManager
{
	static ResourceManager *s_root;
	ResourceManager *m_next;
public:
	ResourceManager();

	virtual ~ResourceManager()
	{
		//Purge();
	}

	virtual void Purge() = 0;
	virtual void Shutdown() = 0;
	virtual void Reload() = 0;

	static void PurgeAll()
	{
		ResourceManager *cur = s_root;
		while ( cur )
		{
			cur->Purge();
			cur = cur->m_next;
		}
	}

	static void ShutdownAll()
	{
		PurgeAll();
		ResourceManager *cur = s_root;
		while ( cur )
		{
			cur->Shutdown();
			cur = cur->m_next;
		}
	}
};

template <class T>
class TypedResourceManager : public ResourceManager
{
protected:
	std::map< std::string, T* > m_resources;

	virtual T* CreateEmpty( const char *filename ) = 0;
	virtual T* LoadResource( const char *filename ) = 0;

	virtual void Purge()
	{
		std::map< std::string, T* >::iterator c = m_resources.begin();
		while ( c != m_resources.end() )
		{
			c->second->Purge();
			c->second->SetLoaded( false );
			c++;
		}
	}
	virtual void Shutdown()
	{
		std::map< std::string, T* >::iterator c = m_resources.begin();
		while ( c != m_resources.end() )
		{
			delete c->second;
			c++;
		}
		m_resources.clear();
	}
public:


	~TypedResourceManager()
	{
	}

	T *Find( const char *name, bool createDefault )
	{
		std::map<std::string, T*>::iterator f = m_resources.find( name );
		if ( f != m_resources.end() )
		{
			return f->second;
		}
		if ( createDefault )
		{
			T *r = LoadResource( name );
			r->SetName( name );
			r->SetLoaded( true );
			m_resources[name] = r;
			return r;
		} else
		{
			return NULL;
		}
	}

	T *Create( const char *name )
	{
		T *r = Find( name, false );
		if ( r )
		{
			return r;
		}
		r = CreateEmpty();
		r->SetName( name );
		m_resources[name] = r;
		return r;
	}

	T *Load( const char *name, bool force_reload = false )
	{
		T *r = Find( name, false );
		if ( r )
		{
			if ( force_reload )
			{
				r->Reload();
				r->SetLoaded( true );
			}

			return r;
		}
		r = LoadResource( name );
		r->SetName( name );
		r->SetLoaded( true );
		m_resources[name] = r;
		return r;
	}

	virtual void Reload()
	{
		std::map< std::string, T* >::iterator c = m_resources.begin();
		while ( c != m_resources.end() )
		{
			if ( c->second->IsLoaded() )
			{
				c->second->Reload();
				c->second->SetLoaded( true );
			}
			c++;
		}
	}
};

