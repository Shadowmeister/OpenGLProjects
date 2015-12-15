cmake_minimum_required (VERSION 2.8 FATAL_ERROR)
project (LearnOpenGL)

# Set up linux search paths
set(LIBRARY_SEARCH_PATH
  /usr/local
  /opt/local
  /usr
  /opt
)

set (HEADER_SEARCH_PATH
  /usr/local
  /opt/local
  /usr
  /opt
)

if(NOT CMAKE_BUILD_TYPE)
  message(STATUS "Build type defaulting to \"Release\"")
  set(CMAKE_BUILD_TYPE "Release")
endif()

if(CMAKE_COMPILER_IS_GNUCXX)
  set(CMAKE_C_FLAGS_DEBUG "-g -Wall -Wextra -DDEBUG -D_DEBUG")
  set(CMAKE_CXX_FLAGS_DEBUG "-g -Wall -Wextra -DDEBUG -D_DEBUG")
endif(CMAKE_COMPILER_IS_GNUCXX)

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")
# The version number.
set (LearnOpenGL_VERSION_MAJOR 0)
set (LearnOpenGL_VERSION_MINOR 1)

# configure a header file to pass some of the CMake settings
# to the source code
configure_file (
  "${PROJECT_SOURCE_DIR}/LearnOpenGLConfig.h.in"
  "${PROJECT_BINARY_DIR}/LearnOpenGLConfig.h"
  )

find_package(OpenGL)
find_package(PkgConfig REQUIRED)
pkg_search_module(GLFW REQUIRED glfw3)
if(GLFW_FOUND)
  message("FOUND GLFW")
endif()

# Set additional libs
set( ADDITIONAL_LIBS
      -pthread
      -lrt
      -lXinerama
      -lXrandr
      -lXxf86vm
      -lXi
)

find_package(GLEW REQUIRED)
if(GLEW_FOUND)
  message("Found GLEW")
  include_directories(${GLEW_INCLUDE_DIRS})
endif()

find_path(SOIL_INCLUDE_DIR SOIL/SOIL.h)
find_library(SOIL_LIBRARY SOIL)

#Libraries needed on all platforms for this project
set(LIBRARIES
   ${OPENGL_LIBRARIES}
   ${GLFW_STATIC_LIBRARIES}
   ${GLEW_LIBRARY}
   ${ADDITIONAL_LIBS}
   ${SOIL_LIBRARY}
)

include_directories("${PROJECT_BINARY_DIR}"
                    "${PROJECT_SOURCE_DIR}/include"
                    "${GLFW_INCLUDE_DIRS}"
                    "${SOIL_INCLUDE_DIR}")

set(LearnOpenGL_HEADER include/Shader.h
                       include/Camera.h
                       include/Object.h
   )

set(LearnOpenGL_SOURCE main.cpp
                       Object.cpp
                       Shader.cpp
                       Camera.cpp
   )

#add the executable
add_executable(LearnOpenGL 
               ${LearnOpenGL_SOURCE}
               ${LearnOpenGL_HEADER}
               )

target_link_libraries(LearnOpenGL glfw
                      ${LIBRARIES}
                      )
