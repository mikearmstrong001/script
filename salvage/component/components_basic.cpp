#define _USE_MATH_DEFINES

#include "faff.h"
#include "bind.h"
#include "components_basic.h"
#include "components_render.h"

#include <math.h>

#include <float.h>
#include "model_manager.h"
#include "material.h"
#include "technique.h"
#include "flare.h"
#include "animset.h"
#include "anim.h"
#include "collisionmodel_manager.h"

#include "btBulletDynamicsCommon.h"
#include "BulletCollision/CollisionShapes/btConvexPointCloudShape.h"
#include "BulletCollision/CollisionDispatch/btGhostObject.h"
#include "BulletDynamics/Character/btKinematicCharacterController.h"
#include "physics_world.h"
#include "render_world.h"
#include "world_db.h"
#include "hw.h"

#include "input.h"

CTransform::CTransform( const char *name, class GameObject *go ) : PARENT(name, go), m_parent(NULL)
{
}

void CTransform::Bind( const char *tag, const char *data )
{
	PARENT::Bind( tag, data );
	::Bind( "local", m_local, tag, data, "0 0 0" );
}

void CTransform::Enter()
{
	Update();
}

void CTransform::Update()
{
	if ( m_parent == NULL )
	{
		UpdateKids();
	}
}

void CTransform::UpdateKids()
{
	if ( m_parent )
	{
		m_localToWorld = m_parent->Transform()->world() * local();
	} else
	{
		m_localToWorld = local();
	}
	for (unsigned int i=0; i<m_kids.size(); i++)
	{
		m_kids[i]->Transform()->UpdateKids();
	}
}

TYPENAME(CTransform,Transform)



class JointTransform : public CTransform
{
	DECLAREINFO( JointTransform, CTransform );
	std::string m_joint;

public:
	JointTransform( const char *name, class GameObject *go ) : PARENT(name, go)
	{
	}

	virtual void Bind( const char *tag, const char *data )
	{
		PARENT::Bind( tag, data );
		::Bind( "joint", m_joint, tag, data, "*unset*" );
	}
};

TYPE(JointTransform);

void Renderable::Enter()
{
	Owner()->World()->GetRenderWorld()->RegisterRenderable( this );
}

void Renderable::Remove()
{
	Owner()->World()->GetRenderWorld()->UnregisterRenderable( this );
}


void Camera::Bind( const char *tag, const char *data )
{
	PARENT::Bind( tag, data );
	::Bind( "fov", m_fov, tag, data, "80" );
	::Bind( "znear", m_znear, tag, data, "1" );
	::Bind( "zfar", m_zfar, tag, data, "1000" );
	::Bind( "priority", m_priority, tag, data, "64" );
	::Bind( "clearColour", m_clearColour, tag, data, "1" );
	::Bind( "clearDepth", m_clearDepth, tag, data, "1" );
}

void Camera::Enter()
{
	Owner()->World()->GetRenderWorld()->RegisterCamera( this );
}

void Camera::Remove()
{
	Owner()->World()->GetRenderWorld()->UnregisterCamera( this );
}


TYPE(Camera);






class SoundSource : public Component
{
	DECLAREINFO( SoundSource, Component );
public:
	SoundSource( const char *name, class GameObject *go ) : PARENT(name, go)
	{
	}

	virtual void Bind( const char *tag, const char *data )
	{
		PARENT::Bind( tag, data );
	}
};

TYPE(SoundSource);

class SoundListener : public Component
{
	DECLAREINFO( SoundListener, Component );
public:
	SoundListener( const char *name, class GameObject *go ) : PARENT(name, go)
	{
	}

	virtual void Bind( const char *tag, const char *data )
	{
		PARENT::Bind( tag, data );
	}
};

TYPE(SoundListener);


/*************TEST******************/

class Spinner : public Component
{
	DECLAREINFO( Spinner, Component );
	float m_speed;
	float m_roty;
public:
	Spinner( const char *name, class GameObject *go ) : m_speed(0.f), m_roty(0.f), PARENT(name, go)
	{
		go->RegisterForUpdate( this, ePREUPDATE );
	}

	virtual void Bind( const char *tag, const char *data )
	{
		PARENT::Bind( tag, data );
		::Bind( "speed", m_speed, tag, data, "0.05" );
		::Bind( "start", m_roty, tag, data, "0" );
	}

