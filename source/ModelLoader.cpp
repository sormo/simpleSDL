#include "ModelLoader.h"
#include "Common.h"
#include <sstream>
#include <regex>

bool LoadObj(const char * path,
             std::vector<glm::vec3> & vertices,
             std::vector<glm::vec2> & uvs,
             std::vector<glm::vec3> & normals)
{
    printf("Loading OBJ file %s.\n", path);

    std::vector<uint8_t> data = Common::ReadFile(path);
    if (data.empty())
    {
        printf("Error reading OBJ file data.");
        return false;
    }

    // append zero and consider data as string
    data.push_back('\0');
    std::stringstream dataStream((const char *)data.data());
    std::string line;

    std::vector<glm::vec2> tempUvs;
    std::vector<glm::vec3> tempVertices, tempNormals;
    std::vector<uint32_t> indicesVertex, indicesUv, indicesNormal;

    while(std::getline(dataStream, line))
    {
#define FLOAT_PATTERN "([+-]?[0-9]*[.]?[0-9]+)"
#define FACE_VERTEX_PATTERN "([0-9]+)/([0-9]+)/([0-9]+)"

        static std::regex patternVertex("^v " FLOAT_PATTERN " " FLOAT_PATTERN " " FLOAT_PATTERN);
        static std::regex patternTexture("^vt " FLOAT_PATTERN " " FLOAT_PATTERN);
        static std::regex patternNormal("^vn " FLOAT_PATTERN " " FLOAT_PATTERN " " FLOAT_PATTERN);
        static std::regex patternFace("^f " FACE_VERTEX_PATTERN " " FACE_VERTEX_PATTERN " " FACE_VERTEX_PATTERN);

        std::smatch match;
        if (std::regex_search(line, match, patternVertex))
        {
            glm::vec3 vertex(std::stof(match[1]), std::stof(match[2]), std::stof(match[3]));
            tempVertices.push_back(std::move(vertex));
        }
        else if (std::regex_search(line, match, patternTexture))
        {
            glm::vec2 uv(std::stof(match[1]), std::stof(match[2]));
            uv.y = -uv.y; // Invert V coordinate since we will only use DDS texture, which are inverted. Remove if you want to use TGA or BMP loaders.
            tempUvs.push_back(std::move(uv));
        }
        else if (std::regex_search(line, match, patternNormal))
        {
            glm::vec3 normal(std::stof(match[1]), std::stof(match[2]), std::stof(match[3]));
            tempNormals.push_back(std::move(normal));
        }
        else if (std::regex_search(line, match, patternFace))
        {
            indicesVertex.insert(std::end(indicesVertex), { (uint32_t)std::stoi(match[1]), (uint32_t)std::stoi(match[4]), (uint32_t)std::stoi(match[7]) });
            indicesUv.insert(std::end(indicesUv), { (uint32_t)std::stoi(match[2]), (uint32_t)std::stoi(match[5]), (uint32_t)std::stoi(match[8]) });
            indicesNormal.insert(std::end(indicesNormal), { (uint32_t)std::stoi(match[3]), (uint32_t)std::stoi(match[6]), (uint32_t)std::stoi(match[9]) });
        }
    }

    // For each vertex of each triangle
    for(size_t i = 0; i < indicesVertex.size(); ++i )
    {
        // Get the indices of its attributes
        uint32_t vertexIndex = indicesVertex[i];
        uint32_t uvIndex = indicesUv[i];
        uint32_t normalIndex = indicesNormal[i];

        // Get the attributes thanks to the index
        glm::vec3 vertex = tempVertices[ vertexIndex-1 ];
        glm::vec2 uv = tempUvs[ uvIndex-1 ];
        glm::vec3 normal = tempNormals[ normalIndex-1 ];

        // Put the attributes in buffers
        vertices.push_back(std::move(vertex));
        uvs.push_back(std::move(uv));
        normals.push_back(std::move(normal));
    }

    return true;
}
