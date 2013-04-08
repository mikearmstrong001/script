#ifndef TESTING_ANIM_H
#define TESTING_ANIM_H

#include "faff.h"
#include "resource_core.h"
#include "anim_structs.h"

enum	MirrorSpecBits
{
	kQuatX		=	0,
	kQuatY		=	1,
	kQuatZ		=	2,
	kQuatW		=	3,

	kTransX		=	0,
	kTransY		=	1,
	kTransZ		=	2,
	kTransW		=	3,

	kNegate		=	4,
	
	kNegQuatX	=	kNegate + kQuatX,
	kNegQuatY	=	kNegate + kQuatY,
	kNegQuatZ	=	kNegate + kQuatZ,
	kNegQuatW	=	kNegate + kQuatW,

	kNegTransX	=	kNegate + kTransX,
	kNegTransY	=	kNegate + kTransY,
	kNegTransZ	=	kNegate + kTransZ,
	kNegTransW	=	kNegate + kTransW,
};

#define	ANIM_MIRROR_QUAT( qx, qy, qz, qw )		( ( ( qx ) << 28 ) | ( ( qy ) << 24 ) | ( ( qz ) << 20 ) | ( ( qw ) << 16 ) )
#define	ANIM_MIRROR_TRANS( tx, ty, tz )			( ( ( tx ) << 12 ) | ( ( ty ) <<  8 ) | ( ( tz ) <<  4 ) | ( kTransW << 0 ) )
#define	ANIM_MIRROR( qx, qy, qz, qw, tx, ty, tz )	( ANIM_MIRROR_QUAT( ( qx ), ( qy ), ( qz ), ( qw ) ) | ANIM_MIRROR_TRANS( ( tx ), ( ty ), ( tz ) ) )

enum	MirrorSpecs
{
	kNotMirrored					= ANIM_MIRROR( kQuatX, kQuatY, kQuatZ, kQuatW,			kTransX, kTransY, kTransZ ),		// No mirroring at all

	kBehaviouralNonPaired			= ANIM_MIRROR( kNegQuatX, kQuatY, kQuatZ, kNegQuatW,		kNegTransX, kTransY, kTransZ ),		// Mirror rotation in yz, mirror translation in x
	kBehaviouralLinkedToNonPaired	= ANIM_MIRROR( kNegQuatW, kQuatZ, kNegQuatY, kQuatX,		kNegTransX, kTransY, kTransZ ),		// Mirror rotation in yz, rotate around x by pi, mirror translation in x
	kBehaviouralPaired				= ANIM_MIRROR( kQuatX, kQuatY, kQuatZ, kQuatW,			kNegTransX, kNegTransY, kTransZ ),	// Mirror translation in x and y

	kOrientation					= ANIM_MIRROR( kQuatX, kNegQuatY, kNegQuatZ, kQuatW,		kNegTransX, kTransY, kTransZ ),		// Mirror rotation in yz, mirror translation in x
};	


struct AnimMirrorPair
{
	unsigned int idx;
	unsigned int mirrorfunc;
};

struct EdgeAnimAnimation;

class Anim : public Resource
{
	anim a;
	EdgeAnimAnimation *edgeAnim;

	void LoadMD5( const char *filename );

public:

	Anim();
	~Anim();

	virtual void Purge();

	void Load( const char *filename );

	virtual void Reload()
	{
		Purge();
		Load( Name() );
	}

	int  NumTransforms() const;
	void SampleObjectSpace( Vectormath::Aos::Transform3 *out, int num, float time ) const;
	void SampleJointSpace( animsample *out, int num, float time, AnimMirrorPair *mirror ) const;

	int JointParent( int jointIndex );

	EdgeAnimAnimation *GetEdgeAnim() const { return edgeAnim; }
};

class AnimManager : public TypedResourceManager<Anim>
{
	virtual Anim* CreateEmpty( const char *filename );
	virtual Anim* LoadResource( const char *filename );

public:
	AnimManager();
};

AnimManager *animManager();

#endif
