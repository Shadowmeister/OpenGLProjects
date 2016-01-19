# Locate the glfw3 library
#
# This module defines the following variables:
#
# GLFW3_LIBRARIES the name of the library;
# GLFW3_INCLUDE_DIR where to find glfw include files.
# GLFW3_FOUND true if both the GLFW3_LIBRARY and GLFW3_INCLUDE_DIR have been found.
#
# To help locate the library and include file, you can define a
# variable called GLFW3_ROOT which points to the root of the glfw library
# installation.
#
# default search dirs

set( _glfw3_HEADER_SEARCH_DIRS
"/usr/include"
"/usr/local/include"
"${CMAKE_SOURCE_DIR}/include"
"$ENV{PROGRAMFILES}/glfw/include"
"C:/vs_dev_lib/GLstuff/include")
set( _glfw3_LIB_SEARCH_DIRS
"/usr/lib"
"/usr/local/lib"
"${CMAKE_SOURCE_DIR}/lib"
"C:/vs_dev_lib/GLstuff/lib/Win32/Release"
"C:/vs_dev_lib/GLstuff/lib/Win64/Release")

# Check environment for root search directory
set( _glfw3_ENV_ROOT ENV${GLFW3_ROOT} )
if(NOT GLFW3_ROOT AND _glfw3_ENV_ROOT)
  set(GLFW3_ROOT ${_glfw3_ENV_ROOT} )
endif()

# Put user specified location at beginning of search
if( GLFW3_ROOT )
  list( INSERT _glfw3_HEADER_SEARCH_DIRS 0 "${GLFW3_ROOT}/include" )
  list( INSERT _glfw3_LIB_SEARCH_DIRS 0 "${GLFW3_ROOT}/lib")
endif()

# Search for the header
FIND_PATH(GLFW3_INCLUDE_DIR "GLFW/glfw3.h"
  PATHS ${_glfw3_HEADER_SEARCH_DIRS} )
  
# Search for the library
FIND_LIBRARY(GLFW3_LIBRARY NAMES libglfw3 libglfw glfw3 glfw
  PATHS ${_glfw3_LIB_SEARCH_DIRS} )
  
IF(GLFW3_INCLUDE_DIR AND GLFW3_LIBRARY)
  SET(GLFW3_FOUND TRUE)
  SET(GLFW3_LIBRARIES ${GLFW3_LIBRARY})
ENDIF(GLFW3_INCLUDE_DIR AND GLFW3_LIBRARY)

IF(GLFW3_FOUND)
  IF(NOT GLFW3_FIND_QUIETLY)
    MESSAGE(STATUS "Found GLFW3: ${GLFW_LIBRARIES}") 
  ENDIF(NOT GLFW3_FIND_QUIETLY)
ELSE(GLFW3_FOUND)
  IF(GLFW3_FIND_REQUIRED)
    MESSAGE(FATAL_ERROR "Could not find GLFW3") 
  ENDIF(GLFW3_FIND_REQUIRED)
ENDIF(GLFW3_FOUND)

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(GLFW3 DEFAULT_MSG
  GLFW3_LIBRARY GLFW3_INCLUDE_DIR)
