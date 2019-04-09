#include "Shapes.h"

namespace Shapes
{
    Shape::Shape(ModelShader & ashader)
        : shader(ashader)
    {

    }

    Shape::~Shape()
    {
        glDeleteBuffers(1, &vboData);
        glDeleteBuffers(1, &vboIndices);
        glDeleteVertexArrays(1, &vao);
    }

    void Shape::Init(const float * data, size_t dataSize, const uint16_t * indices, size_t indicesSize, size_t acount)
    {
        count = acount;

        if (IsVAOSupported())
        {
            glGenVertexArrays(1, &vao);
            glBindVertexArray(vao);
        }

        glGenBuffers(1, &vboData);
        glBindBuffer(GL_ARRAY_BUFFER, vboData);
        glBufferData(GL_ARRAY_BUFFER, dataSize, data, GL_STATIC_DRAW);
        CheckGlError("glBufferData");

        if (IsVAOSupported())
        {
            glEnableVertexAttribArray(shader.GetLocations().buffers.positions);
            glVertexAttribPointer(shader.GetLocations().buffers.positions, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);

            glEnableVertexAttribArray(shader.GetLocations().buffers.normals);
            glVertexAttribPointer(shader.GetLocations().buffers.normals, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));

            //glEnableVertexAttribArray(m_locationVertexUV);
            //glVertexAttribPointer(m_locationVertexUV, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));

            CheckGlError("glEnableVertexAttribArray");
        }

