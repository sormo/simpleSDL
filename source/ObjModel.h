#pragma once
#include "Shader.h"
#include "Texture.h"
#include "Common.h"
#include "ObjLoader.h"
#include "VboIndexer.h"
#include "TangentSpace.h"

class ObjModel
{
    void DrawSkyboxCommon(GLuint skyboxTexture, const glm::mat4 & model, const glm::mat4 & view, const glm::mat4 & projection, const glm::vec3 & cameraPosition, Shader & shader)
    {
        glm::mat4 MVP = projection * view * model;

        shader.BeginRender();

        shader.SetUniform(MVP, "MVP");
        shader.SetUniform(model, "M");
        shader.SetUniform(cameraPosition, "cameraWorldSpace");

        shader.BindCubemapTexture(skyboxTexture, "textureSkybox");

        shader.BindBuffer<glm::vec3>(m_bufferVertex, "positionModelSpace");
        shader.BindBuffer<glm::vec3>(m_bufferNormal, "normalModelSpace");
        shader.BindElementBuffer(m_bufferIndex);

        glDrawElements(GL_TRIANGLES, m_verticesCount, GL_UNSIGNED_SHORT, (void*)0);

        shader.EndRender();
    }

public:
    ObjModel()
        : m_shader(Common::ReadFileToString("shaders/vertTangent.glsl").c_str(), Common::ReadFileToString("shaders/fragDiffSpecNormSpot.glsl").c_str()),
          m_shaderSkyboxReflection(Common::ReadFileToString("shaders/vertNormal.glsl").c_str(), Common::ReadFileToString("shaders/fragSkyboxReflection.glsl").c_str()),
          m_shaderSkyboxRefraction(Common::ReadFileToString("shaders/vertNormal.glsl").c_str(), Common::ReadFileToString("shaders/fragSkyboxRefraction.glsl").c_str())
    {
        m_verticesCount = InitBuffers();
        if (m_verticesCount == 0)
            throw std::runtime_error("Error loading buffers.");

        if (!LoadTextures())
            throw std::runtime_error("Error loading textures.");
    }
    ~ObjModel()
    {
        glDeleteBuffers(1, &m_bufferVertex);
        glDeleteBuffers(1, &m_bufferNormal);
        glDeleteBuffers(1, &m_bufferUV);
        glDeleteBuffers(1, &m_bufferTangent);
        glDeleteBuffers(1, &m_bufferBitangent);
        glDeleteBuffers(1, &m_bufferIndex);
        glDeleteTextures(1, &m_textureDiffuse);
    }

    void DrawSkyboxRefraction(GLuint skyboxTexture, const glm::mat4 & model, const glm::mat4 & view, const glm::mat4 & projection, const glm::vec3 & cameraPosition)
    {
        DrawSkyboxCommon(skyboxTexture, model, view, projection, cameraPosition, m_shaderSkyboxRefraction);
    }

    void DrawSkyboxReflection(GLuint skyboxTexture, const glm::mat4 & model, const glm::mat4 & view, const glm::mat4 & projection, const glm::vec3 & cameraPosition)
    {
        DrawSkyboxCommon(skyboxTexture, model, view, projection, cameraPosition, m_shaderSkyboxReflection);
    }

    void Draw(const glm::mat4 & model, const glm::mat4 & view, const glm::mat4 & projection, const glm::vec3 & lightPosition, const glm::vec3 & cameraPosition)
    {
        glm::mat4 MVP = projection * view * model;

        m_shader.BeginRender();

        m_shader.SetUniform(MVP, "MVP");
        m_shader.SetUniform(model, "M");
        m_shader.SetUniform(cameraPosition, "cameraWorldSpace");

        m_shader.SetUniform(lightPosition, "light.position");
        m_shader.SetUniform(-lightPosition, "light.direction");
        m_shader.SetUniform(glm::cos(glm::radians(25.5f)), "light.cutOff");
        m_shader.SetUniform(glm::cos(glm::radians(28.5f)), "light.outerCutOff");
        m_shader.SetUniform(glm::vec3(0.1f, 0.1f, 0.1f), "light.ambient");
        m_shader.SetUniform(glm::vec3(1.0f, 1.0f, 1.0f), "light.diffuse");
        m_shader.SetUniform(glm::vec3(1.0f, 1.0f, 1.0f), "light.specular");
        m_shader.SetUniform(1.0f, "light.constant");
        m_shader.SetUniform(0.01f, "light.linear");
        m_shader.SetUniform(0.02f, "light.quadratic");
        m_shader.SetUniform(32.0f, "shininess");

        m_shader.BindTexture(m_textureDiffuse, "textureDiffuse");
        m_shader.BindTexture(m_textureNormal, "textureNormal");
        m_shader.BindTexture(m_textureSpecular, "textureSpecular");

        m_shader.BindBuffer<glm::vec3>(m_bufferVertex, "positionModelSpace");
        m_shader.BindBuffer<glm::vec3>(m_bufferNormal, "normalModelSpace");
        m_shader.BindBuffer<glm::vec2>(m_bufferUV, "vertexUV");
        m_shader.BindBuffer<glm::vec3>(m_bufferTangent, "tangentModelSpace");
        //m_shader.BindBuffer<glm::vec3>(m_bufferBitangent, "bitangentModelSpace");

        m_shader.BindElementBuffer(m_bufferIndex);

        glDrawElements(GL_TRIANGLES, m_verticesCount, GL_UNSIGNED_SHORT, (void*)0);

        m_shader.EndRender();
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

        std::vector<glm::vec3> tangents;
        std::vector<glm::vec3> bitangents;

        ComputeTangentsAndBitangents(vertices, uvs, tangents, bitangents);

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
        //auto textureDiffuse = Texture::Load("models/cylinder/cylinderDiffuse.bmp");
        auto textureDiffuse = Texture::Load("models/cube/diffuse.png");
        if (!textureDiffuse)
        {
            printf("Error loading texture diffuse.png.\n");
            return false;
        }
        m_textureDiffuse = *textureDiffuse;

        auto textureNormal = Texture::Load("models/cube/normal.png");
        if (!textureNormal)
        {
            printf("Error loading texture normal.png.\n");
            return false;
        }
        m_textureNormal = *textureNormal;

        auto textureSpecular = Texture::Load("models/cube/specular.png");
        if (!textureSpecular)
        {
            printf("Error loading texture normal.png.\n");
            return false;
        }
        m_textureSpecular = *textureSpecular;

        return true;
    }

    Shader m_shader;
    Shader m_shaderSkyboxReflection;
    Shader m_shaderSkyboxRefraction;

    GLuint m_bufferVertex;
    GLuint m_bufferNormal;
    GLuint m_bufferUV;
    GLuint m_bufferTangent;
    GLuint m_bufferBitangent;
    GLuint m_bufferIndex;

    GLuint m_textureDiffuse;
    GLuint m_textureNormal;
    GLuint m_textureSpecular;

    GLuint m_verticesCount;

};
