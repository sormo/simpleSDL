#include "DebugDraw.h"
#include "Shader.h"
#include "Common.h"
#define DEBUG_DRAW_IMPLEMENTATION
#include "debug_draw.hpp"

// dummy definitions
// just to make it complilable
#ifndef GL_R8
#define GL_R8 0x8229
#endif
#ifndef GL_RED
#define GL_RED 0x1903
#endif
#ifndef GL_PROGRAM_POINT_SIZE
#define GL_PROGRAM_POINT_SIZE 0x8642
#endif

inline void DebugDraw::drawPointList(const dd::DrawVertex * points, int count, bool depthEnabled)
{
    assert(points != nullptr);
    assert(count > 0 && count <= DEBUG_DRAW_VERTEX_BUFFER_SIZE);

    glBindVertexArray(linePointVAO);
    glUseProgram(linePointProgram);

    glUniformMatrix4fv(linePointProgram_MvpMatrixLocation,
        1, GL_FALSE, &mvpMatrix[0][0]);

    if (depthEnabled)
    {
        glEnable(GL_DEPTH_TEST);
    }
    else
    {
        glDisable(GL_DEPTH_TEST);
    }

    // NOTE: Could also use glBufferData to take advantage of the buffer orphaning trick...
    glBindBuffer(GL_ARRAY_BUFFER, linePointVBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, count * sizeof(dd::DrawVertex), points);

    // Issue the draw call:
    glDrawArrays(GL_POINTS, 0, count);

    glUseProgram(0);
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    CheckGlError("drawPointList");
}

inline void DebugDraw::drawLineList(const dd::DrawVertex * lines, int count, bool depthEnabled)
{
    assert(lines != nullptr);
    assert(count > 0 && count <= DEBUG_DRAW_VERTEX_BUFFER_SIZE);

    glBindVertexArray(linePointVAO);
    glUseProgram(linePointProgram);

    glUniformMatrix4fv(linePointProgram_MvpMatrixLocation,
        1, GL_FALSE, &mvpMatrix[0][0]);

    if (depthEnabled)
    {
        glEnable(GL_DEPTH_TEST);
    }
    else
    {
        glDisable(GL_DEPTH_TEST);
    }

    // NOTE: Could also use glBufferData to take advantage of the buffer orphaning trick...
    glBindBuffer(GL_ARRAY_BUFFER, linePointVBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, count * sizeof(dd::DrawVertex), lines);

    // Issue the draw call:
    glDrawArrays(GL_LINES, 0, count);

    glUseProgram(0);
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    CheckGlError("drawLineList");
}

inline void DebugDraw::drawGlyphList(const dd::DrawVertex * glyphs, int count, dd::GlyphTextureHandle glyphTex)
{
    assert(glyphs != nullptr);
    assert(count > 0 && count <= DEBUG_DRAW_VERTEX_BUFFER_SIZE);

    glBindVertexArray(textVAO);
    glUseProgram(textProgram);

    auto[width, height] = Common::GetWindowSize();;

    // These doesn't have to be reset every draw call, I'm just being lazy ;)
    glUniform1i(textProgram_GlyphTextureLocation, 0);
    glUniform2f(textProgram_ScreenDimensions,
        static_cast<GLfloat>(width),
        static_cast<GLfloat>(height));

    if (glyphTex != nullptr)
    {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, handleToGL(glyphTex));
    }

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_DEPTH_TEST);

    glBindBuffer(GL_ARRAY_BUFFER, textVBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, count * sizeof(dd::DrawVertex), glyphs);

    glDrawArrays(GL_TRIANGLES, 0, count); // Issue the draw call

    glDisable(GL_BLEND);
    glUseProgram(0);
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
    CheckGlError("drawGlyphList");
}

inline dd::GlyphTextureHandle DebugDraw::createGlyphTexture(int width, int height, const void * pixels)
{
    assert(width > 0 && height > 0);
    assert(pixels != nullptr);

    GLuint textureId = 0;
    glGenTextures(1, &textureId);
    glBindTexture(GL_TEXTURE_2D, textureId);

    glPixelStorei(GL_PACK_ALIGNMENT, 1);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, width, height, 0, GL_RED, GL_UNSIGNED_BYTE, pixels);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    glBindTexture(GL_TEXTURE_2D, 0);
    CheckGlError("createGlyphTexture");

    return GLToHandle(textureId);
}

inline void DebugDraw::destroyGlyphTexture(dd::GlyphTextureHandle glyphTex)
{
    if (glyphTex == nullptr)
    {
        return;
    }

    const GLuint textureId = handleToGL(glyphTex);
    glBindTexture(GL_TEXTURE_2D, 0);
    glDeleteTextures(1, &textureId);
}

