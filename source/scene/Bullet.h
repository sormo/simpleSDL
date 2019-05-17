#pragma once

#include <btBulletDynamicsCommon.h>
#include "glm/glm.hpp"
#include <memory>
#include "BulletDebug.h"
// include shapes because of defintions
#include "Shapes.h"

class Bullet
{
public:
    Bullet(const glm::vec3 & gravity);
    ~Bullet();

    // TODO in a case single shape body is created Shapes::Definition position and rotation is actaully body position and rotation
    btRigidBody * AddBox(const Shapes::Defintion::Box & definition, bool isStatic);
    btRigidBody * AddSphere(const Shapes::Defintion::Sphere & definition, bool isStatic);
    btRigidBody * AddCylinder(const Shapes::Defintion::Cylinder & definition, bool isStatic);
    btRigidBody * AddCone(const Shapes::Defintion::Cone & definition, bool isStatic);

    btRigidBody * AddCompound(const glm::vec3 & position, const glm::vec3 & rotation, bool isStatic);
    template<class T>
    btCollisionShape * AddShape(btRigidBody * body, const T & definition);
    void RemoveShape(btRigidBody * body, btCollisionShape * shape);

    void RemoveBody(btRigidBody * body);

    void Step();
    void DebugDraw(const glm::mat4 & view, const glm::mat4 & projection);

    struct RayResult
    {
        const btCollisionObject * object;
        const btCollisionShape * shape;
        glm::vec3 worldPoint;
    };
    std::vector<RayResult> RayCast(const glm::vec3 & position, const glm::vec3 & direction);

private:
    btBoxShape * CreateShape(const Shapes::Defintion::Box & definition);
    btSphereShape * CreateShape(const Shapes::Defintion::Sphere & definition);
    btCylinderShape * CreateShape(const Shapes::Defintion::Cylinder & definition);
    btConeShape * CreateShape(const Shapes::Defintion::Cone & definition);

    btRigidBody * AddCommon(const glm::vec3 & position, const glm::vec3 & rotation, bool isStatic, btCollisionShape * shape);

    BulletDebug m_debug;

    std::unique_ptr<btDefaultCollisionConfiguration> m_collisionConfiguration;
    std::unique_ptr<btCollisionDispatcher> m_dispatcher;
    std::unique_ptr<btBroadphaseInterface> m_broadphase;
    std::unique_ptr<btSequentialImpulseConstraintSolver> m_solver;
    std::unique_ptr<btDiscreteDynamicsWorld> m_world;

    btAlignedObjectArray<btCollisionShape*> m_collisionShapes;
};
