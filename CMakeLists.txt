cmake_minimum_required(VERSION 3.6.0 FATAL_ERROR)
project(simpleSDL C CXX)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

#
# Set some helper variables.
#
string(TOLOWER "${CMAKE_SYSTEM_NAME}" targetSystem)
set(projectDir      "${CMAKE_CURRENT_LIST_DIR}")
set(sourceDir       "${projectDir}/source")
set(sourceCommonDir "${projectDir}/sourceCommon")
set(targetName      "SimpleExample")
set(binDir          "${projectDir}/bin")
set(flatbuffersDir  "${projectDir}/contrib/flatbuffers")
set(modelConvertDir "${projectDir}/modelConvert")

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
# TODO remove this ???
OPTION(BUILD_UNIT_TESTS "Build Unit Tests" ON)
OPTION(BUILD_BULLET2_DEMOS "Set when you want to build the Bullet 2 demos" ON)
OPTION(BUILD_OPENGL3_DEMOS "Set when you want to build Bullet 3 OpenGL3+ demos" ON)
OPTION(BUILD_EXTRAS "Set when you want to build the extras" ON)
OPTION(USE_MSVC_RUNTIME_LIBRARY_DLL "Use MSVC Runtime Library DLL (/MD or /MDd)" OFF)

set(BUILD_UNIT_TESTS OFF)
set(BUILD_BULLET2_DEMOS OFF)
set(BUILD_OPENGL3_DEMOS OFF)
set(BUILD_EXTRAS OFF)
set(BUILD_SHARED_LIBS OFF)
set(USE_MSVC_RUNTIME_LIBRARY_DLL ON)

set(bulletDir "${projectDir}/contrib/bullet3")
add_subdirectory(${bulletDir})
set(BULLET_DYNAMICS_LIBRARY BulletDynamics)
set(BULLET_COLLISION_LIBRARY BulletCollision)
set(BULLET_LINEAR_MATH_LIBRARY LinearMath)

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

# Include dirs.
set(projectIncludeDirs ${projectIncludeDirs}
  "${sdlDir}/include"
  "${flatbuffersDir}/include"
  "${modelConvertDir}/include"
  "${bulletDir}/src"
  "${sourceCommonDir}"
  "${sourceDir}"
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

#
# Filters in visual studio
# -----------------------------------------------------------------------
#
foreach(_source IN ITEMS ${projectSources})
    get_filename_component(_source_path "${_source}" PATH)
    string(REPLACE "${CMAKE_SOURCE_DIR}" "" _group_path "${_source_path}")
    string(REPLACE "/" "\\" _group_path "${_group_path}")
    source_group("${_group_path}" FILES "${_source}")
endforeach()
# -----------------------------------------------------------------------
#

target_link_libraries(${targetName}
  ${SDL2MAIN_LIBRARY}
  ${SDL2_LIBRARY}
  ${BULLET_DYNAMICS_LIBRARY}
  ${BULLET_COLLISION_LIBRARY}
  ${BULLET_LINEAR_MATH_LIBRARY}
  ${OPENGL_LIBRARY}
)

target_include_directories(${targetName}
  PUBLIC ${projectIncludeDirs}
)

set_target_properties(${SDL2MAIN_LIBRARY} PROPERTIES FOLDER "SDL")
set_target_properties(${SDL2_LIBRARY} PROPERTIES FOLDER "SDL")

set_target_properties(BulletDynamics PROPERTIES FOLDER "Bullet")
set_target_properties(BulletCollision PROPERTIES FOLDER "Bullet")
set_target_properties(BulletSoftBody PROPERTIES FOLDER "Bullet")
set_target_properties(Bullet2FileLoader PROPERTIES FOLDER "Bullet")
set_target_properties(Bullet3Collision PROPERTIES FOLDER "Bullet")
set_target_properties(Bullet3Common PROPERTIES FOLDER "Bullet")
set_target_properties(Bullet3Dynamics PROPERTIES FOLDER "Bullet")
set_target_properties(Bullet3Geometry PROPERTIES FOLDER "Bullet")
set_target_properties(Bullet3OpenCL_clew PROPERTIES FOLDER "Bullet")
set_target_properties(BulletInverseDynamics PROPERTIES FOLDER "Bullet")
set_target_properties(LinearMath PROPERTIES FOLDER "Bullet")

message("CMAKE_SOURCE_DIR ${CMAKE_SOURCE_DIR}")

set_target_properties(SimpleExample PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/data")
