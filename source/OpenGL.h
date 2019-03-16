#pragma once

#if defined(ANDROID)
#define GL_GLES_PROTOTYPES 1
#define GL_GLEXT_PROTOTYPES 1
#include <SDL_opengles.h>
#include <SDL_opengles2.h>
#undef GL_GLES_PROTOTYPES
#undef GL_GLEXT_PROTOTYPES

// extension for VAO on opengl ES
#define glGenVertexArrays glGenVertexArraysOES
#define glBindVertexArray glBindVertexArrayOES
#define glDeleteVertexArrays glDeleteVertexArraysOES

// this will not work but at least compile it
#ifndef GL_GEOMETRY_SHADER
#define GL_GEOMETRY_SHADER 0x8DD9
#endif

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
extern PFNGLUNIFORM1IPROC glUniform1i;
extern PFNGLUNIFORM3FPROC glUniform3f;
extern PFNGLUNIFORMMATRIX3FVPROC glUniformMatrix3fv;
extern PFNGLGETSTRINGIPROC glGetStringi;
extern PFNGLUNIFORM1FPROC glUniform1f;
extern PFNGLBUFFERSUBDATAPROC glBufferSubData;
extern PFNGLUNIFORM2FPROC glUniform2f;
extern PFNGLGETACTIVEUNIFORMPROC glGetActiveUniform;
extern PFNGLGETACTIVEATTRIBPROC glGetActiveAttrib;
extern PFNGLGENFRAMEBUFFERSPROC glGenFramebuffers;
extern PFNGLBINDFRAMEBUFFERPROC glBindFramebuffer;
extern PFNGLFRAMEBUFFERTEXTURE2DPROC glFramebufferTexture2D;
extern PFNGLGENRENDERBUFFERSPROC glGenRenderbuffers;
extern PFNGLBINDRENDERBUFFERPROC glBindRenderbuffer;
extern PFNGLRENDERBUFFERSTORAGEPROC glRenderbufferStorage;
extern PFNGLFRAMEBUFFERRENDERBUFFERPROC glFramebufferRenderbuffer;
extern PFNGLCHECKFRAMEBUFFERSTATUSPROC glCheckFramebufferStatus;
extern PFNGLDELETEFRAMEBUFFERSPROC glDeleteFramebuffers;
extern PFNGLUNIFORM1FVPROC glUniform1fv;
// TODO those functions are defined on opengl es 3.1
#if !defined(EMSCRIPTEN) && !defined(ANDROID)
extern PFNGLTEXIMAGE2DMULTISAMPLEPROC glTexImage2DMultisample;
extern PFNGLRENDERBUFFERSTORAGEMULTISAMPLEPROC glRenderbufferStorageMultisample;
extern PFNGLBLITFRAMEBUFFERPROC glBlitFramebuffer;
extern PFNGLFRAMEBUFFERTEXTUREPROC glFramebufferTexture;
#endif
// opengl 1.2 and 1.3 is defined for emscripten
// gl in windows SDK has only opengl 1.1
#ifndef EMSCRIPTEN
extern PFNGLCOMPRESSEDTEXIMAGE2DPROC glCompressedTexImage2D;
extern PFNGLACTIVETEXTUREPROC glActiveTexture;
#endif

#endif

#ifndef GL_CLAMP_TO_BORDER
#ifdef GL_NV_texture_border_clamp
#define GL_CLAMP_TO_BORDER GL_NV_texture_border_clamp
#elif GL_EXT_texture_border_clamp
#define GL_CLAMP_TO_BORDER GL_EXT_texture_border_clamp
#elif GL_OES_texture_border_clamp
#define GL_CLAMP_TO_BORDER GL_OES_texture_border_clamp
#else
#error "GL_CLAMP_TO_BORDER not defined"
#endif
#endif

#ifndef glReadBuffer
#ifdef glReadBufferNV
#define glReadBuffer glReadBufferNV
//#else
//#error "glReadBuffer not defined"
#endif
#endif

bool InitOpenGL();

void CheckGlError(const char * function, const char * file = __FILE__, const int line = __LINE__);

bool IsOpenGlExtensionSupported(const char * extension);
bool IsVAOSupported();

void PrintAllExtensions();
