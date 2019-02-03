#pragma once
#include "glm/glm.hpp"
#include "Shader.h"
#include "ModelLoader.h"
#include "VboIndexer.h"

class Light
{
public:
    Light()
        : m_shader("shaders/vertLight.glsl", "shaders/fragLight.glsl")
    {
        m_verticesCount = InitBuffers();
        if (m_verticesCount == 0)
            throw std::runtime_error("Error loading buffers for Light.");
    }

    ~Light()
    {
        glDeleteBuffers(1, &m_bufferVertex);
        glDeleteBuffers(1, &m_bufferIndex);
    }

    void Draw(const glm::mat4 & model, const glm::mat4 & view, const glm::mat4 & projection)
    {
        glm::mat4 MVP = projection * view * model;

        m_shader.Begin();

        m_shader.SetUniform(MVP, "MVP");
        m_shader.BindBuffer<glm::vec3>(m_bufferVertex, "positionModelSpace");
        m_shader.BindElementBuffer(m_bufferIndex);

        glDrawElements(GL_TRIANGLES,      // mode
                       m_verticesCount,   // count
                       GL_UNSIGNED_SHORT, // type
                       (void*)0);         // element array buffer offset

        m_shader.CleanUp();
    }

private:
    // return number of vertices initialized
    uint32_t InitBuffers()
    {
        std::vector<glm::vec3> vertices;
        std::vector<glm::vec2> uvs;
        std::vector<glm::vec3> normals;

        if (!LoadObj("models/cube/cube.obj", vertices, uvs, normals))
        {
            printf("Error loading obj file.");
            return 0;
        }

        auto indexed = VboIndex(vertices);

        glGenBuffers(1, &m_bufferVertex);
        glBindBuffer(GL_ARRAY_BUFFER, m_bufferVertex);
        glBufferData(GL_ARRAY_BUFFER, indexed.vertices.size() * sizeof(glm::vec3), &indexed.vertices[0], GL_STATIC_DRAW);

        glGenBuffers(1, &m_bufferIndex);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_bufferIndex);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexed.indices.size() * sizeof(uint16_t), &indexed.indices[0], GL_STATIC_DRAW);

        return indexed.indices.size();
    }

    Shader m_shader;

    GLuint m_bufferVertex;
    GLuint m_bufferIndex;
    GLuint m_verticesCount;
};
