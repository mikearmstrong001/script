#ifndef TESTING_MODEL_JOINTED_H
#define TESTING_MODEL_JOINTED_H

#include "model.h"
#include <vector>
#include "hashname.h"

struct AnimMirrorPair;

class ModelJointed : public Model
{
	Vectormath::Aos::Transform3 *m_bindPose;
	Vectormath::Aos::Transform3 *m_invBindPose;
	std::vector< RenderSurface > m_surfaces;
	HashName *m_boneName;
	int *m_jointParentIndices;
	AnimMirrorPair *m_mirrorParams;
	int m_numJoints;

	EdgeAnimSkeleton *edgeSkel;

public:
	
	~ModelJointed();
	
	void LoadMd5Mesh( const char *filename );

	virtual int NumSurfaces() const { return (int)m_surfaces.size(); }
	virtual const RenderSurface *Surface( int idx ) const { return &m_surfaces[idx]; }

	virtual int NumJoints() const { return m_numJoints; }
	virtual int* JointParentIndices() const { return m_jointParentIndices; }
	virtual Vectormath::Aos::Transform3 *BindTransforms() const { return m_bindPose; }
	virtual Vectormath::Aos::Transform3 *InvBindTransforms() const { return m_invBindPose; }
	virtual AnimMirrorPair  *MirrorParams() const { return m_mirrorParams; }

	virtual void Purge();

	virtual void Reload()
	{
		Purge();
		LoadMd5Mesh( Name() );
	}

	virtual EdgeAnimSkeleton *GetEdgeSkel() const { return edgeSkel; }

};


#endif