	virtual void Update( int /*idx*/)
	{
		CTransform *t = FindComponent<CTransform>();
		Vectormath::Aos::Transform3 cur = t->local();
		cur.setUpper3x3( Vectormath::Aos::Matrix3::rotationY( m_roty ) );
		t->setLocal( cur );
		m_roty += m_speed;
	}
};

TYPE(Spinner);

class KeyboardTransformModifier : public Component
{
	DECLAREINFO( KeyboardTransformModifier, Component );
	float m_speed;
	float m_rotspeed;
	float m_roty;
public:
	KeyboardTransformModifier( const char *name, class GameObject *go ) : m_speed(0.f), m_rotspeed(0.f), m_roty(0.f), PARENT(name, go)
	{
		go->RegisterForUpdate( this, ePREUPDATE );
	}

	virtual void Bind( const char *tag, const char *data )
	{
		PARENT::Bind( tag, data );
		::Bind( "speed", m_speed, tag, data, "0.8" );
		::Bind( "rotspeed", m_rotspeed, tag, data, "0.1" );
		::Bind( "roty", m_roty, tag, data, "0" );
	}

	virtual void Update( int /*idx*/ )
	{
		CTransform *t = FindComponent<CTransform>();
		Vectormath::Aos::Transform3 trans = t->local();

		if ( in::GetKeyState( in::KEY_A ) )
		{
			m_roty -= m_rotspeed;
		}
		if ( in::GetKeyState( in::KEY_D ) )
		{
			m_roty += m_rotspeed;
		}
		if ( in::GetKeyState( in::KEY_UP ) )
		{
			trans[3][0] += sinf( m_roty ) * m_speed;
			trans[3][2] += cosf( m_roty ) * m_speed;
		}
		if ( in::GetKeyState( in::KEY_DOWN ) )
		{
			trans[3][0] += sinf( m_roty ) * -m_speed;
			trans[3][2] += cosf( m_roty ) * -m_speed;
		}
		if ( in::GetKeyState( in::KEY_LEFT ) )
		{
			trans[3][0] += cosf( m_roty ) * -m_speed;
			trans[3][2] -= sinf( m_roty ) * -m_speed;
		}
		if ( in::GetKeyState( in::KEY_RIGHT ) )
		{
			trans[3][0] += cosf( m_roty ) * m_speed;
			trans[3][2] -= sinf( m_roty ) * m_speed;
		}
		if ( in::GetKeyState( in::KEY_Q ) )
		{
			trans[3][1] += m_speed;
		}
		if ( in::GetKeyState( in::KEY_E ) )
		{
			trans[3][1] -= m_speed;
		}
		Vectormath::Aos::Matrix3 roty = Vectormath::Aos::Matrix3::rotationY( m_roty );
		trans.setUpper3x3( roty );


		t->setLocal( trans );
	}
};

TYPE(KeyboardTransformModifier);


class CollisionModel;
class MeshCollisionModel : public Component
{
	DECLAREINFO( MeshCollisionModel, Component );

	CollisionModel *model;

	void SetModel( const char *data )
	{
		model = collisionModelManager()->Load( data );
	}

public:
	MeshCollisionModel( const char *name, class GameObject *go ) : model(NULL), PARENT(name, go)
	{
	}

	virtual void Bind( const char *tag, const char *data )
	{
		PARENT::Bind( tag, data );
		::Bind< const char* >( "model", this, &MeshCollisionModel::SetModel, tag, data );
	}

	CollisionModel *GetModel() { return model; }
};

TYPE(MeshCollisionModel);


class RigidBody : public Component
{
	DECLAREINFO( RigidBody, Component );

	btDefaultMotionState* motionState;
	btRigidBody* body;

public:
	RigidBody( const char *name, class GameObject *go ) : motionState(NULL), body(NULL), PARENT(name, go)
	{
		go->RegisterForUpdate( this, ePREUPDATE );
	}

	~RigidBody()
	{
		PhysicsWorld *physicsWorld = Owner()->World()->GetPhysicsWorld();
		physicsWorld->RemoveRigidBody( body );

		delete body;
		delete motionState;
	}

	virtual void Bind( const char *tag, const char *data )
	{
		PARENT::Bind( tag, data );
	}

