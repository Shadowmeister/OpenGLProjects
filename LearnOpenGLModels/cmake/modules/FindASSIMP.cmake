# - Try to find Assimp
# Once done, this will define
#
# ASSIMP_FOUND - system has Assimp
# ASSIMP_INCLUDE_DIR - the Assimp include directories
# ASSIMP_LIBRARIES - link these to use Assimp
# ASSIMP_DYNAMIC_LIB - for copying to executable location if needed

FIND_PATH( ASSIMP_INCLUDE_DIR assimp/mesh.h
  /usr/include
  /usr/local/include
  /opt/local/include
  ${CMAKE_SOURCE_DIR}/include
  C:/vs_dev_lib/GLstuff/include
)

FIND_PATH( ASSIMP_DYNAMIC_LIB assimp.dll assimp.a
  /usr/lib64
  /usr/lib
  /usr/local/lib
  /opt/local/lib
  ${CMAKE_SOURCE_DIR}/lib
  C:/vs_dev_lib/GLstuff/lib/Win32/Release
  C:/vs_dev_lib/GLstuff/lib/Win64/Release
)

FIND_LIBRARY( ASSIMP_LIBRARY assimp
  /usr/lib64
  /usr/lib
  /usr/local/lib
  /opt/local/lib
  ${CMAKE_SOURCE_DIR}/lib
  C:/vs_dev_lib/GLstuff/lib/Win32/Release
  C:/vs_dev_lib/GLstuff/lib/Win64/Release
)

IF(ASSIMP_INCLUDE_DIR AND ASSIMP_LIBRARY)
  SET( ASSIMP_FOUND TRUE )
  SET( ASSIMP_LIBRARIES ${ASSIMP_LIBRARY} )
ENDIF(ASSIMP_INCLUDE_DIR AND ASSIMP_LIBRARY)
IF(ASSIMP_FOUND)
  IF(NOT ASSIMP_FIND_QUIETLY)
    MESSAGE(STATUS "Found ASSIMP: ${ASSIMP_LIBRARY}")
  ENDIF(NOT ASSIMP_FIND_QUIETLY)
  IF(ASSIMP_DYNAMIC_LIB)
	IF(WIN32)
	  set(ASSIMP_DYNAMIC_LIB "${ASSIMP_DYNAMIC_LIB}/assimp.dll")
	ELSE(WIN32)
	  set(ASSIMP_DYNAMIC_LIB "${ASSIMP_DYNAMIC_LIB}/assimp.a")
	ENDIF(WIN32)
	MESSAGE(STATUS "Found Dynamic ASSIMP: ${ASSIMP_DYNAMIC_LIB}")
  ENDIF(ASSIMP_DYNAMIC_LIB)
ELSE(ASSIMP_FOUND)
  IF(ASSIMP_FIND_REQUIRED)
    MESSAGE(FATAL_ERROR "Could not find libASSIMP")
  ENDIF(ASSIMP_FIND_REQUIRED)
ENDIF(ASSIMP_FOUND)