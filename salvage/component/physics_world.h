#ifndef TESTING_PHYSICS_WORLD_H
#define TESTING_PHYSICS_WORLD_H

class btDefaultCollisionConfiguration;
class btCollisionDispatcher;
class btBroadphaseInterface;
class btSequentialImpulseConstraintSolver;
class btDiscreteDynamicsWorld;
class btRigidBody;
class btActionInterface;
class btCollisionObject;
class btGhostPairCallback;

class PhysicsWorld
{
	btDefaultCollisionConfiguration* collisionConfiguration;
	btCollisionDispatcher* dispatcher;
	btBroadphaseInterface* overlappingPairCache;
	btSequentialImpulseConstraintSolver* solver;
	btDiscreteDynamicsWorld* dynamicsWorld;
	btGhostPairCallback* ghostPairCallback;

public:
	PhysicsWorld();
	~PhysicsWorld();

	void Update();

	void AddAction( btActionInterface *action );
	void AddRigidBody( btRigidBody *body );
	void AddCollisionObject( btCollisionObject *object );
	void RemoveAction( btActionInterface *action );
	void RemoveRigidBody( btRigidBody *body );
	void RemoveCollisionObject( btCollisionObject *object );
};

#endif
