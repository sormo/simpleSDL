#pragma once

#if defined(ANDROID)
#define GL_GLES_PROTOTYPES 1
#define GL_GLEXT_PROTOTYPES 1
#include <SDL_opengles2.h>
#undef GL_GLES_PROTOTYPES
#undef GL_GLEXT_PROTOTYPES

#define glGenVertexArrays glGenVertexArraysOES
#define glBindVertexArray glBindVertexArrayOES
#define glDeleteVertexArrays glDeleteVertexArraysOES

#define GL_BGR                            0x80E0

#else
//#ifdef EMSCRIPTEN
//#include <gl\glew.h>
//#else
//#include "glew\glew.h"
//#endif

#ifndef EMSCRIPTEN
#include <Windows.h>
#endif
#include <gl\GL.h>
#include <SDL_opengl_glext.h>

extern PFNGLCREATESHADERPROC glCreateShader;
extern PFNGLSHADERSOURCEPROC glShaderSource;
extern PFNGLCOMPILESHADERPROC glCompileShader;
extern PFNGLGETSHADERIVPROC glGetShaderiv;
extern PFNGLGETSHADERINFOLOGPROC glGetShaderInfoLog;
extern PFNGLDELETESHADERPROC glDeleteShader;
extern PFNGLATTACHSHADERPROC glAttachShader;
extern PFNGLCREATEPROGRAMPROC glCreateProgram;
extern PFNGLLINKPROGRAMPROC glLinkProgram;
extern PFNGLVALIDATEPROGRAMPROC glValidateProgram;
extern PFNGLGETPROGRAMIVPROC glGetProgramiv;
extern PFNGLGETPROGRAMINFOLOGPROC glGetProgramInfoLog;
extern PFNGLUSEPROGRAMPROC glUseProgram;
extern PFNGLDELETEPROGRAMPROC glDeleteProgram;
extern PFNGLGENVERTEXARRAYSPROC glGenVertexArrays;
extern PFNGLBINDVERTEXARRAYPROC glBindVertexArray;
extern PFNGLDRAWARRAYSEXTPROC glDrawArraysEXT;
extern PFNGLDELETEVERTEXARRAYSPROC glDeleteVertexArrays;
extern PFNGLVERTEXATTRIBPOINTERPROC glVertexAttribPointer;
extern PFNGLBINDATTRIBLOCATIONPROC glBindAttribLocation;
extern PFNGLENABLEVERTEXATTRIBARRAYPROC glEnableVertexAttribArray;
extern PFNGLGENBUFFERSPROC glGenBuffers;
extern PFNGLBINDBUFFERPROC glBindBuffer;
extern PFNGLDELETEBUFFERSPROC glDeleteBuffers;
extern PFNGLBUFFERDATAPROC glBufferData;
extern PFNGLGETATTRIBLOCATIONPROC glGetAttribLocation;
extern PFNGLDISABLEVERTEXATTRIBARRAYPROC glDisableVertexAttribArray;
extern PFNGLDETACHSHADERPROC glDetachShader;
extern PFNGLGETUNIFORMLOCATIONPROC glGetUniformLocation;
extern PFNGLUNIFORMMATRIX4FVPROC glUniformMatrix4fv;
extern PFNGLGENERATEMIPMAPPROC glGenerateMipmap;
// opengl 1.2 and 1.3 is defined for emscripten
// gl in windows SDK has only opengl 1.1
#ifndef EMSCRIPTEN
extern PFNGLCOMPRESSEDTEXIMAGE2DPROC glCompressedTexImage2D;
#endif

#endif

bool InitOpenGL();
void PrintOpenGlPointers();
