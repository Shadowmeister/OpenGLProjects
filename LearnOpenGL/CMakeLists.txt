cmake_minimum_required (VERSION 2.8 FATAL_ERROR)
project (LearnOpenGL)

if(NOT CMAKE_BUILD_TYPE)
  message(STATUS "Build type defaulting to \"Release\"")
  set(CMAKE_BUILD_TYPE "Release")
endif()

if(CMAKE_COMPILER_IS_GNUCXX)
  set(CMAKE_C_FLAGS_DEBUG "-g -Wall -Wextra -DDEBUG -D_DEBUG")
  set(CMAKE_CXX_FLAGS_DEBUG "-g -Wall -Wextra -DDEBUG -D_DEBUG")
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")
endif(CMAKE_COMPILER_IS_GNUCXX)

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

if(CMAKE_COMPILER_IS_GNUCXX)
	
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
	endif()
	
	find_path(SOIL_INCLUDE_DIR SOIL/SOIL.h)
	find_library(SOIL_LIBRARY SOIL)

elseif(MSVC)
	set(MSVC_LIBS_DIR 
		C:/vs_dev_lib/GLStuff/lib/Win32/Release
		C:/vs_dev_lib/GLStuff/lib/Win64/Release)
	set(MSVC_INCLUDE_DIR C:/vs_dev_lib/GLStuff/include)
	
	message("MSVC")
	message("Libs Dir ${MSVC_LIBS_DIR}")
	message("Include Dir ${MSVC_INCLUDE_DIR}")
	
	find_path(GLFW_INCLUDE_DIRS GLFW/glfw3.h ${MSVC_INCLUDE_DIR})
	if(GLFW_INCLUDE_DIRS)
		message("Found GLFW Includes")
	endif()
	find_library(GLFW_STATIC_LIBRARIES glfw3.lib ${MSVC_LIBS_DIR})
	if(GLFW_STATIC_LIBRARIES)
		message("Found GLFW Static Libraries")
	endif()
	
	find_path(GLEW_INCLUDE_DIRS GL/glew.h ${MSVC_INCLUDE_DIR})
	if(GLEW_INCLUDE_DIRS)
		message("GLEW Includes Found")
	endif()
	find_library(GLEW_LIBRARY libglew32.lib ${MSVC_LIBS_DIR})
	if(GLEW_LIBRARY)
		message("GLEW Library Found")
	endif()
	
	find_path(SOIL_INCLUDE_DIR SOIL/SOIL.h ${MSVC_INCLUDE_DIR})
	if(SOIL_INCLUDE_DIR)
		message("Found SOIL Includes")
	endif()
	find_library(SOIL_LIBRARY SOIL ${MSVC_LIBS_DIR}/Release)
	if(SOIL_LIBRARY)
		message("Found SOIL Library")
	endif()
endif(CMAKE_COMPILER_IS_GNUCXX)

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
					"${GLEW_INCLUDE_DIRS}"
                    "${SOIL_INCLUDE_DIR}")

set(LearnOpenGL_HEADER include/Shader.h
                       include/Camera.h
                       include/SimpleObject.h
		       include/Texture.h
   )

set(LearnOpenGL_SOURCE main.cpp
                       SimpleObject.cpp
                       Shader.cpp
                       Camera.cpp
		       Texture.cpp
   )

#add the executable
add_executable(LearnOpenGL 
               ${LearnOpenGL_SOURCE}
               ${LearnOpenGL_HEADER}
               )

target_link_libraries(LearnOpenGL
						${LIBRARIES}
                      )
					  
if(MSVC)
	add_custom_command(TARGET LearnOpenGL POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy_directory
			${CMAKE_SOURCE_DIR}/Shaders
			$<TARGET_FILE_DIR:LearnOpenGL>/../Shaders)
	add_custom_command(TARGET LearnOpenGL POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy_directory
			${CMAKE_SOURCE_DIR}/Textures
			$<TARGET_FILE_DIR:LearnOpenGL>/../Textures)
endif()
