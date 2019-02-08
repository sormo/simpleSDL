#pragma once
#include "debug_draw.hpp"
#include "OpenGL.h"
#include "glm/glm.hpp"

class DebugDraw final : public dd::RenderInterface
{
public:
    void drawPointList(const dd::DrawVertex * points, int count, bool depthEnabled) override;
    void drawLineList(const dd::DrawVertex * lines, int count, bool depthEnabled) override;
    void drawGlyphList(const dd::DrawVertex * glyphs, int count, dd::GlyphTextureHandle glyphTex) override;

    dd::GlyphTextureHandle createGlyphTexture(int width, int height, const void * pixels) override;

    void destroyGlyphTexture(dd::GlyphTextureHandle glyphTex) override;

    // These two can also be implemented to perform GL render
    // state setup/cleanup, but we don't use them in this sample.
    //void beginDraw() override { }
    //void endDraw()   override { }

    //
    // Local methods:
    //

    DebugDraw();
    ~DebugDraw();

    void setupShaderPrograms();
    void setupVertexBuffers();

    static GLuint handleToGL(dd::GlyphTextureHandle handle);
    static dd::GlyphTextureHandle GLToHandle(const GLuint id);

    // The "model-view-projection" matrix for the scene.
    // In this demo, it consists of the camera's view and projection matrices only.
    glm::mat4 mvpMatrix;

private:

    GLuint linePointProgram;
    GLint  linePointProgram_MvpMatrixLocation;

    GLuint textProgram;
    GLint  textProgram_GlyphTextureLocation;
    GLint  textProgram_ScreenDimensions;

    GLuint linePointVAO;
    GLuint linePointVBO;

    GLuint textVAO;
    GLuint textVBO;

    static const char * linePointVertShaderSrc;
    static const char * linePointFragShaderSrc;

    static const char * textVertShaderSrc;
    static const char * textFragShaderSrc;

};