	virtual void Enter()
	{
		btScalar mass(1.f);

		MeshCollisionModel *collision = FindComponent<MeshCollisionModel>();
		CollisionModel *model = collision->GetModel();
		model->GenerateHull();

		btVector3 localInertia(0,0,0);
		model->convexHullShape->calculateLocalInertia(mass,localInertia);

		CTransform *t = FindComponent<CTransform>();
		const Vectormath::Aos::Transform3 &xfrom = t->world();
		const Vectormath::Aos::Quat rot( xfrom.getUpper3x3() );
		const Vectormath::Aos::Vector3 trans = xfrom.getTranslation();

		btTransform transform( btQuaternion( rot.getX(), rot.getY(), rot.getZ(), rot.getW() ), 
			btVector3( trans.getX(), trans.getY(), trans.getZ() ) );

		//using motionstate is recommended, it provides interpolation capabilities, and only synchronizes 'active' objects
		motionState = new btDefaultMotionState(transform);
		btRigidBody::btRigidBodyConstructionInfo rbInfo(mass,motionState,model->convexHullShape,localInertia);
		body = new btRigidBody(rbInfo);

		PhysicsWorld *physicsWorld = Owner()->World()->GetPhysicsWorld();
		physicsWorld->AddRigidBody( body );
	}

	virtual void Update( int /*idx*/ )
	{
		btTransform xform;
		motionState->getWorldTransform( xform );
		btQuaternion q = xform.getRotation();
		btVector3 t = xform.getOrigin();

		Vectormath::Aos::Transform3 transform( Vectormath::Aos::Quat( q.getX(), q.getY(), q.getZ(), q.getW() ),
			Vectormath::Aos::Vector3( t.getX(), t.getY(), t.getZ() ) );
		
		CTransform *tc = FindComponent<CTransform>();
		tc->setLocal( transform );

	}
};

TYPE(RigidBody);


class StaticBody : public Component
{
	DECLAREINFO( StaticBody, Component );

	btDefaultMotionState* motionState;
	btRigidBody* body;

public:
	StaticBody( const char *name, class GameObject *go ) : motionState(NULL), body(NULL), PARENT(name, go)
	{
	}

	~StaticBody()
	{
		PhysicsWorld *physicsWorld = Owner()->World()->GetPhysicsWorld();
		physicsWorld->RemoveRigidBody( body );
		delete body;
		delete motionState;
	}

	virtual void Bind( const char *tag, const char *data )
	{
		PARENT::Bind( tag, data );
	}

	virtual void Enter()
	{
		btScalar mass(0.f);

		MeshCollisionModel *collision = FindComponent<MeshCollisionModel>();
		CollisionModel *model = collision->GetModel();

		btVector3 localInertia(0,0,0);

		CTransform *t = FindComponent<CTransform>();
		const Vectormath::Aos::Transform3 &xfrom = t->world();
		const Vectormath::Aos::Quat rot( xfrom.getUpper3x3() );
		const Vectormath::Aos::Vector3 trans = xfrom.getTranslation();

		btTransform transform( btQuaternion( rot.getX(), rot.getY(), rot.getZ(), rot.getW() ), 
			btVector3( trans.getX(), trans.getY(), trans.getZ() ) );

		//using motionstate is recommended, it provides interpolation capabilities, and only synchronizes 'active' objects
		motionState = new btDefaultMotionState(transform);
		btRigidBody::btRigidBodyConstructionInfo rbInfo(mass,motionState,model->shape,localInertia);
		body = new btRigidBody(rbInfo);

		PhysicsWorld *physicsWorld = Owner()->World()->GetPhysicsWorld();
		physicsWorld->AddRigidBody( body );
	}
};

TYPE(StaticBody);


class CharacterController : public Component
{
	DECLAREINFO( CharacterController, Component );

	btConvexShape* capsule;
	btPairCachingGhostObject *ghostObject;
	btKinematicCharacterController *character;

public:
	CharacterController( const char *name, class GameObject *go ) : ghostObject(NULL), character(NULL), PARENT(name, go)
	{
		go->RegisterForUpdate( this, ePREUPDATE );
	}

	~CharacterController()
	{
		PhysicsWorld *physicsWorld = Owner()->World()->GetPhysicsWorld();
		physicsWorld->RemoveAction( character );
		physicsWorld->RemoveCollisionObject(ghostObject);

		delete ghostObject;
		delete character;
		delete capsule;
	}

	virtual void Bind( const char *tag, const char *data )
	{
		PARENT::Bind( tag, data );
	}

