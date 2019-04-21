#include "EditorDebug.h"
#include "Editor.h"

EditorDebug::EditorDebug(Editor& editor)
    : m_editor(editor)
{

}

void EditorDebug::Draw(const glm::mat4& view, const glm::mat4& projection)
{


    m_debugDraw.Draw(view, projection);
}

void EditorDebug::Clear()
{
    m_debugDraw.Clear();
}