DebugDraw::DebugDraw()
    : mvpMatrix(glm::mat4(1.0f))
    , linePointProgram(0)
    , linePointProgram_MvpMatrixLocation(-1)
    , textProgram(0)
    , textProgram_GlyphTextureLocation(-1)
    , textProgram_ScreenDimensions(-1)
    , linePointVAO(0)
    , linePointVBO(0)
    , textVAO(0)
    , textVBO(0)
{
    printf("\n");
    printf("GL_VENDOR    : %s\n", glGetString(GL_VENDOR));
    printf("GL_RENDERER  : %s\n", glGetString(GL_RENDERER));
    printf("GL_VERSION   : %s\n", glGetString(GL_VERSION));
    printf("GLSL_VERSION : %s\n\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
    printf("DebugDraw initializing ...\n");

    // Default OpenGL states:
    //glEnable(GL_CULL_FACE);
    //glEnable(GL_DEPTH_TEST);
    //glDisable(GL_BLEND);

    // This has to be enabled since the point drawing shader will use gl_PointSize.
    glEnable(GL_PROGRAM_POINT_SIZE);

    setupShaderPrograms();
    setupVertexBuffers();

    printf("DebugDraw ready!\n\n");
}

DebugDraw::~DebugDraw()
{
    glDeleteProgram(linePointProgram);
    glDeleteProgram(textProgram);

    glDeleteVertexArrays(1, &linePointVAO);
    glDeleteBuffers(1, &linePointVBO);

    glDeleteVertexArrays(1, &textVAO);
    glDeleteBuffers(1, &textVBO);
}

inline void DebugDraw::setupShaderPrograms()
{
    printf("> DDRenderInterfaceCoreGL::setupShaderPrograms()\n");

    //
    // Line/point drawing shader:
    //
    {
        auto programOpt = CreateAndLinkProgram(linePointVertShaderSrc, linePointFragShaderSrc,
            [](GLuint program)
        {
            glBindAttribLocation(program, 0, "in_Position");
            glBindAttribLocation(program, 1, "in_ColorPointSize");
        });
        assert(programOpt);
        linePointProgram = *programOpt;

        linePointProgram_MvpMatrixLocation = glGetUniformLocation(linePointProgram, "u_MvpMatrix");
        if (linePointProgram_MvpMatrixLocation < 0)
        {
            printf("Unable to get u_MvpMatrix uniform location!");
        }
        CheckGlError("glGetUniformLocation");
    }

    //
    // Text rendering shader:
    //
    {
        auto programOpt = CreateAndLinkProgram(textVertShaderSrc, textFragShaderSrc,
            [](GLuint program)
        {
            glBindAttribLocation(program, 0, "in_Position");
            glBindAttribLocation(program, 1, "in_TexCoords");
            glBindAttribLocation(program, 2, "in_Color");
        });
        assert(programOpt);
        textProgram = *programOpt;

        textProgram_GlyphTextureLocation = glGetUniformLocation(textProgram, "u_glyphTexture");
        if (textProgram_GlyphTextureLocation < 0)
        {
            printf("Unable to get u_glyphTexture uniform location!");
        }

        textProgram_ScreenDimensions = glGetUniformLocation(textProgram, "u_screenDimensions");
        if (textProgram_ScreenDimensions < 0)
        {
            printf("Unable to get u_screenDimensions uniform location!");
        }
        CheckGlError("glGetUniformLocation");
    }
}

inline void DebugDraw::setupVertexBuffers()
{
    printf("> DDRenderInterfaceCoreGL::setupVertexBuffers()\n");

    //
    // Lines/points vertex buffer:
    //
    {
        glGenVertexArrays(1, &linePointVAO);
        glGenBuffers(1, &linePointVBO);
        CheckGlError("glGenBuffers");

        glBindVertexArray(linePointVAO);
        glBindBuffer(GL_ARRAY_BUFFER, linePointVBO);

        // RenderInterface will never be called with a batch larger than
        // DEBUG_DRAW_VERTEX_BUFFER_SIZE vertexes, so we can allocate the same amount here.
        glBufferData(GL_ARRAY_BUFFER, DEBUG_DRAW_VERTEX_BUFFER_SIZE * sizeof(dd::DrawVertex), nullptr, GL_STREAM_DRAW);
        CheckGlError("glBufferData");

        // Set the vertex format expected by 3D points and lines:
        std::size_t offset = 0;

        glEnableVertexAttribArray(0); // in_Position (vec3)
        glVertexAttribPointer(
            /* index     = */ 0,
            /* size      = */ 3,
            /* type      = */ GL_FLOAT,
            /* normalize = */ GL_FALSE,
            /* stride    = */ sizeof(dd::DrawVertex),
            /* offset    = */ reinterpret_cast<void *>(offset));
        offset += sizeof(float) * 3;

        glEnableVertexAttribArray(1); // in_ColorPointSize (vec4)
        glVertexAttribPointer(
            /* index     = */ 1,
            /* size      = */ 4,
            /* type      = */ GL_FLOAT,
            /* normalize = */ GL_FALSE,
            /* stride    = */ sizeof(dd::DrawVertex),
            /* offset    = */ reinterpret_cast<void *>(offset));

        CheckGlError("glVertexAttribPointer");

        // VAOs can be a pain in the neck if left enabled...
        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    //
    // Text rendering vertex buffer:
    //
    {
        glGenVertexArrays(1, &textVAO);
        glGenBuffers(1, &textVBO);
        CheckGlError("glGenBuffers");

        glBindVertexArray(textVAO);
        glBindBuffer(GL_ARRAY_BUFFER, textVBO);

        // NOTE: A more optimized implementation might consider combining
        // both the lines/points and text buffers to save some memory!
        glBufferData(GL_ARRAY_BUFFER, DEBUG_DRAW_VERTEX_BUFFER_SIZE * sizeof(dd::DrawVertex), nullptr, GL_STREAM_DRAW);
        CheckGlError("glBufferData");

        // Set the vertex format expected by the 2D text:
        std::size_t offset = 0;

        glEnableVertexAttribArray(0); // in_Position (vec2)
        glVertexAttribPointer(
            /* index     = */ 0,
            /* size      = */ 2,
            /* type      = */ GL_FLOAT,
            /* normalize = */ GL_FALSE,
            /* stride    = */ sizeof(dd::DrawVertex),
            /* offset    = */ reinterpret_cast<void *>(offset));
        offset += sizeof(float) * 2;

        glEnableVertexAttribArray(1); // in_TexCoords (vec2)
        glVertexAttribPointer(
            /* index     = */ 1,
            /* size      = */ 2,
            /* type      = */ GL_FLOAT,
            /* normalize = */ GL_FALSE,
            /* stride    = */ sizeof(dd::DrawVertex),
            /* offset    = */ reinterpret_cast<void *>(offset));
        offset += sizeof(float) * 2;

        glEnableVertexAttribArray(2); // in_Color (vec4)
        glVertexAttribPointer(
            /* index     = */ 2,
            /* size      = */ 4,
            /* type      = */ GL_FLOAT,
            /* normalize = */ GL_FALSE,
            /* stride    = */ sizeof(dd::DrawVertex),
            /* offset    = */ reinterpret_cast<void *>(offset));

        CheckGlError("glVertexAttribPointer");

        // Ditto.
        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
}

inline GLuint DebugDraw::handleToGL(dd::GlyphTextureHandle handle)
{
    const std::size_t temp = reinterpret_cast<std::size_t>(handle);
    return static_cast<GLuint>(temp);
}

inline dd::GlyphTextureHandle DebugDraw::GLToHandle(const GLuint id)
{
    const std::size_t temp = static_cast<std::size_t>(id);
    return reinterpret_cast<dd::GlyphTextureHandle>(temp);
}

// ========================================================
// Minimal shaders we need for the debug primitives:
// ========================================================

const char * DebugDraw::linePointVertShaderSrc = "\n"
    "#version 150\n"
    "\n"
    "in vec3 in_Position;\n"
    "in vec4 in_ColorPointSize;\n"
    "\n"
    "out vec4 v_Color;\n"
    "uniform mat4 u_MvpMatrix;\n"
    "\n"
    "void main()\n"
    "{\n"
    "    gl_Position  = u_MvpMatrix * vec4(in_Position, 1.0);\n"
    "    gl_PointSize = in_ColorPointSize.w;\n"
    "    v_Color      = vec4(in_ColorPointSize.xyz, 1.0);\n"
    "}\n";

const char * DebugDraw::linePointFragShaderSrc = "\n"
    "#version 150\n"
    "\n"
    "in  vec4 v_Color;\n"
    "out vec4 out_FragColor;\n"
    "\n"
    "void main()\n"
    "{\n"
    "    out_FragColor = v_Color;\n"
    "}\n";

const char * DebugDraw::textVertShaderSrc = "\n"
    "#version 150\n"
    "\n"
    "in vec2 in_Position;\n"
    "in vec2 in_TexCoords;\n"
    "in vec3 in_Color;\n"
    "\n"
    "uniform vec2 u_screenDimensions;\n"
    "\n"
    "out vec2 v_TexCoords;\n"
    "out vec4 v_Color;\n"
    "\n"
    "void main()\n"
    "{\n"
    "    // Map to normalized clip coordinates:\n"
    "    float x = ((2.0 * (in_Position.x - 0.5)) / u_screenDimensions.x) - 1.0;\n"
    "    float y = 1.0 - ((2.0 * (in_Position.y - 0.5)) / u_screenDimensions.y);\n"
    "\n"
    "    gl_Position = vec4(x, y, 0.0, 1.0);\n"
    "    v_TexCoords = in_TexCoords;\n"
    "    v_Color     = vec4(in_Color, 1.0);\n"
    "}\n";

const char * DebugDraw::textFragShaderSrc = "\n"
    "#version 150\n"
    "\n"
    "in vec2 v_TexCoords;\n"
    "in vec4 v_Color;\n"
    "\n"
    "uniform sampler2D u_glyphTexture;\n"
    "out vec4 out_FragColor;\n"
    "\n"
    "void main()\n"
    "{\n"
    "    out_FragColor = v_Color;\n"
    "    out_FragColor.a = texture(u_glyphTexture, v_TexCoords).r;\n"
    "}\n";