	virtual void Enter()
	{
		CTransform *t = FindComponent<CTransform>();
		const Vectormath::Aos::Transform3 &xfrom = t->world();
		const Vectormath::Aos::Quat rot( xfrom.getUpper3x3() );
		const Vectormath::Aos::Vector3 trans = xfrom.getTranslation();

		btTransform transform( btQuaternion( rot.getX(), rot.getY(), rot.getZ(), rot.getW() ), 
			btVector3( trans.getX(), trans.getY(), trans.getZ() ) );

		ghostObject = new btPairCachingGhostObject();
		ghostObject->setWorldTransform(transform);
		btScalar characterHeight=1.75;
		btScalar characterWidth =1.75;
		
		capsule = new btCapsuleShape(characterWidth,characterHeight);
		ghostObject->setCollisionShape (capsule);
		ghostObject->setCollisionFlags (btCollisionObject::CF_CHARACTER_OBJECT);

		btScalar stepHeight = btScalar(0.35);
		character = new btKinematicCharacterController (ghostObject,capsule,stepHeight);


		PhysicsWorld *physicsWorld = Owner()->World()->GetPhysicsWorld();
		physicsWorld->AddCollisionObject(ghostObject);
		physicsWorld->AddAction( character );
	}

	virtual void Update( int /*idx*/ )
	{
		btTransform xform = ghostObject->getWorldTransform();
		btVector3 t = xform.getOrigin();

		CTransform *tc = FindComponent<CTransform>();
		tc->setLocalTranslation( Vectormath::Aos::Vector3( t.getX(), t.getY(), t.getZ() ) );

	}

	void SetWalkDir( Vectormath::Aos::Vector3 dir )
	{
		character->setWalkDirection( btVector3( dir.getX(), dir.getY(), dir.getZ() ) );
	}
};

TYPE(CharacterController);

class KeyboardCharacterControllerModifier : public Component
{
	DECLAREINFO( KeyboardCharacterControllerModifier, Component );
	float m_speed;
	float m_rotspeed;
	float m_roty;
public:
	KeyboardCharacterControllerModifier( const char *name, class GameObject *go ) : m_speed(0.f), m_rotspeed(0.f), m_roty(0.f), PARENT(name, go)
	{
		go->RegisterForUpdate( this, ePREUPDATE );
		//go->RegisterForUpdate( this, ePOSTUPDATE );
	}

	virtual void Bind( const char *tag, const char *data )
	{
		PARENT::Bind( tag, data );
		::Bind( "speed", m_speed, tag, data, "0.8" );
		::Bind( "rotspeed", m_rotspeed, tag, data, "0.1" );
		::Bind( "roty", m_roty, tag, data, "0" );
	}

	virtual void Update( int idx )
	{
		if ( idx == ePREUPDATE )
		{
			CTransform *t = FindComponent<CTransform>();
			Vectormath::Aos::Vector3 walkDir(0.f,0.f,0.f);
			Vectormath::Aos::Transform3 trans = t->local();

			if ( in::GetKeyState( in::KEY_A ) )
			{
				m_roty -= m_rotspeed;
			}
			if ( in::GetKeyState( in::KEY_D ) )
			{
				m_roty += m_rotspeed;
			}
			if ( in::GetKeyState( in::KEY_UP ) )
			{
				walkDir += Vectormath::Aos::Vector3( sinf( m_roty ), 0.f, cosf( m_roty ) ) * m_speed;
			}
			if ( in::GetKeyState( in::KEY_DOWN ) )
			{
				walkDir -= Vectormath::Aos::Vector3( sinf( m_roty ), 0.f, cosf( m_roty ) ) * m_speed;
			}
			if ( in::GetKeyState( in::KEY_LEFT ) )
			{
				walkDir -= Vectormath::Aos::Vector3( cosf( m_roty ), 0.f, -sinf( m_roty ) ) * m_speed;
			}
			if ( in::GetKeyState( in::KEY_RIGHT ) )
			{
				walkDir += Vectormath::Aos::Vector3( cosf( m_roty ), 0.f, -sinf( m_roty ) ) * m_speed;
			}

			CharacterController *cc = FindComponent<CharacterController>();
			if ( cc )
			{
				cc->SetWalkDir( walkDir );
			}
			Vectormath::Aos::Matrix3 roty = Vectormath::Aos::Matrix3::rotationY( m_roty );
			t->setLocalRotation( roty );
		} else
		if ( idx == ePOSTUPDATE )
		{
			CTransform *t = FindComponent<CTransform>();
			Vectormath::Aos::Matrix3 roty = Vectormath::Aos::Matrix3::rotationY( m_roty );
			t->setWorldRotation( roty );
		}
	}
};

