cmake_minimum_required(VERSION 3.6.0 FATAL_ERROR)
project(ModelConvert C CXX)

#
# Set some helper variables.
#
string(TOLOWER "${CMAKE_SYSTEM_NAME}" targetSystem)
set(projectDir     "${CMAKE_CURRENT_LIST_DIR}")
set(sourceDir      "${projectDir}/source")
set(sourceCommonDir "${projectDir}/../sourceCommon")
set(targetName     "ModelConvert")
set(binDir         "${projectDir}/bin")
set(assimpBuildDir "${projectDir}/build/assimp")

# Define executable output dir.
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${binDir}/${targetSystem}")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE "${binDir}/${targetSystem}")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL "${binDir}/${targetSystem}")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG "${binDir}/${targetSystem}_debug")

set(assimpDir "${projectDir}/../contrib/assimp")
add_subdirectory(${assimpDir} ${assimpBuildDir})
set(ASSIMP_BUILD_STATIC_LIB ON)
set(ASSIMP_BUILD_TESTS OFF)
set(ASSIMP_BUILD_ASSIMP_TOOLS OFF)
set(ASSIMP_BUILD_ALL_IMPORTERS_BY_DEFAULT OFF)
set(ASSIMP_LIBRARY assimp)

set(flatbuffersDir "${projectDir}/../contrib/flatbuffers")

include_directories("${sourceDir}"
	"${assimpDir}/include"
	"${projectDir}/include"
	"${flatbuffersDir}/include")

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

#
# GLM include directory
#
set(includeForGlm "${sourceCommonDir}")

# Include dirs.
set(projectIncludeDirs ${projectIncludeDirs}
  "${includeForGlm}"
)

message("Sources: ${projectSources}")
message("Include dirs: ${projectIncludeDirs}\n")

if(MSVC)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++17")
endif(MSVC)

#
# Build the binary.
# -----------------------------------------------------------------------
#

  add_executable(${targetName} ${projectSources})

target_link_libraries(${targetName}
  ${ASSIMP_LIBRARY}
)

target_include_directories(${targetName}
  PUBLIC ${projectIncludeDirs}
)

message("CMAKE_SOURCE_DIR ${CMAKE_SOURCE_DIR}")

set_target_properties(ModelConvert PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/data")
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
