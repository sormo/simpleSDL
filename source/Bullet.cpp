#include "Bullet.h"

static glm::vec3 Convert(const btVector3 & v)
{
    return { v.x(), v.y(), v.z() };
}

static btVector3 Convert(const glm::vec3 & v)
{
    return { v.x, v.y, v.z };
}


Bullet::Bullet(const glm::vec3 & gravity)
{
    // Collision configuration contains default setup for memory, collision setup. Advanced users can create their own configuration.
    m_collisionConfiguration.reset(new btDefaultCollisionConfiguration());

    // Use the default collision dispatcher. For parallel processing you can use a diffent dispatcher (see Extras/BulletMultiThreaded)
    m_dispatcher.reset(new btCollisionDispatcher(m_collisionConfiguration.get()));

    // btDbvtBroadphase is a good general purpose broadphase. You can also try out btAxis3Sweep.
    m_broadphase.reset(new btDbvtBroadphase());

    // The default constraint solver. For parallel processing you can use a different solver (see Extras/BulletMultiThreaded)
    m_solver.reset(new btSequentialImpulseConstraintSolver());

    m_world.reset(new btDiscreteDynamicsWorld(m_dispatcher.get(), m_broadphase.get(), m_solver.get(), m_collisionConfiguration.get()));

    m_world->setGravity(Convert(gravity));
    m_world->setDebugDrawer(&m_debug);
}

Bullet::~Bullet()
{
    // Remove the rigid bodies from the dynamics world and delete them
    for (int32_t i = m_world->getNumCollisionObjects() - 1; i >= 0; i--)
    {
        btCollisionObject * obj = m_world->getCollisionObjectArray()[i];
        btRigidBody * body = btRigidBody::upcast(obj);
        if (body && body->getMotionState())
        {
            delete body->getMotionState();
        }
        m_world->removeCollisionObject(obj);

        delete obj;
    }

    // Delete collision shapes
    for (int32_t i = 0; i < m_collisionShapes.size(); i++)
    {
        btCollisionShape * shape = m_collisionShapes[i];
        m_collisionShapes[i] = nullptr;

        delete shape;
    }
}

btRigidBody * Bullet::AddBox(const glm::vec3 & position, const glm::vec3 & rotation, const glm::vec3 & halfExtents, bool isStatic)
{
    btCollisionShape* groundShape = new btBoxShape(Convert(halfExtents));
    m_collisionShapes.push_back(groundShape);

    btTransform groundTransform;
    groundTransform.setIdentity();
    groundTransform.setOrigin(Convert(position));

    btQuaternion rotationQuaternion;
    rotationQuaternion.setEulerZYX(rotation.x, rotation.y, rotation.z);
    groundTransform.setRotation(rotationQuaternion);

    btScalar mass = isStatic ? 0.0f : 1.0f;

    //rigidbody is dynamic if and only if mass is non zero, otherwise static
    bool isDynamic = (mass != 0.f);

    btVector3 localInertia(0, 0, 0);
    if (isDynamic)
        groundShape->calculateLocalInertia(mass, localInertia);

    //using motionstate is optional, it provides interpolation capabilities, and only synchronizes 'active' objects
    btDefaultMotionState * myMotionState = new btDefaultMotionState(groundTransform);
    btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, groundShape, localInertia);
    btRigidBody* body = new btRigidBody(rbInfo);

    //add the body to the dynamics world
    m_world->addRigidBody(body);

    return body;
}

void Bullet::RemoveBody(btRigidBody * body)
{
    m_world->removeRigidBody(body);
    delete body->getMotionState();
    delete body;
}

void Bullet::Step()
{
    m_debug.Clear();
    m_world->stepSimulation(1.0f / 60.f, 10);
}

void Bullet::DebugDraw(const glm::mat4 & view, const glm::mat4 & projection)
{
    m_world->debugDrawWorld();
    m_debug.Draw(view, projection);
}