TYPE(KeyboardCharacterControllerModifier);

#include "edgeanim.h"

struct EDGE_ALIGNED(16) CharacterInstance 
{
	EdgeAnimInstance	header;

	EdgeAnimMemoryRef 	refWorldMatrices;
	EdgeAnimMemoryRef 	refAnimation[2];

	float				evalTime[2];
	float				blendAlpha;
	int					loops[2];

	EdgeAnimLocomotionState	loco;

	unsigned int		mask;
};

// offsets used to access cmd list parameters stored in the instance data
enum {
	kInstanceMask = offsetof( CharacterInstance, mask )+0,
	kEvalTime0	= offsetof( CharacterInstance, evalTime )+0,
	kEvalTime1	= offsetof( CharacterInstance, evalTime )+4,
	kBlendAlpha	= offsetof( CharacterInstance, blendAlpha ),
	kLoop0		= offsetof( CharacterInstance, loops )+0,
	kLoop1		= offsetof( CharacterInstance, loops )+4
};

// offsets used to access memory addresses stored in the instance data
enum {
	kWorldMatrices	= offsetof( CharacterInstance, refWorldMatrices ),
	kAnimation0		= offsetof( CharacterInstance, refAnimation )+0,
	kAnimation1		= offsetof( CharacterInstance, refAnimation )+4,
	kLoco		= offsetof( CharacterInstance, loco )
};

// In an ideal world this would be built offline & loaded in
static EdgeAnimCmd		g_commandList[] = 
{
	EdgeAnimCmdPushAndEvaluateWithLocomotion( kInstanceMask, 0x1, EDGE_ANIM_CMD_EVALUATE_JOINTS, kAnimation0, kEvalTime0, kLoop0, kLoco ),
	EdgeAnimCmdPushAndEvaluateWithLocomotion( kInstanceMask, 0x2, EDGE_ANIM_CMD_EVALUATE_JOINTS, kAnimation1, kEvalTime1, kLoop1, kLoco ),
	EdgeAnimCmdLinear2Blend( kInstanceMask, 0x2, kBlendAlpha ),
	EdgeAnimCmdSaveJoints( kInstanceMask, 0x1, EDGE_ANIM_CMD_SAVE_JOINTS_MAT44, EDGE_ANIM_CMD_SAVE_JOINTS_WORLD, kWorldMatrices ),
	EdgeAnimCmdStop( 0, 0, EDGE_ANIM_CMD_STOP_EMPTY_STACK )
};

// The command LUT
EdgeAnimCmdFunc 				g_commandFuncs[ NUM_EDGE_ANIM_CMD_OPS ] = 
{
	_edgeAnimCmdStop,
	_edgeAnimCmdPushPose,
	_edgeAnimCmdPopPose,
	_edgeAnimCmdLoadJoints,
	_edgeAnimCmdSaveJoints,
	_edgeAnimCmdLoadChannels,
	_edgeAnimCmdSaveChannels,
	_edgeAnimCmdLoadWeights,
	_edgeAnimCmdSaveWeights,
	_edgeAnimCmdEvaluate,
	_edgeAnimCmdUpdateLocoRoot,
	_edgeAnimCmdLinear2Blend,
	_edgeAnimCmdLinearNBlend,
	_edgeAnimCmdDelta2Blend,
	_edgeAnimCmdCompositeNBlend,
	_edgeAnimCmdMirrorJoints,
	_edgeAnimCmdRetargetPose,
	_edgeAnimCmdTwoJointIK
};

class MeshAnimator : public Component
{
	DECLAREINFO( MeshAnimator, Component );

	struct stateInfo
	{
		std::string		state;
		float			time;
		float			speed;

		stateInfo() : time(0.f), speed(1.f)
		{
		}
	};

	const AnimSet *	animset;
	stateInfo	curState;
	stateInfo	nextState;
	float		curBlendInTime;
	float		blendInTime;
	int mirror;
	EdgeAnimContext edgeContext;
	void *edgeContextWork;
	CharacterInstance edgeInstance;

	void SetAnimSet( const char *data )
	{
		animset = animSetManager()->Load( data );
	}
public:
	MeshAnimator( const char *name, class GameObject *go ) : PARENT(name, go), curBlendInTime(0.f), blendInTime(0.f), edgeContextWork(NULL)
	{
		go->RegisterForUpdate( this, ePREUPDATE );
	}

	~MeshAnimator()
	{
		_aligned_free( edgeContextWork );
		edgeContextWork = NULL;
	}