        glGenBuffers(1, &vboIndices);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboIndices);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicesSize, indices, GL_STATIC_DRAW);
        CheckGlError("glBufferData");

        if (IsVAOSupported())
            glBindVertexArray(0);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }

    void Shape::Bind()
    {
        if (IsVAOSupported())
        {
            shader.GetShader().BindVAO(vao);
        }
        else
        {
            // TODO this should be bound using shader

            glBindBuffer(GL_ARRAY_BUFFER, vboData);

            glEnableVertexAttribArray(shader.GetLocations().buffers.positions);
            glVertexAttribPointer(shader.GetLocations().buffers.positions, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);

            glEnableVertexAttribArray(shader.GetLocations().buffers.normals);
            glVertexAttribPointer(shader.GetLocations().buffers.normals, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));

            //glEnableVertexAttribArray(m_locationVertexUV);
            //glVertexAttribPointer(m_locationVertexUV, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
        }

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboIndices);
    }

    void Shape::Draw()
    {
        glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_SHORT, (void*)0);
        //glDrawArrays(GL_TRIANGLES, 0, count);
        CheckGlError("glDrawArrays");
    }

    ///////////////////////////////////////////////////////////////////////////

    static const float VERTICES_CUBE[] =
    {
        // back face
        -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
         1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
         1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 0.0f, // bottom-right
         1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
        -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
        -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 1.0f, // top-left
        // front face
        -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
         1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 0.0f, // bottom-right
         1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
         1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
        -1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 1.0f, // top-left
        -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
        // left face
        -1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
        -1.0f,  1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-left
        -1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
        -1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
        -1.0f, -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-right
        -1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
        // right face
         1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
         1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
         1.0f,  1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-right
         1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
         1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
         1.0f, -1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-left
        // bottom face
        -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
         1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 1.0f, // top-left
         1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
         1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
        -1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f, // bottom-right
        -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
        // top face
        -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
         1.0f,  1.0f , 1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
         1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 1.0f, // top-right
         1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
        -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
        -1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f  // bottom-left
    };

    // TODO
    uint16_t INDICES_CUBE[] =
    {
        0, 1, 2, 3, 4, 5, 6, 7,
        8, 9, 10, 11, 12, 13, 14, 15,
        16, 17, 18, 19, 20, 21, 22, 23,
        24, 25, 26, 27, 28, 29, 30, 31,
        32, 33, 34, 35
    };

    Cube::Cube(ModelShader & shader)
        : Shape(shader)
    {
        Init(VERTICES_CUBE, sizeof(VERTICES_CUBE), INDICES_CUBE, sizeof(INDICES_CUBE), 36);
    }

    ///////////////////////////////////////////////////////////////////////////

    struct Buffers
    {
        std::vector<glm::vec3> vertices;
        std::vector<glm::vec3> normals;
        std::vector<glm::vec2> texCoords;
        std::vector<uint16_t> indices;
    };

    std::vector<float> GetShapeBuffer(const std::vector<glm::vec3> & vertices, const std::vector<glm::vec3> & normals, const std::vector<glm::vec2> & texCoords)
    {
        std::vector<float> result;

        auto pushVec3 = [](const glm::vec3 & v, std::vector<float> & r)
        {
            r.push_back(v.x);
            r.push_back(v.y);
            r.push_back(v.z);
        };

        auto pushVec2 = [](const glm::vec2 & v, std::vector<float> & r)
        {
            r.push_back(v.x);
            r.push_back(v.y);
        };

        for (size_t i = 0; i < vertices.size(); ++i)
        {
            pushVec3(vertices[i], result);
            pushVec3(normals[i], result);
            pushVec2(texCoords[i], result);
        }

        return result;
    }

    ///////////////////////////////////////////////////////////////////////////

    // modified: http://www.songho.ca/opengl/gl_sphere.html
    // sectors goes horizontaly and stacks verticaly
    Buffers GenerateCircle(uint32_t sectorCount, uint32_t stackCount)
    {
        static const float PI = 3.1415926f;
        static const float RADIUS = 1.0f;

        Buffers ret;

        float x, y, z, xy;                              // vertex position
        float nx, ny, nz, lengthInv = 1.0f / RADIUS;    // normal
        float s, t;                                     // texCoord

        float sectorStep = 2 * PI / sectorCount;
        float stackStep = PI / stackCount;
        float sectorAngle, stackAngle;

        for (int i = 0; i <= stackCount; ++i)
        {
            stackAngle = PI / 2 - i * stackStep;        // starting from pi/2 to -pi/2
            xy = RADIUS * cosf(stackAngle);             // r * cos(u)
            z = RADIUS * sinf(stackAngle);              // r * sin(u)

            // add (sectorCount+1) vertices per stack
            // the first and last vertices have same position and normal, but different tex coords
            for (int j = 0; j <= sectorCount; ++j)
            {
                sectorAngle = j * sectorStep;           // starting from 0 to 2pi

                // vertex position
                x = xy * cosf(sectorAngle);             // r * cos(u) * cos(v)
                y = xy * sinf(sectorAngle);             // r * cos(u) * sin(v)
                ret.vertices.emplace_back(x, y, z);

                // normalized vertex normal
                nx = x * lengthInv;
                ny = y * lengthInv;
                nz = z * lengthInv;
                ret.normals.emplace_back(nx, ny, nz);

                // vertex tex coord between [0, 1]
                s = (float)j / sectorCount;
                t = (float)i / stackCount;
                ret.texCoords.emplace_back(s, t);
            }
        }

        // indices
        //  k1--k1+1
        //  |  / |
        //  | /  |
        //  k2--k2+1
        unsigned int k1, k2;
        for (int i = 0; i < stackCount; ++i)
        {
            k1 = i * (sectorCount + 1);     // beginning of current stack
            k2 = k1 + sectorCount + 1;      // beginning of next stack

            for (int j = 0; j < sectorCount; ++j, ++k1, ++k2)
            {
                // 2 triangles per sector excluding 1st and last stacks
                if (i != 0)
                {
                    // k1---k2---k1+1
                    ret.indices.push_back(k1);
                    ret.indices.push_back(k2);
                    ret.indices.push_back(k1 + 1);
                }

                if (i != (stackCount - 1))
                {
                    // k1+1---k2---k2+1
                    ret.indices.push_back(k1 + 1);
                    ret.indices.push_back(k2);
                    ret.indices.push_back(k2 + 1);
                }
            }
        }

        return ret;
    }

    Sphere::Sphere(ModelShader & shader)
        : Shape(shader)
    {
        auto circle = GenerateCircle(10, 10);
        auto circleData = GetShapeBuffer(circle.vertices, circle.normals, circle.texCoords);

        Init(circleData.data(), circleData.size()*sizeof(float), circle.indices.data(), circle.indices.size() * sizeof(uint16_t), circle.indices.size());
    }
}
