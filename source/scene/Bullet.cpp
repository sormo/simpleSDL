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

btBoxShape * Bullet::CreateShape(const Shapes::Defintion::Box & definition)
{
    return new btBoxShape(Convert(definition.extents / 2.0f));
}

btSphereShape * Bullet::CreateShape(const Shapes::Defintion::Sphere & definition)
{
    return new btSphereShape(definition.radius);
}

btCylinderShape * Bullet::CreateShape(const Shapes::Defintion::Cylinder & definition)
{
    // TODO why /2 ???
    return new btCylinderShape({ definition.radius, definition.height / 2.0f, definition.radius });
}

btConeShape * Bullet::CreateShape(const Shapes::Defintion::Cone & definition)
{
    return new btConeShape(definition.radius, definition.height);
}

btRigidBody * Bullet::AddBox(const Shapes::Defintion::Box & definition, bool isStatic)
{
    return AddCommon(definition.position, definition.rotation, isStatic, CreateShape(definition));
}

btRigidBody * Bullet::AddSphere(const Shapes::Defintion::Sphere & definition, bool isStatic)
{
    return AddCommon(definition.position, definition.rotation, isStatic, CreateShape(definition));
}

btRigidBody * Bullet::AddCylinder(const Shapes::Defintion::Cylinder & definition, bool isStatic)
{
    return AddCommon(definition.position, definition.rotation, isStatic, CreateShape(definition));
}

btRigidBody * Bullet::AddCone(const Shapes::Defintion::Cone & definition, bool isStatic)
{
    return AddCommon(definition.position, definition.rotation, isStatic, CreateShape(definition));
}

btRigidBody * Bullet::AddCompound(const glm::vec3 & position, const glm::vec3 & rotation, bool isStatic)
{
    btCompoundShape * shape = new btCompoundShape();

    return AddCommon(position, rotation, isStatic, shape);
}

template<class T>
btCollisionShape * Bullet::AddShape(btRigidBody * body, const T & definition)
{
    assert(body->getCollisionShape()->isCompound());

    btCollisionShape * result = CreateShape(definition);

    btQuaternion quatRotation;
    quatRotation.setEulerZYX(definition.rotation.x, definition.rotation.y, definition.rotation.z);
    btTransform transform(quatRotation, Convert(definition.position));

    //btCompoundShape * parent = dynamic_cast<btCompoundShape*>(body->getCollisionShape());
    btCompoundShape* parent = static_cast<btCompoundShape*>(body->getCollisionShape());

    parent->addChildShape(transform, result);

    if (!body->isStaticObject())
    {
        btVector3 inertia;
        btScalar mass = body->getInvMass();

        parent->calculateLocalInertia(mass, inertia);
        body->setMassProps(mass, inertia);
    }

    return result;
}

template class btCollisionShape * Bullet::AddShape(btRigidBody * body, const Shapes::Defintion::Box & definition);
template class btCollisionShape * Bullet::AddShape(btRigidBody * body, const Shapes::Defintion::Sphere & definition);
template class btCollisionShape * Bullet::AddShape(btRigidBody * body, const Shapes::Defintion::Cylinder & definition);
template class btCollisionShape * Bullet::AddShape(btRigidBody * body, const Shapes::Defintion::Cone & definition);

void Bullet::RemoveShape(btRigidBody * body, btCollisionShape * shape)
{
    assert(body->getCollisionShape()->isCompound());

    //btCompoundShape * parent = dynamic_cast<btCompoundShape*>(body->getCollisionShape());
    btCompoundShape* parent = static_cast<btCompoundShape*>(body->getCollisionShape());

    parent->removeChildShape(shape);
}

btRigidBody * Bullet::AddCommon(const glm::vec3 & position, const glm::vec3 & rotation, bool isStatic, btCollisionShape * shape)
{
    m_collisionShapes.push_back(shape);

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
        shape->calculateLocalInertia(mass, localInertia);

    //using motionstate is optional, it provides interpolation capabilities, and only synchronizes 'active' objects
    btDefaultMotionState * myMotionState = new btDefaultMotionState(groundTransform);
    btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, shape, localInertia);
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

struct RayCastResult : public btCollisionWorld::RayResultCallback
{
    std::vector<Bullet::RayResult> bodies;
    const btVector3 fromPoint;
    const btVector3 toPoint;

    RayCastResult(const btVector3 & from, const btVector3 to) : fromPoint(from), toPoint(to) {}

    virtual btScalar addSingleResult(btCollisionWorld::LocalRayResult& rayResult, bool normalInWorldSpace) override
    {
        const btCollisionShape* shape = rayResult.m_collisionObject->getCollisionShape();
        if (shape->getShapeType() == COMPOUND_SHAPE_PROXYTYPE)
        {
            shape = static_cast<const btCompoundShape*>(shape)->getChildShape(rayResult.m_localShapeInfo->m_triangleIndex);
        }
        
        btVector3 worldPoint;
        if (normalInWorldSpace)
        {
            worldPoint = rayResult.m_hitNormalLocal;
        }
        else
        {
            worldPoint = rayResult.m_collisionObject->getWorldTransform().getBasis() * rayResult.m_hitNormalLocal;
        }

        // should be used closest hitFraction ???
        worldPoint.setInterpolate3(fromPoint, toPoint, rayResult.m_hitFraction);

        bodies.push_back({ rayResult.m_collisionObject, shape, Convert(worldPoint) });

        return 0.0f;
    }
};

std::vector<Bullet::RayResult> Bullet::RayCast(const glm::vec3 & position, const glm::vec3 & direction)
{
    static const float TEST_DISTANCE = 200.0f;
    glm::vec3 destination = position + glm::normalize(direction) * TEST_DISTANCE;
    RayCastResult result(Convert(position), Convert(destination));

    m_world->getCollisionWorld()->rayTest(result.fromPoint, result.toPoint, result);

    return result.bodies;
}
