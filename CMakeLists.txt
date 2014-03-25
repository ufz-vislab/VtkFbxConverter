# General project setup
CMAKE_MINIMUM_REQUIRED (VERSION 2.8.3)
PROJECT (VtkFbxConverter)
SET(VTKFBXCONVERTER_MAJOR_VERSION 0)
SET(VTKFBXCONVERTER_MINOR_VERSION 1)
SET(VTKFBXCONVERTER_PATCH_VERSION 0)
SET(VTKFBXCONVERTER_VERSION
	${VTKFBXCONVERTER_MAJOR_VERSION}.${VTKFBXCONVERTER_MINOR_VERSION}.${VTKFBXCONVERTER_PATCH_VERSION})

IF(NOT WIN32)
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-ignored-qualifiers -Wno-unused-parameter -Wno-missing-field-initializers")
ENDIF()

# Find
LIST(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}")
FIND_PACKAGE(FBX REQUIRED)
IF(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
  FIND_PACKAGE(ParaView QUIET)
ENDIF()
IF(ParaView_FOUND)
  INCLUDE(${PARAVIEW_USE_FILE})
  OPTION(INSTALL_IN_PARAVIEW "Should the ParaView plugin be installed?" ON)
ELSE()
  FIND_PACKAGE(VTK)
  IF(OGS_BUILD_GUI AND VTK_VERSION VERSION_LESS 6.0.0)
    MESSAGE(FATAL_ERROR "VTK not found but is required! Minimum version: VTK 5.8.x")
  ENDIF()
  INCLUDE( ${VTK_USE_FILE} )
ENDIF()
FIND_PACKAGE(GTest)
IF(OGS_BUILD_GUI)
	ADD_DEFINITIONS(-DOGS_BUILD_GUI)
ENDIF()

# Offer the user the choice of overriding the installation directories
SET(INSTALL_LIB_DIR lib CACHE PATH "Installation directory for libraries")
SET(INSTALL_BIN_DIR bin CACHE PATH "Installation directory for executables")
SET(INSTALL_INCLUDE_DIR include CACHE PATH
  "Installation directory for header files")
IF(WIN32 AND NOT CYGWIN)
  SET(DEF_INSTALL_CMAKE_DIR CMake)
ELSE()
  SET(DEF_INSTALL_CMAKE_DIR lib/CMake/VtkFbxConverter)
ENDIF()
SET(INSTALL_CMAKE_DIR ${DEF_INSTALL_CMAKE_DIR} CACHE PATH
  "Installation directory for CMake files")

# Make relative paths absolute (needed later on)
FOREACH(p LIB BIN INCLUDE CMAKE)
  SET(var INSTALL_${p}_DIR)
  IF(NOT IS_ABSOLUTE "${${var}}")
    SET(${var} "${CMAKE_INSTALL_PREFIX}/${${var}}")
  ENDIF()
ENDFOREACH()

INCLUDE_DIRECTORIES( . )
INCLUDE_DIRECTORIES( SYSTEM ${FBX_INCLUDE_DIRS} )

# Library
ADD_LIBRARY(VtkFbxConverter
	Common.h
	Common.cxx
	VtkFbxHelper.h
	VtkFbxHelper.cxx
	VtkFbxConverter.h
	VtkFbxConverter.cxx
)

SET_TARGET_PROPERTIES(VtkFbxConverter PROPERTIES
  PUBLIC_HEADER "VtkFbxConverter.h")

IF(INSTALL_IN_PARAVIEW OR NOT CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
  # Do not install
ELSE()
  INSTALL(TARGETS VtkFbxConverter
    # IMPORTANT: Add the library to the "export-set"
    EXPORT VtkFbxConverterLibraryDepends
    RUNTIME DESTINATION "${INSTALL_BIN_DIR}" COMPONENT bin
    ARCHIVE DESTINATION "${INSTALL_LIB_DIR}" COMPONENT lib
    LIBRARY DESTINATION "${INSTALL_LIB_DIR}" COMPONENT shlib
    PUBLIC_HEADER DESTINATION "${INSTALL_INCLUDE_DIR}/VtkFbxConverter"
      COMPONENT dev)
ENDIF()

# Executable
ADD_EXECUTABLE(vtk_fbx_converter main.cxx)

SET( LINK_LIBRARIES ${FBX_LIBRARIES} ${VTK_LIBRARIES} )

TARGET_LINK_LIBRARIES (vtk_fbx_converter VtkFbxConverter ${LINK_LIBRARIES})

IF(INSTALL_IN_PARAVIEW OR NOT CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
  # Do not install
ELSE()
  INSTALL(TARGETS vtk_fbx_converter
    # IMPORTANT: Add the executable to the "export-set"
    EXPORT VtkFbxHelperLibraryDepends
    RUNTIME DESTINATION "${INSTALL_BIN_DIR}" COMPONENT bin)
ENDIF()

# Tests are only built when this is built standalone
IF (GTEST_FOUND AND CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
	ADD_SUBDIRECTORY (tests)
ENDIF () # GTEST_FOUND

# Add all targets to the build-tree export set
EXPORT(TARGETS VtkFbxConverter vtk_fbx_converter
  FILE "${PROJECT_BINARY_DIR}/VtkFbxConverterLibraryDepends.cmake")

# Export the package for use from the build-tree
# (this registers the build-tree with a global CMake-registry)
EXPORT(PACKAGE VtkFbxConverter)

# Create a VtkFbxConverterBuildTreeSettings.cmake file for the use from the build tree
CONFIGURE_FILE(VtkFbxConverterBuildTreeSettings.cmake.in
  "${PROJECT_BINARY_DIR}/VtkFbxConverterBuildTreeSettings.cmake" @ONLY)

# Create the VtkFbxConverterConfig.cmake and VtkFbxConverterConfigVersion files
FILE(RELATIVE_PATH CONF_REL_INCLUDE_DIR "${INSTALL_CMAKE_DIR}"
   "${INSTALL_INCLUDE_DIR}")
CONFIGURE_FILE(VtkFbxConverterConfig.cmake.in
  "${PROJECT_BINARY_DIR}/VtkFbxConverterConfig.cmake" @ONLY)
CONFIGURE_FILE(VtkFbxConverterConfigVersion.cmake.in
  "${PROJECT_BINARY_DIR}/VtkFbxConverterConfigVersion.cmake" @ONLY)

# Cache variables when this is a subproject
IF(NOT CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
	SET(VTKFBXCONVERTER_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR} ${FBX_INCLUDE_DIRS} CACHE STRING "")
	SET(VTKFBXCONVERTER_LIBRARIES VtkFbxConverter ${LINK_LIBRARIES} CACHE STRING "")
	SET(VTKFBXCONVERTER_FOUND TRUE CACHE BOOL "")
ENDIF()

IF(ParaView_FOUND)
  ADD_SUBDIRECTORY(pv_plugin)
ENDIF()