	virtual void Bind( const char *tag, const char *data )
	{
		PARENT::Bind( tag, data );
		::Bind< const char* >( "animset", this, &THISCLASS::SetAnimSet, tag, data );
		::Bind( "state", curState.state, tag, data, "idle" );
		::Bind( "speed", curState.speed, tag, data, "1" );
		::Bind( "mirror", mirror, tag, data, "0" );
	}

	virtual void Update( int /*idx*/ )
	{
		const Anim *curAnim = animset->Find( curState.state.c_str() );
		if ( curAnim == NULL )
		{
			return;
		}

		JointedMeshModel *jointedModel = FindComponent<JointedMeshModel>();
		Model *mdl = jointedModel->GetModel();
		
		{
			EdgeAnimSkeleton *edgeSkel = mdl->GetEdgeSkel();
			if ( edgeContextWork == NULL )
			{
				int size = edgeAnimComputeContextSize( edgeSkel->numJoints, edgeSkel->numUserChannels, 2, 0 );
				edgeContextWork = _aligned_malloc( size, 16 );
				edgeAnimInitializeContext( &edgeContext, edgeContextWork, size, edgeSkel );
	
				edgeAnimInitializeInstance(&edgeInstance.header, sizeof(CharacterInstance));
				edgeInstance.loco.m_flags = EDGE_ANIM_LOCOMOTION_RESET|EDGE_ANIM_LOCOMOTION_DELTA;
			}
		}

		int numJoints = curAnim->NumTransforms();

		edgeInstance.refAnimation[0]    = EdgeAnimMemoryRef( curAnim->GetEdgeAnim() );
		float lastEvalTime0 = edgeInstance.evalTime[0];
		edgeInstance.evalTime[0] = fmodf( curState.time, curAnim->GetEdgeAnim()->duration );
		edgeInstance.loops[0] = ( edgeInstance.evalTime[0] < lastEvalTime0 ) ? 1 : 0;
		*(Vectormath::Aos::Quat*)(edgeInstance.header.rootJoint+0) = Vectormath::Aos::Quat::identity();
		*(Vectormath::Aos::Point3*)(edgeInstance.header.rootJoint+4) = Vectormath::Aos::Point3::Point3(0.f);
		*(Vectormath::Aos::Vector3*)(edgeInstance.header.rootJoint+8) = Vectormath::Aos::Vector3::Vector3(1.f);
		edgeInstance.mask = 0x1;

		if ( curBlendInTime > 0.f )
		{
			const Anim *nextAnim = animset->Find( nextState.state.c_str() );
			if ( nextAnim == NULL )
			{
				return;
			}

			int numNextJoints = nextAnim->NumTransforms();
			assert( numJoints == numNextJoints );

			float blendIn = 1.f - (curBlendInTime/blendInTime);
			curBlendInTime -= 1.f/30.f;
			nextState.time += 1.f/30.f * nextState.speed;

			edgeInstance.refAnimation[1] = EdgeAnimMemoryRef( nextAnim->GetEdgeAnim() );
			edgeInstance.evalTime[1] = nextState.time;
			edgeInstance.blendAlpha = blendIn;

			if ( curBlendInTime <= 0.f )
			{
				curState = nextState;
			}
		}

		Vectormath::Aos::Matrix4 *transformStack = (Vectormath::Aos::Matrix4*)FrameAlloc( numJoints * sizeof(Vectormath::Aos::Matrix4) );

		edgeInstance.refWorldMatrices = EdgeAnimMemoryRef( transformStack );
		edgeAnimProcessCmdList(&edgeContext, &edgeInstance.header, g_commandList, g_commandFuncs, NUM_EDGE_ANIM_CMD_OPS);

		Vectormath::Aos::Transform3 *dest = jointedModel->GetJoints();
		assert( mdl->NumJoints() == numJoints );
		Vectormath::Aos::Transform3 *invBind = mdl->InvBindTransforms();

		for (int i=0; i<numJoints; i++)
		{
			Vectormath::Aos::Transform3 trans( transformStack[i].getUpper3x3(), transformStack[i].getTranslation() );//curSample[i].q, Vectormath::Aos::Vector3(curSample[i].t) );
			dest[i] = trans * invBind[i];
		}
		curState.time += 1.f/30.f * curState.speed;
	}

	void SetState( const char *state, float blendTime )
	{
	}
};

TYPE(MeshAnimator);
