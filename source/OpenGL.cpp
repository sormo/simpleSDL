#include "OpenGL.h"
#include <SDL.h>
#include "Common.h"
#include <cstdio>

#ifndef ANDROID
PFNGLCREATESHADERPROC glCreateShader;
PFNGLSHADERSOURCEPROC glShaderSource;
PFNGLCOMPILESHADERPROC glCompileShader;
PFNGLGETSHADERIVPROC glGetShaderiv;
PFNGLGETSHADERINFOLOGPROC glGetShaderInfoLog;
PFNGLDELETESHADERPROC glDeleteShader;
PFNGLATTACHSHADERPROC glAttachShader;
PFNGLCREATEPROGRAMPROC glCreateProgram;
PFNGLLINKPROGRAMPROC glLinkProgram;
PFNGLVALIDATEPROGRAMPROC glValidateProgram;
PFNGLGETPROGRAMIVPROC glGetProgramiv;
PFNGLGETPROGRAMINFOLOGPROC glGetProgramInfoLog;
PFNGLUSEPROGRAMPROC glUseProgram;
PFNGLDELETEPROGRAMPROC glDeleteProgram;
PFNGLGENVERTEXARRAYSPROC glGenVertexArrays;
PFNGLBINDVERTEXARRAYPROC glBindVertexArray;
PFNGLDRAWARRAYSEXTPROC glDrawArraysEXT;
PFNGLDELETEVERTEXARRAYSPROC glDeleteVertexArrays;
PFNGLVERTEXATTRIBPOINTERPROC glVertexAttribPointer;
PFNGLBINDATTRIBLOCATIONPROC glBindAttribLocation;
PFNGLENABLEVERTEXATTRIBARRAYPROC glEnableVertexAttribArray;
PFNGLBINDBUFFERPROC glBindBuffer;
PFNGLGENBUFFERSPROC glGenBuffers;
PFNGLDELETEBUFFERSPROC glDeleteBuffers;
PFNGLBUFFERDATAPROC glBufferData;
PFNGLGETATTRIBLOCATIONPROC glGetAttribLocation;
PFNGLDISABLEVERTEXATTRIBARRAYPROC glDisableVertexAttribArray;
PFNGLDETACHSHADERPROC glDetachShader;
PFNGLGETUNIFORMLOCATIONPROC glGetUniformLocation;
PFNGLUNIFORMMATRIX4FVPROC glUniformMatrix4fv;
PFNGLGENERATEMIPMAPPROC glGenerateMipmap;
#ifndef EMSCRIPTEN
PFNGLCOMPRESSEDTEXIMAGE2DPROC glCompressedTexImage2D;
#endif

