#pragma once
#include "Shader.h"
#include "Texture.h"
#include "Common.h"
#include "ModelLoader.h"
#include "VboIndexer.h"

class Cylinder
{
public:
    Cylinder()
        : m_shader("shaderVertex.glsl", "shaderFragment.glsl")
    {
        m_verticesCount = InitBuffers();
        if (m_verticesCount == 0)
            throw std::runtime_error("Error loading buffers for Cylinder.");

        if (!LoadTextures())
            throw std::runtime_error("Error loading textures for Cylinder.");
    }
    ~Cylinder()
    {
        glDeleteBuffers(1, &m_bufferVertex);
        glDeleteBuffers(1, &m_bufferNormal);
        glDeleteBuffers(1, &m_bufferUV);
        glDeleteBuffers(1, &m_bufferTangent);
        glDeleteBuffers(1, &m_bufferBitangent);
        glDeleteBuffers(1, &m_bufferIndex);
        glDeleteTextures(1, &m_textureDiffuse);
    }

    void Draw(const glm::mat4 & model, const glm::mat4 & view, const glm::mat4 & projection, const glm::vec3 & lightPosition)
    {
        glm::mat4 MVP = projection * view * model;

        m_shader.Begin();

        m_shader.SetUniform(MVP, "MVP");
        m_shader.SetUniform(glm::mat3(view * model), "MV3x3");
        m_shader.SetUniform(model, "M");
        m_shader.SetUniform(view, "V");
        m_shader.SetUniform(lightPosition, "lightPositionWorldSpace");

        m_shader.BindTexture(m_textureDiffuse, "textureDiffuse");
        m_shader.BindTexture(m_textureNormal, "textureNormal");

        m_shader.BindBuffer<glm::vec3>(m_bufferVertex, "positionModelSpace");
        m_shader.BindBuffer<glm::vec3>(m_bufferNormal, "normalModelSpace");
        m_shader.BindBuffer<glm::vec2>(m_bufferUV, "vertexUV");
        m_shader.BindBuffer<glm::vec3>(m_bufferTangent, "tangentModelSpace");
        m_shader.BindBuffer<glm::vec3>(m_bufferBitangent, "bitangentModelSpace");

        m_shader.BindElementBuffer(m_bufferIndex);

        glDrawElements(GL_TRIANGLES,      // mode
                       m_verticesCount,   // count
                       GL_UNSIGNED_SHORT, // type
                       (void*)0);         // element array buffer offset

        m_shader.End();
    }

private:
    // return number of vertices initialized
    uint32_t InitBuffers()
    {
        std::vector<glm::vec3> vertices;
        std::vector<glm::vec2> uvs;
        std::vector<glm::vec3> normals;

        if (!LoadObj("models/cylinder/cylinder.obj", vertices, uvs, normals))
        {
            printf("Error loading obj file.");
            return 0;
        }

        std::vector<glm::vec3> tangents;
        std::vector<glm::vec3> bitangents;

        Common::ComputeTangentsAndBitangents(vertices, uvs, tangents, bitangents);

        auto indexed = VboIndex(vertices, uvs, normals, tangents, bitangents);

        glGenBuffers(1, &m_bufferVertex);
        glBindBuffer(GL_ARRAY_BUFFER, m_bufferVertex);
        glBufferData(GL_ARRAY_BUFFER, indexed.vertices.size() * sizeof(glm::vec3), &indexed.vertices[0], GL_STATIC_DRAW);

        glGenBuffers(1, &m_bufferUV);
        glBindBuffer(GL_ARRAY_BUFFER, m_bufferUV);
        glBufferData(GL_ARRAY_BUFFER, indexed.uvs.size() * sizeof(glm::vec2), &indexed.uvs[0], GL_STATIC_DRAW);

        glGenBuffers(1, &m_bufferNormal);
        glBindBuffer(GL_ARRAY_BUFFER, m_bufferNormal);
        glBufferData(GL_ARRAY_BUFFER, indexed.normals.size() * sizeof(glm::vec3), &indexed.normals[0], GL_STATIC_DRAW);

        glGenBuffers(1, &m_bufferTangent);
        glBindBuffer(GL_ARRAY_BUFFER, m_bufferTangent);
        glBufferData(GL_ARRAY_BUFFER, indexed.tangents.size() * sizeof(glm::vec3), &indexed.tangents[0], GL_STATIC_DRAW);

        glGenBuffers(1, &m_bufferBitangent);
        glBindBuffer(GL_ARRAY_BUFFER, m_bufferBitangent);
        glBufferData(GL_ARRAY_BUFFER, indexed.bitangents.size() * sizeof(glm::vec3), &indexed.bitangents[0], GL_STATIC_DRAW);

        glGenBuffers(1, &m_bufferIndex);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_bufferIndex);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexed.indices.size() * sizeof(uint16_t), &indexed.indices[0], GL_STATIC_DRAW);

        return indexed.indices.size();
    }

    bool LoadTextures()
    {
        auto textureDiffuse = Texture::Load("models/cylinder/cylinderDiffuse.bmp");
        if (!textureDiffuse)
        {
            printf("Error loading texture cylinderDiffuse.bmp.\n");
            return false;
        }
        m_textureDiffuse = *textureDiffuse;

        auto textureNormal = Texture::Load("models/cylinder/cylinderNormal.bmp");
        if (!textureNormal)
        {
            printf("Error loading texture cylinderNormal.bmp.\n");
            return false;
        }
        m_textureNormal = *textureNormal;

        return true;
    }

    Shader m_shader;

    GLuint m_bufferVertex;
    GLuint m_bufferNormal;
    GLuint m_bufferUV;
    GLuint m_bufferTangent;
    GLuint m_bufferBitangent;
    GLuint m_bufferIndex;

    GLuint m_textureDiffuse;
    GLuint m_textureNormal;

    GLuint m_verticesCount;

};
