#include "dxut.h"
#include "model_jointed.h"

#include "loader_obj.h"
#include "loader_atgi.h"
#include "loader_meshmd5.h"
#include "material_manager.h"

#include "faff.h"
#include "file_system.h"

#include "anim.h"

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


ModelJointed::~ModelJointed()
{
	Purge();
}

static int FindJoint( std::vector< modeljoint > &joints, const char *name )
{
	for (unsigned int i=0; i<joints.size(); i++)
	{
		if ( joints[i].name == name )
		{
			return i;
		}
	}
	return 0;
}


void ModelJointed::LoadMd5Mesh( const char *filename )
{
	std::vector< ModelGeometry* > geoms;
	std::vector< std::string > materials;
	std::vector< std::string > surfaceName;
	std::vector< modeljoint >  joints;
	::LoadMD5Mesh( geoms, materials, surfaceName, joints, filename );

	char outputName[256];
	sprintf( outputName, "%s.edgeskel", filename );
	PHYSFS_readFileAligned( outputName, (void**)&edgeSkel, 16 );

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
	m_numJoints = (int)joints.size();
	m_bindPose = (Vectormath::Aos::Transform3*)_aligned_malloc( sizeof(Vectormath::Aos::Transform3) * m_numJoints, 16 );
	m_invBindPose = (Vectormath::Aos::Transform3*)_aligned_malloc( sizeof(Vectormath::Aos::Transform3) * m_numJoints, 16 );
	m_boneName = new HashName[m_numJoints];
	m_mirrorParams = new AnimMirrorPair[m_numJoints];
	m_jointParentIndices = new int[m_numJoints];
	for ( unsigned int i=0; i<joints.size(); i++ )
	{
		m_bindPose[i] = joints[i].m;
		m_invBindPose[i] = joints[i].im;
		m_boneName[i] = HashName::FromCStr( joints[i].name.c_str() );
		m_jointParentIndices[i] = joints[i].parent;
		m_mirrorParams[i].idx = FindJoint( joints, joints[i].mirror.c_str() );
		m_mirrorParams[i].mirrorfunc = joints[i].mirrorFunc;
	}
	//writeSurfaces( filename, m_surfaces );
}


void ModelJointed::Purge()
{
	for ( unsigned int i=0; i<m_surfaces.size(); i++)
	{
		delete m_surfaces[i].geom->m_indices;
		delete m_surfaces[i].geom->m_verts;
		delete m_surfaces[i].geom->m_weights;
		delete m_surfaces[i].geom;
	}
	_aligned_free(m_bindPose); m_bindPose = NULL;
	_aligned_free(m_invBindPose); m_invBindPose = NULL;
	delete []m_boneName; m_boneName = NULL;
	delete []m_jointParentIndices; m_jointParentIndices = NULL;
	delete []m_mirrorParams; m_mirrorParams = NULL;
	m_surfaces.clear();
	m_numJoints = 0;
	_aligned_free( edgeSkel );
	edgeSkel = NULL;

	SetLoaded( false );
	SetDefaulted( false );
}
