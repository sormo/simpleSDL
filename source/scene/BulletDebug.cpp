#include "BulletDebug.h"

void BulletDebug::drawLine(const btVector3& from, const btVector3& to, const btVector3& color)
{
    m_debugDraw.DrawLine({ from.x(), from.y(), from.z() }, { to.x(), to.y(), to.z() }, { color.x(), color.y(), color.z(), 1.0f });
}

void BulletDebug::Draw(const glm::mat4 & view, const glm::mat4 & projection)
{
    m_debugDraw.Draw(view, projection);
}

void BulletDebug::Clear()
{
    m_debugDraw.Clear();
}

void BulletDebug::drawContactPoint(const btVector3& PointOnB, const btVector3& normalOnB, btScalar distance, int lifeTime, const btVector3& color)
{

}

void BulletDebug::reportErrorWarning(const char* warningString)
{

}

void BulletDebug::draw3dText(const btVector3& location, const char* textString)
{

}

void BulletDebug::setDebugMode(int debugMode)
{
    m_debugMode = debugMode;
}

int BulletDebug::getDebugMode() const
{
    return m_debugMode;
}
