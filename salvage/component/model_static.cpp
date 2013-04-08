#include "model_static.h"

#include "loader_obj.h"
#include "loader_atgi.h"
#include "loader_meshmd5.h"
#include "material_manager.h"

#include "faff.h"
#include "file_system.h"

static char* SplitPath( char *out, const char *in )
{
	char drive[_MAX_DRIVE];
	char dir[_MAX_DIR];
	char filename[_MAX_FNAME];
	char ext[_MAX_EXT];
	_splitpath( in, drive, dir, filename, ext );

	sprintf( out, "%s%s", drive, dir );
	return out;
}

static void writeSurfaces( const char *filename, std::vector< RenderSurface > &surfaces )
{
	uint64_t hashedName = HashedString( 0, filename );
	char outfile[256];
	PHYSFS_mkdir( "static_models/" );
	sprintf( outfile, "static_models/%08x_%08x.sm", (uint32_t)(hashedName>>32), (uint32_t)(hashedName&0xffffffff) );
	PHYSFS_File *model = PHYSFS_openWrite( outfile );
	if ( model )
	{
		PHYSFS_writeULE32( model, 0x090 );
		PHYSFS_writeULE32( model, surfaces.size() );
		for ( uint32_t i=0; i<surfaces.size(); i++ )
		{
			RenderSurface const &surf = surfaces[i];
			PHYSFS_writeCStr( model, surf.name.c_str() );
			PHYSFS_writeCStr( model, surf.mat->Name() );
			const ModelGeometry *geom = surf.geom;
			PHYSFS_writeSLE32( model, geom->m_numIndices );
			PHYSFS_writeSLE32( model, geom->m_numVerts );
			PHYSFS_write( model, geom->m_indices, sizeof(uint16_t)*geom->m_numIndices, 1 );
			PHYSFS_write( model, geom->m_verts, sizeof(geom->m_verts[0])*geom->m_numVerts, 1 );
		}
		PHYSFS_close( model );
	}
}

static bool readSurfaces( const char *filename, std::vector< RenderSurface > &surfaces )
{
	uint64_t hashedName = HashedString( 0, filename );
	char outfile[256];
	PHYSFS_mkdir( "static_models/" );
	sprintf( outfile, "static_models/%08x_%08x.sm", (uint32_t)(hashedName>>32), (uint32_t)(hashedName&0xffffffff) );
	PHYSFS_File *model = PHYSFS_openRead( outfile );
	if ( model )
	{
		unsigned int ver, numSurf;
		PHYSFS_readULE32( model, &ver );
		PHYSFS_readULE32( model, &numSurf );
		surfaces.resize( numSurf );
		for ( uint32_t i=0; i<surfaces.size(); i++ )
		{
			RenderSurface &surf = surfaces[i];
			PHYSFS_readCStr( model, surf.name );
			std::string mat;
			PHYSFS_readCStr( model, mat );
			surf.mat = materialManager()->Load( mat.c_str() );

			ModelGeometry *geom = new ModelGeometry;
			surf.geom = geom;
			PHYSFS_readSLE32( model, &geom->m_numIndices );
			PHYSFS_readSLE32( model, &geom->m_numVerts );
			geom->m_indices = new unsigned short[geom->m_numIndices];
			geom->m_verts = new ModelVert[geom->m_numVerts];
			PHYSFS_read( model, geom->m_indices, sizeof(uint16_t)*geom->m_numIndices, 1 );
			PHYSFS_read( model, geom->m_verts, sizeof(geom->m_verts[0])*geom->m_numVerts, 1 );
		}
		PHYSFS_close( model );
		return true;
	}
	return false;
}


ModelStatic::~ModelStatic()
{
	Purge();
}

void ModelStatic::LoadObj( const char *filename )
{
	if ( !readSurfaces( filename, m_surfaces ) )
	{
		std::vector< ModelGeometry* > geoms;
		std::vector< std::string > materials;
		std::vector< std::string > surfaceName;
		::LoadObj( geoms, materials, surfaceName, filename );
		char path[256];
		SplitPath( path, filename );
		for ( unsigned int i=0; i<geoms.size(); i++)
		{
			RenderSurface surf;
			surf.name = surfaceName[i];
			surf.geom = geoms[i];
			surf.mat = materialManager()->Load( (materials[i] + std::string(".material")).c_str() );
			m_surfaces.push_back( surf );
		}
		writeSurfaces( filename, m_surfaces );
	}
}

void ModelStatic::LoadAtgi( const char *filename )
{
	std::vector< ModelGeometry* > geoms;
	std::vector< std::string > materials;
	std::vector< std::string > surfaceName;
	::LoadAtgi( geoms, materials, surfaceName, filename );
	char path[256];
	SplitPath( path, filename );
	for ( unsigned int i=0; i<geoms.size(); i++)
	{
		RenderSurface surf;
		surf.name = surfaceName[i];
		surf.geom = geoms[i];
		surf.mat = materialManager()->Load( (materials[i] + std::string(".material")).c_str() );
		m_surfaces.push_back( surf );
	}
	writeSurfaces( filename, m_surfaces );
}

void ModelStatic::LoadMd5Mesh( const char *filename )
{
	std::vector< ModelGeometry* > geoms;
	std::vector< std::string > materials;
	std::vector< std::string > surfaceName;
	std::vector< modeljoint >  joints;
	::LoadMD5Mesh( geoms, materials, surfaceName, joints, filename );
	char path[256];
	SplitPath( path, filename );
	for ( unsigned int i=0; i<geoms.size(); i++)
	{
		RenderSurface surf;
		surf.name = surfaceName[i];
		surf.geom = geoms[i];
		surf.mat = materialManager()->Load( (materials[i] + std::string(".material")).c_str() );
		m_surfaces.push_back( surf );
	}
	writeSurfaces( filename, m_surfaces );
}


void ModelStatic::Purge()
{
	for ( unsigned int i=0; i<m_surfaces.size(); i++)
	{
		delete m_surfaces[i].geom->m_indices;
		delete m_surfaces[i].geom->m_verts;
		delete m_surfaces[i].geom;
	}
	m_surfaces.clear();

	SetLoaded( false );
	SetDefaulted( false );
}
