cmake_minimum_required(VERSION 3.6.0 FATAL_ERROR)
project(simpleSDL C CXX)

#
# Set some helper variables.
#
string(TOLOWER "${CMAKE_SYSTEM_NAME}" targetSystem)
set(projectDir  "${CMAKE_CURRENT_LIST_DIR}")
set(sourceDir   "${projectDir}/source")
set(sourceCommonDir   "${projectDir}/sourceCommon")
set(targetName  "SimpleExample")
set(binDir      "${projectDir}/bin")

# Define executable output dir.
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${binDir}/${targetSystem}")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE "${binDir}/${targetSystem}")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL "${binDir}/${targetSystem}")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG "${binDir}/${targetSystem}_debug")

#
# Dependencies:
#   Using SDL as a subproject for simplicity.
#   NOTE: This means that CMAKE_C_FLAGS set anywhere here are also passed to SDL.
#
if(MINGW)
  set(VIDEO_OPENGLES OFF CACHE STRING "")
endif()

set(sdlDir "${projectDir}/contrib/SDL")
add_subdirectory(${sdlDir} EXCLUDE_FROM_ALL)
set(SDL2MAIN_LIBRARY SDL2main)
set(SDL2_LIBRARY SDL2)

#
# bullet
#
#set(bulletDir "${projectDir}/contrib/bullet3")
#add_subdirectory(${bulletDir} EXCLUDE_FROM_ALL)
#set(BULLET_LIBRARY bullet)

#
# Sources (relative to the project root dir).
#
file(GLOB_RECURSE projectSources RELATIVE ${projectDir}
  "${sourceDir}/*.h"
  "${sourceDir}/*.hpp"
  "${sourceDir}/*.cpp"
  "${sourceDir}/*.c"
)

file(GLOB_RECURSE projectCommonSources RELATIVE ${projectDir}
  "${sourceCommonDir}/*.h"
  "${sourceCommonDir}/*.hpp"
  "${sourceCommonDir}/*.cpp"
  "${sourceCommonDir}/*.c"
)

list(APPEND projectSources ${projectCommonSources})

# magic to have directories in visual studio
foreach(source IN LISTS projectSources)
    get_filename_component(sourcePath "${source}" PATH)
    string(REPLACE "/" "\\" sourcePathMsvc "${sourcePath}")
    source_group("${sourcePathMsvc}" FILES "${source}")
endforeach() 

set(flatbuffersDir "${projectDir}/contrib/flatbuffers")
set(modelConvertDir "${projectDir}/modelConvert")
#
# GLM include directory
#
set(includeForGlm "${sourceCommonDir}")

# Include dirs.
set(projectIncludeDirs ${projectIncludeDirs}
  "${sdlDir}/include"
  "${flatbuffersDir}/include"
  "${modelConvertDir}/include"
  "${includeForGlm}"
)

message("Sources: ${projectSources}")
message("Include dirs: ${projectIncludeDirs}\n")

#
# Platform dependent stuff.
#
if(NOT ANDROID)
  find_package(OpenGL REQUIRED)
endif()

if(MINGW)
  # -Link standard libs statically to reduce dll clutter.
  # -lmingw32 is needed to make WinMain not disappear when linking.
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -static -static-libgcc -lmingw32")

  # Only show the console in debug builds and also strip unused dependencies on MinSizeRel.
  set(CMAKE_EXE_LINKER_FLAGS_DEBUG "-mconsole")
  set(CMAKE_EXE_LINKER_FLAGS_RELEASE "-mwindows")
  set(CMAKE_EXE_LINKER_FLAGS_MINSIZEREL "-mwindows -s")
endif()

if(EMSCRIPTEN)
  # This makes emscripten build a html page in addition to the js code.
  set(CMAKE_EXECUTABLE_SUFFIX ".html")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -s FULL_ES2=1 \
                                          -s ERROR_ON_UNDEFINED_SYMBOLS=0 \
                                          -s \"EXTRA_EXPORTED_RUNTIME_METHODS=['Pointer_stringify']\" \
                                          --embed-file ${projectDir}/data@/ \
                                          --no-heap-copy \
                                          -s FORCE_FILESYSTEM=1 \
                                          -s ASSERTIONS=1 \
                                          -std=c++1z \
                                          -s ALLOW_MEMORY_GROWTH=1 \
                                          -s USE_WEBGL2=1")

  # Emscripten requires static linking.
  set(SDL2_LIBRARY SDL2-static)
endif()

if(MSVC)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++17")
endif(MSVC)

#
# Build the binary.
# -----------------------------------------------------------------------
#
if(ANDROID)
  # On android the final binary is a shared library not an executable.
  add_library(${targetName} SHARED ${projectSources})
else()
  add_executable(${targetName} ${projectSources})
endif()

target_link_libraries(${targetName}
  ${SDL2MAIN_LIBRARY}
  ${SDL2_LIBRARY}
  #${BULLET_LIBRARY}
  ${OPENGL_LIBRARY}
)

target_include_directories(${targetName}
  PUBLIC ${projectIncludeDirs}
)

message("CMAKE_SOURCE_DIR ${CMAKE_SOURCE_DIR}")

set_target_properties(SimpleExample PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/data")
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
