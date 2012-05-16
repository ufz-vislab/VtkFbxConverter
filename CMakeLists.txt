# General project setup
CMAKE_MINIMUM_REQUIRED (VERSION 2.8.3)
PROJECT (VtkFbxConverter)
SET(VTKFBXCONVERTER_MAJOR_VERSION 0)
SET(VTKFBXCONVERTER_MINOR_VERSION 1)
SET(VTKFBXCONVERTER_PATCH_VERSION 0)
SET(VTKFBXCONVERTER_VERSION
	${VTKFBXCONVERTER_MAJOR_VERSION}.${VTKFBXCONVERTER_MINOR_VERSION}.${VTKFBXCONVERTER_PATCH_VERSION})

# Find
LIST(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}")
FIND_PACKAGE(FBX REQUIRED)
FIND_PACKAGE(Boost COMPONENTS filesystem system regex REQUIRED)
FIND_PACKAGE(VTK 5.8 REQUIRED)
FIND_PACKAGE(GTest)

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

INCLUDE( ${VTK_USE_FILE} )

INCLUDE_DIRECTORIES (
	.
	${FBX_INCLUDE_DIRS}
)

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

INSTALL(TARGETS VtkFbxConverter
  # IMPORTANT: Add the library to the "export-set"
  EXPORT VtkFbxConverterLibraryDepends
  RUNTIME DESTINATION "${INSTALL_BIN_DIR}" COMPONENT bin
  ARCHIVE DESTINATION "${INSTALL_LIB_DIR}" COMPONENT lib
  LIBRARY DESTINATION "${INSTALL_LIB_DIR}" COMPONENT shlib
  PUBLIC_HEADER DESTINATION "${INSTALL_INCLUDE_DIR}/VtkFbxConverter"
    COMPONENT dev)

# Executable
ADD_EXECUTABLE(vtk_fbx_converter main.cxx)

SET (LINK_LIBRARIES
	${FBX_LIBRARIES}
	${Boost_LIBRARIES}
	vtkRendering
	vtkIO
)

TARGET_LINK_LIBRARIES (vtk_fbx_converter VtkFbxConverter ${LINK_LIBRARIES})

INSTALL(TARGETS vtk_fbx_converter
  # IMPORTANT: Add the executable to the "export-set"
  EXPORT VtkFbxHelperLibraryDepends
  RUNTIME DESTINATION "${INSTALL_BIN_DIR}" COMPONENT bin)

# Tests
#IF (GTEST_FOUND)
#	ADD_SUBDIRECTORY (tests)
#ENDIF () # GTEST_FOUND

# BLUB
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
