# Locate the FBX SDK (version 2013.1 only atm)
#
# Defines the following variables:
#
#    FBX_FOUND - Found the FBX SDK
#    FBX_VERSION - Version number
#    FBX_INCLUDE_DIRS - Include directories
#    FBX_LIBRARIES - The libraries to link to
#
# Accepts the following variables as input:
#
#    FBX_ROOT - (as a CMake or environment variable)
#               The root directory of the FBX SDK install

set(FBX_MAC_LOCATIONS
    /Applications/Autodesk/FBXSDK20131
)

if(WIN32)
    string(REGEX REPLACE "\\\\" "/" WIN_PROGRAM_FILES_X64_DIRECTORY $ENV{ProgramW6432})
endif()

set(FBX_WIN_LOCATIONS
    "${WIN_PROGRAM_FILES_X64_DIRECTORY}/Autodesk/FBX/FbxSdk/2013.1"
)

set(FBX_SEARCH_LOCATIONS
    $ENV{FBX_ROOT} ${FBX_ROOT} ${FBX_MAC_LOCATIONS} ${FBX_WIN_LOCATIONS}
)

set(FBX_VERSION 2013.1)

function(_fbx_append_debugs _endvar _library)
    if(${_library} AND ${_library}_DEBUG)
        set(_output optimized ${${_library}} debug ${${_library}_DEBUG})
    else()
        set(_output ${${_library}})
    endif()
    set(${_endvar} ${_output} PARENT_SCOPE)
endfunction()

function(_fbx_find_library _name)
    find_library(${_name}
        NAMES ${ARGN}
        HINTS ${FBX_SEARCH_LOCATIONS}
        PATH_SUFFIXES lib/gcc4/ub lib/vs2010/x64 lib/vs2008/x64
    )
    mark_as_advanced(${_name})
endfunction()

find_path(FBX_INCLUDE_DIR fbxsdk.h
    PATHS ${FBX_SEARCH_LOCATIONS}
    PATH_SUFFIXES include
)
mark_as_advanced(FBX_INCLUDE_DIR)

if(WIN32)
    _fbx_find_library(FBX_LIBRARY            fbxsdk-${FBX_VERSION}-md)
    _fbx_find_library(FBX_LIBRARY_DEBUG      fbxsdk-${FBX_VERSION}-mdd)
elseif(APPLE)
    find_library(CARBON NAMES Carbon)
    find_library(SYSTEM_CONFIGURATION NAMES SystemConfiguration)
    _fbx_find_library(FBX_LIBRARY            fbxsdk-${FBX_VERSION}-static)
    _fbx_find_library(FBX_LIBRARY_DEBUG      fbxsdk-${FBX_VERSION}-staticd)
endif()

include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(FBX DEFAULT_MSG FBX_LIBRARY FBX_INCLUDE_DIR)

if(FBX_FOUND)
    set(FBX_INCLUDE_DIRS ${FBX_INCLUDE_DIR})
    _fbx_append_debugs(FBX_LIBRARIES      FBX_LIBRARY)
    add_definitions (-DFBXSDK_NEW_API)
    if(WIN32)
        add_definitions(-DK_PLUGIN -DK_FBXSDK -DK_NODLL)
        set(CMAKE_EXE_LINKER_FLAGS /NODEFAULTLIB:\"LIBCMT\")
        set(FBX_LIBRARIES ${FBX_LIBRARIES} Wininet.lib)
    elseif(APPLE)
        set(FBX_LIBRARIES ${FBX_LIBRARIES} ${CARBON} ${SYSTEM_CONFIGURATION})
    endif()
endif()
