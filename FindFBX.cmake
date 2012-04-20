# Locate the FBX SDK
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
        HINTS
            $ENV{FBX_ROOT}/lib/gcc4/ub
            ${FBX_ROOT}/lib/gcc4/ub
        # PATH_SUFFIXES ${_gtest_libpath_suffixes}
    )
    mark_as_advanced(${_name})
endfunction()

find_library(CARBON NAMES Carbon)
find_library(SYSTEM_CONFIGURATION NAMES SystemConfiguration)

find_path(FBX_INCLUDE_DIR fbxsdk.h
    HINTS
        $ENV{FBX_ROOT}/include
        ${FBX_ROOT}/include
)
mark_as_advanced(GTEST_INCLUDE_DIR)

_fbx_find_library(FBX_LIBRARY            fbxsdk-${FBX_VERSION}-static)
_fbx_find_library(FBX_LIBRARY_DEBUG      fbxsdk-${FBX_VERSION}-staticd)

include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(FBX DEFAULT_MSG FBX_LIBRARY FBX_INCLUDE_DIR)

if(FBX_FOUND)
    set(FBX_INCLUDE_DIRS ${FBX_INCLUDE_DIR})
    _fbx_append_debugs(FBX_LIBRARIES      FBX_LIBRARY)
    set(FBX_LIBRARIES ${FBX_LIBRARIES} ${CARBON} ${SYSTEM_CONFIGURATION})
    add_definitions (-DFBXSDK_NEW_API)
endif()
