#include "Shapes.h"
#include "Common.h"

namespace Shapes
{
    Shape::Shape(ModelShader & ashader, Type atype)
        : shader(ashader), type(atype)
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
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
         0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 1.0f, 0.0f, // bottom-right
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 0.0f, 1.0f, // top-left
        // front face
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
         0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f, 1.0f, 0.0f, // bottom-right
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f, 0.0f, 1.0f, // top-left
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
        // left face
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-left
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-right
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
        // right face
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
         0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-right
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
         0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-left
        // bottom face
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
         0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f, 1.0f, 1.0f, // top-left
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f, // bottom-right
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
        // top face
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
         0.5f,  0.5f , 0.5f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
         0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f, 1.0f, 1.0f, // top-right
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f  // bottom-left
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
        : Shape(shader, Type::Cube)
    {
        Init(VERTICES_CUBE, sizeof(VERTICES_CUBE), INDICES_CUBE, sizeof(INDICES_CUBE), 36);
    }

    ///////////////////////////////////////////////////////////////////////////

    static const float PI = 3.1415926f;

    struct Buffers
    {
        std::vector<glm::vec3> vertices;
        std::vector<glm::vec3> normals;
        std::vector<glm::vec2> texCoords;
        std::vector<uint16_t> indices;
    };

    Buffers Merge(const Buffers & buffer1, const Buffers & buffer2)
    {
        Buffers result;

        result.vertices = Common::MergeVectors(buffer1.vertices, buffer2.vertices);
        result.normals = Common::MergeVectors(buffer1.normals, buffer2.normals);
        result.texCoords = Common::MergeVectors(buffer1.texCoords, buffer2.texCoords);

        uint16_t indexOffset = (uint16_t)buffer1.vertices.size();
        result.indices = buffer1.indices;
        for (auto i : buffer2.indices)
            result.indices.push_back(i + indexOffset);

        return result;
    }

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
    Buffers GenerateSphere(uint32_t sectorCount, uint32_t stackCount)
    {
        // TODO is this really radius
        static const float RADIUS = 1.0f;

        Buffers ret;

        float x, y, z, xy;                              // vertex position
        float nx, ny, nz, lengthInv = 1.0f / RADIUS;    // normal
        float s, t;                                     // texCoord

        float sectorStep = 2 * PI / sectorCount;
        float stackStep = PI / stackCount;
        float sectorAngle, stackAngle;

        for (uint32_t i = 0; i <= stackCount; ++i)
        {
            stackAngle = PI / 2 - i * stackStep;        // starting from pi/2 to -pi/2
            xy = RADIUS * cosf(stackAngle);             // r * cos(u)
            z = RADIUS * sinf(stackAngle);              // r * sin(u)

            // add (sectorCount+1) vertices per stack
            // the first and last vertices have same position and normal, but different tex coords
            for (uint32_t j = 0; j <= sectorCount; ++j)
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
        uint32_t k1, k2;
        for (uint32_t i = 0; i < stackCount; ++i)
        {
            k1 = i * (sectorCount + 1);     // beginning of current stack
            k2 = k1 + sectorCount + 1;      // beginning of next stack

            for (uint32_t j = 0; j < sectorCount; ++j, ++k1, ++k2)
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
        : Shape(shader, Type::Sphere)
    {
        auto sphere = GenerateSphere(10, 10);
        auto sphereData = GetShapeBuffer(sphere.vertices, sphere.normals, sphere.texCoords);

        Init(sphereData.data(), sphereData.size()*sizeof(float), sphere.indices.data(), sphere.indices.size() * sizeof(uint16_t), sphere.indices.size());
    }

    ///////////////////////////////////////////////////////////////////////////

    // create circles with offset from xz plane
    Buffers CreateCircle(size_t count, float offset, const glm::vec3 & normal, bool reverse)
    {
        Buffers buffers;

        float angleIncrement = 2.0f * PI / (float)count;
        for (size_t i = 0; i < count; ++i)
        {
            float angle = (float)i * angleIncrement;
            buffers.vertices.push_back({ std::cos(angle), offset, std::sin(angle) });
            buffers.normals.push_back(normal);
            buffers.texCoords.push_back({ 0.0f, 0.0f });
        }

        for (size_t i = 1; i < buffers.vertices.size() - 1; ++i)
        {
            buffers.indices.push_back(0);
            if (reverse)
            {
                buffers.indices.push_back((uint16_t)i);
                buffers.indices.push_back((uint16_t)i + 1);
            }
            else
            {
                buffers.indices.push_back((uint16_t)i + 1);
                buffers.indices.push_back((uint16_t)i);
            }
        }

        return buffers;
    }

    Buffers CreateCylinderWalls(size_t count, float bottom, float upper)
    {
        Buffers buffers;

        auto GenerateCircle = [](size_t count, float offset, Buffers & buffers)
        {
            float angleIncrement = 2.0f * PI / (float)count;
            for (size_t i = 0; i < count; ++i)
            {
                float angle = (float)i * angleIncrement;
                float normalAngle = angle + angleIncrement / 2.0f;
                buffers.vertices.push_back({ std::cos(angle), offset, std::sin(angle) });
                buffers.normals.push_back({ std::cos(normalAngle), 0.0f, std::sin(normalAngle) });
                buffers.texCoords.push_back({ 0.0f, 0.0f });
            }
        };

        GenerateCircle(count, bottom, buffers);
        GenerateCircle(count, upper, buffers);

        for (uint16_t i = 0; i < count; ++i)
        {
            // wrap around
            uint16_t next = i + 1 == count ? 0 : i + 1;
            //   /
            //  / |
            // /  |
            // ----
            buffers.indices.push_back(i);
            buffers.indices.push_back(next + (uint16_t)count);
            buffers.indices.push_back(next);
            // ----
            // |  /
            // | /
            // |/ 
            buffers.indices.push_back(i);
            buffers.indices.push_back(i + (uint16_t)count);
            buffers.indices.push_back(next + (uint16_t)count);
        }

        return buffers;
    }

    Cylinder::Cylinder(ModelShader & shader)
        : Shape(shader, Type::Cylinder)
    {
        static const size_t COUNT = 10;

        auto circleBottom = CreateCircle(COUNT, -0.5f, { 0.0f, -1.0f, 0.0f }, true);
        auto circleUp = CreateCircle(COUNT, 0.5f, { 0.0f, 1.0f, 0.0f }, false);
        auto walls = CreateCylinderWalls(COUNT, -0.5f, 0.5f);
        
        auto merged = Merge(circleBottom, circleUp);
        merged = Merge(merged, walls);

        auto cylinderData = GetShapeBuffer(merged.vertices, merged.normals, merged.texCoords);

        Init(cylinderData.data(), cylinderData.size() * sizeof(float), merged.indices.data(), merged.indices.size() * sizeof(uint16_t), merged.indices.size());
    }

    ///////////////////////////////////////////////////////////////////////////

    Buffers CreateConeWalls(size_t count, float bottom, float upper)
    {
        Buffers buffers;

        float angleIncrement = 2.0f * PI / (float)count;
        // create bottom circle
        for (size_t i = 0; i < count; ++i)
        {
            float angle = (float)i * angleIncrement;
            float normalAngle = angle + angleIncrement / 2.0f;
            buffers.vertices.push_back({ std::cos(angle), bottom, std::sin(angle) });
            buffers.normals.push_back({ std::cos(normalAngle), 0.0f, std::sin(normalAngle) });
            buffers.texCoords.push_back({ 0.0f, 0.0f });
        }
        // create the point
        for (size_t i = 0; i < count; ++i)
        {
            float angle = (float)i * angleIncrement;
            float normalAngle = angle + angleIncrement / 2.0f;
            buffers.vertices.push_back({ 0.0f, upper, 0.0f });
            buffers.normals.push_back({ std::cos(normalAngle), 0.0f, std::sin(normalAngle) });
            buffers.texCoords.push_back({ 0.0f, 0.0f });
        }

        for (uint16_t i = 0; i < count; ++i)
        {
            // wrap around
            uint16_t next = i + 1 == count ? 0 : i + 1;

            buffers.indices.push_back(i);
            buffers.indices.push_back(i + count);
            buffers.indices.push_back(next);
        }

        return buffers;
    }

    Cone::Cone(ModelShader & shader)
        : Shape(shader, Type::Cone)
    {
        static const size_t COUNT = 10;

        auto circleBottom = CreateCircle(COUNT, -0.5f, { 0.0f, -1.0f, 0.0f }, true);
        auto walls = CreateConeWalls(COUNT, -0.5f, 0.5f);
        auto merged = Merge(circleBottom, walls);
        auto coneData = GetShapeBuffer(merged.vertices, merged.normals, merged.texCoords);

        Init(coneData.data(), coneData.size() * sizeof(float), merged.indices.data(), merged.indices.size() * sizeof(uint16_t), merged.indices.size());
    }
}
