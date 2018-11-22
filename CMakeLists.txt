# General project setup
CMAKE_MINIMUM_REQUIRED (VERSION 3.3)
PROJECT (VtkFbxConverter)

# Find
LIST(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}")
FIND_PACKAGE(FBX REQUIRED)
ADD_DEFINITIONS(-DFBX_VERSION_MAJOR=${FBX_VERSION_MAJOR})
IF (ParaView_SOURCE_DIR)
  #building within the ParaView source tree
  INCLUDE_DIRECTORIES(${VTK_INCLUDE_DIRS})
ELSE ()
  #building outside of the source tree
  FIND_PACKAGE(ParaView REQUIRED)
  INCLUDE(${PARAVIEW_USE_FILE})
ENDIF ()

##### Library #####
add_library(VtkFbxConverter
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
