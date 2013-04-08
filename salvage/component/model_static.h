#ifndef TESTING_MODEL_STATIC_H
#define TESTING_MODEL_STATIC_H

#include "model.h"
#include <vector>

class ModelStatic : public Model
{

	std::vector< RenderSurface > m_surfaces;

public:
	
	~ModelStatic();
	
	void LoadObj( const char *filename );
	void LoadAtgi( const char *filename );
	void LoadMd5Mesh( const char *filename );

	virtual int NumSurfaces() const { return (int)m_surfaces.size(); }
	virtual const RenderSurface *Surface( int idx ) const { return &m_surfaces[idx]; }

	virtual void Purge();

	virtual void Reload()
	{
		Purge();
		LoadObj( Name() );
	}

};


#endif

