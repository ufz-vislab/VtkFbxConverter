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
#    FBX_VERSION - as a CMake variable, e.g. 2014.1
#    FBX_ROOT - (as a CMake or environment variable)
#               The root directory of the FBX SDK install

if(NOT DEFINED FBX_VERSION)
    set(FBX_VERSION 2016.1.2)
endif()
string(REGEX REPLACE "^([0-9]+).*$" "\\1" FBX_VERSION_MAJOR "${FBX_VERSION}")
string(REGEX REPLACE "^[0-9]+\\.([0-9]+).*$" "\\1" FBX_VERSION_MINOR  "${FBX_VERSION}")
string(REGEX REPLACE "^[0-9]+\\.[0-9]+\\.([0-9]+).*$" "\\1" FBX_VERSION_PATCH "${FBX_VERSION}")

set(FBX_MAC_LOCATIONS
    "/Applications/Autodesk/FBX\ SDK/${FBX_VERSION}"
)

if(WIN32)
    string(REGEX REPLACE "\\\\" "/" WIN_PROGRAM_FILES_X64_DIRECTORY $ENV{ProgramW6432})
endif()

set(FBX_WIN_LOCATIONS
    "${WIN_PROGRAM_FILES_X64_DIRECTORY}/Autodesk/FBX/FBX SDK/${FBX_VERSION}"
)

set(FBX_SEARCH_LOCATIONS
    $ENV{FBX_ROOT} ${FBX_ROOT} ${FBX_MAC_LOCATIONS} ${FBX_WIN_LOCATIONS}
)

function(_fbx_append_debugs _endvar _library)
    if(${_library} AND ${_library}_DEBUG)
        set(_output optimized ${${_library}} debug ${${_library}_DEBUG})
    else()
        set(_output ${${_library}})
    endif()
    set(${_endvar} ${_output} PARENT_SCOPE)
endfunction()

if(${CMAKE_CXX_COMPILER_ID} MATCHES "Clang")
    set(fbx_compiler clang)
elseif(${CMAKE_CXX_COMPILER_ID} MATCHES "GNU")
    set(fbx_compiler gcc4)
endif()

function(_fbx_find_library _name _lib _suffix)
    if(MSVC_VERSION VERSION_EQUAL 1900)
        set(VS_PREFIX vs2015)
    elseif(MSVC_VERSION VERSION_GREATER_EQUAL 1910)
        set(VS_PREFIX vs2017)
    endif()
    find_library(${_name}
        NAMES ${_lib}
        HINTS ${FBX_SEARCH_LOCATIONS}
        PATH_SUFFIXES
            lib/${fbx_compiler}/release
            lib/${fbx_compiler}/ub/${_suffix}
            lib/${VS_PREFIX}/x64/${_suffix}
    )
    mark_as_advanced(${_name})
endfunction()

find_path(FBX_INCLUDE_DIR fbxsdk.h
    PATHS ${FBX_SEARCH_LOCATIONS}
    PATH_SUFFIXES include
)
mark_as_advanced(FBX_INCLUDE_DIR)

if(WIN32)
    _fbx_find_library(FBX_LIBRARY            libfbxsdk-md release)
    _fbx_find_library(FBX_LIBRARY_DEBUG      libfbxsdk-md debug)
    if(FBX_VERSION VERSION_GREATER_EQUAL 2019)
        _fbx_find_library(XML2_LIBRARY            libxml2-md release)
        _fbx_find_library(XML2_LIBRARY_DEBUG      libxml2-md debug)
        _fbx_find_library(ZLIB_LIBRARY            zlib-md release)
        _fbx_find_library(ZLIB_LIBRARY_DEBUG      zlib-md debug)
    endif()
elseif(APPLE)
    find_library(CARBON NAMES Carbon)
    find_library(SYSTEM_CONFIGURATION NAMES SystemConfiguration)
    _fbx_find_library(FBX_LIBRARY            libfbxsdk.a release)
    _fbx_find_library(FBX_LIBRARY_DEBUG      libfbxsdk.a debug)
endif()

include(FindPackageHandleStandardArgs)
if(WIN32 AND FBX_VERSION VERSION_GREATER_EQUAL 2019)
    FIND_PACKAGE_HANDLE_STANDARD_ARGS(FBX DEFAULT_MSG FBX_LIBRARY FBX_INCLUDE_DIR XML2_LIBRARY ZLIB_LIBRARY) 
else()
    FIND_PACKAGE_HANDLE_STANDARD_ARGS(FBX DEFAULT_MSG FBX_LIBRARY FBX_INCLUDE_DIR)
endif()

if(FBX_FOUND)
    set(FBX_INCLUDE_DIRS ${FBX_INCLUDE_DIR})
    _fbx_append_debugs(FBX_LIBRARIES      FBX_LIBRARY)
    add_definitions (-DFBXSDK_NEW_API)
    if(WIN32)
        add_definitions(-DK_PLUGIN -DK_FBXSDK -DK_NODLL)
        set(CMAKE_EXE_LINKER_FLAGS /NODEFAULTLIB:\"LIBCMT\")
        set(FBX_LIBRARIES ${FBX_LIBRARIES} Wininet.lib)
        if(FBX_VERSION VERSION_GREATER_EQUAL 2019)
            set(FBX_LIBRARIES ${FBX_LIBRARIES} ${XML2_LIBRARY} ${ZLIB_LIBRARY})
        endif()
    elseif(APPLE)
        set(FBX_LIBRARIES ${FBX_LIBRARIES} ${CARBON} ${SYSTEM_CONFIGURATION})
    endif()
endif()
