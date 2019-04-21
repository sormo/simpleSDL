#pragma once
#include "DebugDraw.h"

class Editor;

class EditorDebug
{
public:
    EditorDebug(Editor& editor);

    void Draw(const glm::mat4& view, const glm::mat4& projection);
    void Clear();

private:
    Editor& m_editor;
    DebugDraw m_debugDraw;
};
