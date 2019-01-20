#include "Text2D.h"
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Shader.h"
#include "Texture.h"
#include "Common.h"
#include <sstream>

GLuint g_texture;
GLuint g_bufferVertex;
GLuint g_bufferUV;
GLuint g_program;
GLuint g_locationVertex;
GLuint g_locationUV;
GLuint g_locationTexture;
std::vector<float> g_widths;

static const int32_t FIRST_CHAR_ASCII = 32;
static const int32_t CHARS_IN_ROW = 16;
static const int32_t CHAR_SIZE_PIXELS = 32;

// expect only consecutive character widths as numbers
void Text2DReadWidthFactors(const char * path)
{
    std::vector<uint8_t> data = Common::ReadFile(path);
    data.push_back('\0');

    std::stringstream stream((char*)data.data());
    while (stream)
    {
        uint32_t width;
        stream >> width;

        g_widths.push_back((float)width / (float)CHAR_SIZE_PIXELS);
    }
}

bool Text2DInitFont(const char * path)
{
    // Initialize texture
    auto texture = Texture::Load(path);
    if (!texture)
        return false;
    g_texture = *texture;

    // Initialize VBO
    glGenBuffers(1, &g_bufferVertex);
    glGenBuffers(1, &g_bufferUV);

    // Initialize Shader
    auto program = CreateProgram("shaderVertexText.glsl", "shaderFragmentText.glsl");
    if (!program)
        return false;
    g_program = *program;

    // Get a handle for our buffers
    g_locationVertex = glGetAttribLocation(g_program, "positionScreenSpace");
    g_locationUV = glGetAttribLocation(g_program, "vertexUV");

    // Initialize uniforms' IDs
    g_locationTexture = glGetUniformLocation(g_program, "textureValue");

    Text2DReadWidthFactors("text2d-widths.txt");

    return true;
}

void Text2DPrint(const char * text, int32_t x, int32_t y, int32_t size)
{
    static const float UV_CELL_SIZE = 1.0f / (float)CHARS_IN_ROW;

    // Fill buffers
    std::vector<glm::vec2> vertices;
    std::vector<glm::vec2> UVs;

    float widthCursor = 0.0f;

    for (size_t i = 0; i < strlen(text); i++)
    {
        int32_t characterIndex = (int32_t)text[i] - FIRST_CHAR_ASCII;

        float nextWidthCursor = widthCursor + g_widths[characterIndex] * (float)size;

        glm::vec2 upLeft    = glm::vec2(x + widthCursor    , y + size);
        glm::vec2 upRight   = glm::vec2(x + nextWidthCursor, y + size);
        glm::vec2 downRight = glm::vec2(x + nextWidthCursor, y       );
        glm::vec2 downLeft  = glm::vec2(x + widthCursor    , y       );

        widthCursor = nextWidthCursor;

        vertices.push_back(upLeft);
        vertices.push_back(downLeft);
        vertices.push_back(upRight);

        vertices.push_back(downRight);
        vertices.push_back(upRight);
        vertices.push_back(downLeft);

        float uvx = (characterIndex % CHARS_IN_ROW) / (float)CHARS_IN_ROW;
        float uvy = (characterIndex / CHARS_IN_ROW) / (float)CHARS_IN_ROW;

        // uvy is from upper left but uv coordinates are from bottom right
        uvy = 1.0f - uvy - UV_CELL_SIZE;
        float uvWidthSize = UV_CELL_SIZE *g_widths[characterIndex];

        glm::vec2 uvUpLeft    = glm::vec2(uvx              , uvy + UV_CELL_SIZE);
        glm::vec2 uvUpRight   = glm::vec2(uvx + uvWidthSize, uvy + UV_CELL_SIZE);
        glm::vec2 uvDownRight = glm::vec2(uvx + uvWidthSize, uvy);
        glm::vec2 uvDownLeft  = glm::vec2(uvx              , uvy);

        UVs.push_back(uvUpLeft);
        UVs.push_back(uvDownLeft);
        UVs.push_back(uvUpRight);

        UVs.push_back(uvDownRight);
        UVs.push_back(uvUpRight);
        UVs.push_back(uvDownLeft);
    }

	glBindBuffer(GL_ARRAY_BUFFER, g_bufferVertex);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec2), &vertices[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, g_bufferUV);
	glBufferData(GL_ARRAY_BUFFER, UVs.size() * sizeof(glm::vec2), &UVs[0], GL_STATIC_DRAW);

	// Bind shader
	glUseProgram(g_program);

	// Bind texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, g_texture);
	// Set our "myTextureSampler" sampler to user Texture Unit 0
	glUniform1i(g_locationTexture, 0);

	// 1rst attribute buffer : vertices
	glEnableVertexAttribArray(g_locationVertex);
	glBindBuffer(GL_ARRAY_BUFFER, g_bufferVertex);
	glVertexAttribPointer(g_locationVertex, 2, GL_FLOAT, GL_FALSE, 0, (void*)0 );

	// 2nd attribute buffer : UVs
	glEnableVertexAttribArray(g_locationUV);
	glBindBuffer(GL_ARRAY_BUFFER, g_bufferUV);
	glVertexAttribPointer(g_locationUV, 2, GL_FLOAT, GL_FALSE, 0, (void*)0 );

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Draw call
	glDrawArrays(GL_TRIANGLES, 0, vertices.size() );

	glDisable(GL_BLEND);

	glDisableVertexAttribArray(g_locationVertex);
	glDisableVertexAttribArray(g_locationUV);

}

void Text2DCleanup(){

	// Delete buffers
	glDeleteBuffers(1, &g_bufferVertex);
	glDeleteBuffers(1, &g_bufferUV);

	// Delete texture
	glDeleteTextures(1, &g_texture);

	// Delete shader
	glDeleteProgram(g_program);
}
