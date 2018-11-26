# General project setup
cmake_minimum_required (VERSION 3.3)
project (VtkFbxConverter)
cmake_policy(SET CMP0074 NEW)

# Find
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}")
find_package(FBX REQUIRED)
add_definitions(-DFBX_VERSION_MAJOR=${FBX_VERSION_MAJOR})
if(ParaView_SOURCE_DIR)
  #building within the ParaView source tree
  include_directories(${VTK_INCLUDE_DIRS})
else()
  #building outside of the source tree
  find_package(ParaView REQUIRED)
  include(${PARAVIEW_USE_FILE})
endif()

##### Library #####
add_library(VtkFbxConverter STATIC
	Common.h
	Common.cxx
	VtkFbxHelper.h
	VtkFbxHelper.cxx
	VtkFbxConverter.h
	VtkFbxConverter.cxx
)
target_include_directories(VtkFbxConverter PUBLIC SYSTEM ${FBX_INCLUDE_DIRS})
target_link_libraries(VtkFbxConverter PUBLIC ${VTK_LIBRARIES} ${FBX_LIBRARIES})

##### Executable #####
add_executable(vtk_fbx_converter main.cxx)
target_link_libraries (vtk_fbx_converter VtkFbxConverter)

##### plugin #####
qt5_wrap_cpp(MOC_SRCS pqFbxStarter.h)
add_paraview_auto_start(IFACES IFACE_SRCS CLASS_NAME pqFbxStarter
                        STARTUP onStartup
                        SHUTDOWN onShutdown)

add_paraview_plugin(FbxExporter "${VERSION_MAJOR}.${VERSION_MINOR}"
	GUI_INTERFACES ${IFACES}
	SERVER_MANAGER_XML FbxExporter.xml
	SERVER_MANAGER_SOURCES vtkFbxExporter.cxx
	SOURCES pqFbxStarter.cxx ${MOC_SRCS} ${IFACE_SRCS}
	)
target_link_libraries(FbxExporter PUBLIC VtkFbxConverter)
