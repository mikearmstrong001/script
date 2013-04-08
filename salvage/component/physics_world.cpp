#include "dxut.h"
#include "physics_world.h"
#include "btBulletDynamicsCommon.h"
#include "BulletCollision/CollisionDispatch/btGhostObject.h"


PhysicsWorld::PhysicsWorld()
{
	///collision configuration contains default setup for memory, collision setup. Advanced users can create their own configuration.
	collisionConfiguration = new btDefaultCollisionConfiguration();

	///use the default collision dispatcher. For parallel processing you can use a diffent dispatcher (see Extras/BulletMultiThreaded)
	dispatcher = new	btCollisionDispatcher(collisionConfiguration);

	///btDbvtBroadphase is a good general purpose broadphase. You can also try out btAxis3Sweep.
	overlappingPairCache = new btDbvtBroadphase();
	ghostPairCallback = new btGhostPairCallback();

		
	overlappingPairCache->getOverlappingPairCache()->setInternalGhostPairCallback(ghostPairCallback);

	///the default constraint solver. For parallel processing you can use a different solver (see Extras/BulletMultiThreaded)
	solver = new btSequentialImpulseConstraintSolver;

	dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher,overlappingPairCache,solver,collisionConfiguration);

	dynamicsWorld->setGravity(btVector3(0,-10,0));
}

PhysicsWorld::~PhysicsWorld()
{
	delete dynamicsWorld;
	delete solver;
	delete overlappingPairCache;
	delete ghostPairCallback;
	delete dispatcher;
	delete collisionConfiguration;
}

void PhysicsWorld::Update()
{
	dynamicsWorld->stepSimulation(1.f/30.f,1);
}

void PhysicsWorld::AddAction( btActionInterface *action )
{
	dynamicsWorld->addAction( action );
}


void PhysicsWorld::AddRigidBody( btRigidBody *body )
{
	dynamicsWorld->addRigidBody(body);
}

void PhysicsWorld::AddCollisionObject( btCollisionObject *object )
{
	dynamicsWorld->addCollisionObject(object,btBroadphaseProxy::CharacterFilter, btBroadphaseProxy::StaticFilter|btBroadphaseProxy::DefaultFilter);
}

void PhysicsWorld::RemoveAction( btActionInterface *action )
{
	dynamicsWorld->removeAction( action );
}

void PhysicsWorld::RemoveRigidBody( btRigidBody *body )
{
	dynamicsWorld->removeRigidBody( body );
}

void PhysicsWorld::RemoveCollisionObject( btCollisionObject *object )
{
	dynamicsWorld->removeCollisionObject( object );
}
