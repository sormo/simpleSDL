#pragma once

#include <btBulletDynamicsCommon.h>
#include "glm/glm.hpp"
#include <memory>
#include "BulletDebug.h"

class Bullet
{
public:
    Bullet(const glm::vec3 & gravity);
    ~Bullet();

    // rotation is rotation around x, y, z axes
    // TODO maybe hide bullet completly ???
    btRigidBody * AddBox(const glm::vec3 & position, const glm::vec3 & rotation, const glm::vec3 & halfExtents, bool isStatic);
    btRigidBody * AddSphere(const glm::vec3 & position, float radius, bool isStatic);
    void RemoveBody(btRigidBody * body);

    void Step();
    void DebugDraw(const glm::mat4 & view, const glm::mat4 & projection);

private:
    BulletDebug m_debug;

    std::unique_ptr<btDefaultCollisionConfiguration> m_collisionConfiguration;
    std::unique_ptr<btCollisionDispatcher> m_dispatcher;
    std::unique_ptr<btBroadphaseInterface> m_broadphase;
    std::unique_ptr<btSequentialImpulseConstraintSolver> m_solver;
    std::unique_ptr<btDiscreteDynamicsWorld> m_world;

    btAlignedObjectArray<btCollisionShape*> m_collisionShapes;
};
