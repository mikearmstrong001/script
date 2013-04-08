#ifndef TESTING_MODEL_H
#define TESTING_MODEL_H

#include "resource_core.h"
#include "model_structs.h"

struct AnimMirrorPair;
struct EdgeAnimSkeleton;

class Model : public Resource
{
public:

	virtual ~Model() {}

	virtual int NumSurfaces() const = 0;
	virtual const RenderSurface *Surface( int idx ) const = 0;

	virtual int NumJoints() const { return 0; }
	virtual int* JointParentIndices() const { return 0; }
	virtual Vectormath::Aos::Transform3 *BindTransforms() const { return NULL; }
	virtual Vectormath::Aos::Transform3 *InvBindTransforms() const { return NULL; }
	virtual AnimMirrorPair  *MirrorParams() const { return NULL; }
	virtual EdgeAnimSkeleton *GetEdgeSkel() const { return NULL; }
};

#endif