bool InitOpenGL()
{
    //return glewInit() == GLEW_OK;
    glCreateShader = (PFNGLCREATESHADERPROC)SDL_GL_GetProcAddress("glCreateShader");
    glShaderSource = (PFNGLSHADERSOURCEPROC)SDL_GL_GetProcAddress("glShaderSource");
    glCompileShader = (PFNGLCOMPILESHADERPROC)SDL_GL_GetProcAddress("glCompileShader");
    glGetShaderiv = (PFNGLGETSHADERIVPROC)SDL_GL_GetProcAddress("glGetShaderiv");
    glGetShaderInfoLog = (PFNGLGETSHADERINFOLOGPROC)SDL_GL_GetProcAddress("glGetShaderInfoLog");
    glDeleteShader = (PFNGLDELETESHADERPROC)SDL_GL_GetProcAddress("glDeleteShader");
    glAttachShader = (PFNGLATTACHSHADERPROC)SDL_GL_GetProcAddress("glAttachShader");
    glCreateProgram = (PFNGLCREATEPROGRAMPROC)SDL_GL_GetProcAddress("glCreateProgram");
    glLinkProgram = (PFNGLLINKPROGRAMPROC)SDL_GL_GetProcAddress("glLinkProgram");
    glValidateProgram = (PFNGLVALIDATEPROGRAMPROC)SDL_GL_GetProcAddress("glValidateProgram");
    glGetProgramiv = (PFNGLGETPROGRAMIVPROC)SDL_GL_GetProcAddress("glGetProgramiv");
    glGetProgramInfoLog = (PFNGLGETPROGRAMINFOLOGPROC)SDL_GL_GetProcAddress("glGetProgramInfoLog");
    glUseProgram = (PFNGLUSEPROGRAMPROC)SDL_GL_GetProcAddress("glUseProgram");
    glDeleteProgram = (PFNGLDELETEPROGRAMPROC)SDL_GL_GetProcAddress("glDeleteProgram");
    glGenVertexArrays = (PFNGLGENVERTEXARRAYSPROC)SDL_GL_GetProcAddress("glGenVertexArrays");
    glBindVertexArray = (PFNGLBINDVERTEXARRAYPROC)SDL_GL_GetProcAddress("glBindVertexArray");
    glDrawArraysEXT = (PFNGLDRAWARRAYSEXTPROC)SDL_GL_GetProcAddress("glDrawArrays");
    glDeleteVertexArrays = (PFNGLDELETEVERTEXARRAYSPROC)SDL_GL_GetProcAddress("glDeleteVertexArrays");
    glVertexAttribPointer = (PFNGLVERTEXATTRIBPOINTERPROC)SDL_GL_GetProcAddress("glVertexAttribPointer");
    glBindAttribLocation = (PFNGLBINDATTRIBLOCATIONPROC)SDL_GL_GetProcAddress("glBindAttribLocation");
    glEnableVertexAttribArray = (PFNGLENABLEVERTEXATTRIBARRAYPROC)SDL_GL_GetProcAddress("glEnableVertexAttribArray");
    glBindBuffer = (PFNGLBINDBUFFERPROC)SDL_GL_GetProcAddress("glBindBuffer");
    glGenBuffers = (PFNGLGENBUFFERSPROC)SDL_GL_GetProcAddress("glGenBuffers");
    glDeleteBuffers = (PFNGLDELETEBUFFERSPROC)SDL_GL_GetProcAddress("glDeleteBuffers");
    glBufferData = (PFNGLBUFFERDATAPROC)SDL_GL_GetProcAddress("glBufferData");
    glGetAttribLocation = (PFNGLGETATTRIBLOCATIONPROC)SDL_GL_GetProcAddress("glGetAttribLocation");
    glDisableVertexAttribArray = (PFNGLDISABLEVERTEXATTRIBARRAYPROC)SDL_GL_GetProcAddress("glDisableVertexAttribArray");
    glDetachShader = (PFNGLDETACHSHADERPROC)SDL_GL_GetProcAddress("glDetachShader");
    glGetUniformLocation = (PFNGLGETUNIFORMLOCATIONPROC)SDL_GL_GetProcAddress("glGetUniformLocation");
    glUniformMatrix4fv = (PFNGLUNIFORMMATRIX4FVPROC)SDL_GL_GetProcAddress("glUniformMatrix4fv");
    glGenerateMipmap = (PFNGLGENERATEMIPMAPPROC)SDL_GL_GetProcAddress("glGenerateMipmap");
#ifndef EMSCRIPTEN
    glCompressedTexImage2D = (PFNGLCOMPRESSEDTEXIMAGE2DPROC)SDL_GL_GetProcAddress("glCompressedTexImage2D");
#endif
	return glCreateShader && glShaderSource && glCompileShader && glGetShaderiv &&
        glGetShaderInfoLog && glDeleteShader && glAttachShader && glCreateProgram &&
        glLinkProgram && glValidateProgram && glGetProgramiv && glGetProgramInfoLog &&
        glUseProgram && glGenVertexArrays && glBindVertexArray && glDrawArraysEXT &&
        glDeleteVertexArrays && glDeleteProgram && glVertexAttribPointer &&
        glBindAttribLocation && glEnableVertexAttribArray && glBindBuffer &&
        glGenBuffers && glDeleteBuffers && glBufferData &&
        glGetAttribLocation && glDisableVertexAttribArray && glDetachShader &&
#ifndef EMSCRIPTEN
        glCompressedTexImage2D &&
#endif
        glGetUniformLocation && glUniformMatrix4fv && glGenerateMipmap;
}

void PrintOpenGlPointers()
{
    printf("glCreateShader %p\nglShaderSource %p\nglCompileShader %p\nglGetShaderiv %p\n"
        "glGetShaderInfoLog %p\nglDeleteShader %p\nglAttachShader %p\nglCreateProgram %p\n"
        "glLinkProgram %p\nglValidateProgram %p\nglGetProgramiv %p\nglGetProgramInfoLog %p\n"
        "glUseProgram %p\nglGenVertexArrays %p\nglBindVertexArray %p\nglDrawArraysEXT %p\n"
        "glDeleteVertexArrays %p\nglDeleteProgram %p\nglVertexAttribPointer %p\n"
        "glBindAttribLocation %p\nglEnableVertexAttribArray %p\nglBindBuffer %p\n"
        "glGenBuffers %p\nglDeleteBuffers %p\nglBufferData %p\n"
        "glGetAttribLocation %p\nglDisableVertexAttribArray %p\nglDetachShader %p\n"
#ifndef EMSCRIPTEN
        "glCompressedTexImage2D %p\n"
#endif
        "glGetUniformLocation %p\nglUniformMatrix4fv %p\nglGenerateMipmap %p\n",
        glCreateShader, glShaderSource, glCompileShader, glGetShaderiv,
        glGetShaderInfoLog, glDeleteShader, glAttachShader, glCreateProgram,
        glLinkProgram, glValidateProgram, glGetProgramiv, glGetProgramInfoLog,
        glUseProgram, glGenVertexArrays, glBindVertexArray, glDrawArraysEXT,
        glDeleteVertexArrays, glDeleteProgram, glVertexAttribPointer,
        glBindAttribLocation, glEnableVertexAttribArray, glBindBuffer,
        glGenBuffers, glDeleteBuffers, glBufferData,
        glGetAttribLocation, glDisableVertexAttribArray, glDetachShader,
#ifndef EMSCRIPTEN
        glCompressedTexImage2D,
#endif
        glGetUniformLocation, glUniformMatrix4fv, glGenerateMipmap);
}
#else
bool InitOpenGL()
{
    return true;
}

void PrintOpenGlPointers()
{

}
#endif
