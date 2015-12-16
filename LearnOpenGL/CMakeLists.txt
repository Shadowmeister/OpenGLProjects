cmake_minimum_required (VERSION 2.8)
cmake_policy(VERSION 2.8)
project (LearnOpenGL)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug CACHE STRING "Choose the type of build (Debug or Release)" FORCE)
endif(NOT CMAKE_BUILD_TYPE)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/modules/")

if(WIN32)
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
endif(WIN32)

list(APPEND CMAKE_CXX_FLAGS "-std=c++11")

find_package(GLM REQUIRED)
message(STATUS "GLM included at ${GLM_INCLUDE_DIR}")
find_package(GLFW3 REQUIRED)
message(STATUS "GLFW3 included at ${GLFW3_INCLUDE_DIR}")
find_package(ASSIMP REQUIRED)
message(STATUS "ASSIMP included at ${ASSIMP_INCLUDE_DIR}")
find_package(SOIL REQUIRED)
message(STATUS "SOIL included at ${SOIL_INCLUDE_DIR}")
find_package(GLEW REQUIRED)
message(STATUS "GLEW included at ${GLEW_INCLUDE_DIR}")

include_directories(
  ${GLM_INCLUDE_DIR}
  ${GLFW3_INCLUDE_DIR}
  ${ASSIMP_INCLUDE_DIR}
  ${SOIL_INCLUDE_DIR}
  ${GLEW_INCLUDE_DIR}
)

if(WIN32)
  set(LIBS ${GLFW3_LIBRARIES} opengl32 ${GLEW_LIBRARIES} ${SOIL_LIBRARIES} ${ASSIMP_LIBRARIES})
elseif(UNIX)
  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall")
  find_package(OpenGL REQUIRED)
  add_definitions(${OPENGL_DEFINITIONS})
  find_package(X11 REQUIRED)
  # note that the order is inmportant for setting the libs
  # use pkg-config --libs $(pkg-config --print-requires --print-requires-private glfw3) in a terminal to confirm
  set(LIBS ${GLFW3_LIBRARY} X11 Xrandr Xinerama Xi Xxf86vm Xcursor GL dl pthread GLEW SOIL assimp)
elseif(APPLE)
  INCLUDE_DIRECTORIES(/System/Library/Frameworks)
  FIND_LIBRARY(COCOA_LIBRARY Cocoa)
  FIND_LIBRARY(OpenGL_LIBRARY OpenGL)
  FIND_LIBRARY(IOKit_LIBRARY IOKit)
  MARK_AS_ADVANCED(COCOA_LIBRARY OpenGL_LIBRARY)
  SET(APPLE_LIBS ${COCOA_LIBRARY} ${IOKit_LIBRARY} ${OpenGL_LIBRARY})
  SET(APPLE_LIBS ${APPLE_LIBS} /usr/local/lib/libglfw.a)
  set(LIBS ${LIBS} ${APPLE_LIBS})
else()
  set(LIBS )
endif(WIN32)

configure_file(configuration/root_directory.h.in configuration/root_directory.h)
include_directories(${CMAKE_BINARY_DIR}/configuration)

file(GLOB SOURCE
  "include/*.h"
  "*.cpp"
)
add_executable(LearnOpenGL ${SOURCE})
target_link_libraries(LearnOpenGL ${LIBS})
	if(WIN32)
	  set_target_properties(LearnOpenGL PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/LearnOpenGL")
	elseif(UNIX)
	  set_target_properties(LearnOpenGL PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR/bin/LearnOpenGL")
	endif(WIN32)
	
# Copy shader files to build directory
file(GLOB SHADERS
  "Shaders/*.vs"
  "Shaders/*.frag"
  "Shaders/*.gs"
)
foreach(SHADER ${SHADERS})
  if(WIN32)
    add_custom_command(TARGET LearnOpenGL PRE_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${SHADER} $<TARGET_FILE_DIR:LearnOpenGL>)
  elseif(UNIX)
    file(COPY ${SHADER} DESTINATION ${CMAKE_CURRENT_BINARY_DIR/bin/LearnOpenGL)
  endif(WIN32)
endforeach(SHADER)

# if compiling for visual studio, also use configure file for each project (specifically to set up working directory)
if(MSVC)
  configure_file(${CMAKE_SOURCE_DIR}/configuration/visualstudio.vcxproj.user.in ${CMAKE_CURRENT_BINARY_DIR}/LearnOpenGL.vcxproj.user @ONLY)
endif(MSVC)

include_directories(${CMAKE_SOURCE_DIR}/include)