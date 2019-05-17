#pragma once
#include "OpenGL.h"
#include <btBulletDynamicsCommon.h>
#include "utils/Shader.h"
#include "DebugDraw.h"

class BulletDebug : public btIDebugDraw
{
public:
    void Draw(const glm::mat4 & view, const glm::mat4 & projection);
    void Clear();

public:
    virtual void drawLine(const btVector3& from, const btVector3& to, const btVector3& color) override;
    virtual void drawContactPoint(const btVector3& PointOnB, const btVector3& normalOnB, btScalar distance, int lifeTime, const btVector3& color) override;
    virtual void reportErrorWarning(const char* warningString) override;
    virtual void draw3dText(const btVector3& location, const char* textString) override;
    virtual void setDebugMode(int debugMode) override;
    virtual int getDebugMode() const override;

    int m_debugMode;

    DebugDraw m_debugDraw;
};
