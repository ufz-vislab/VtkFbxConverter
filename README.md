# VtkFbxConverter #

Converts a vtkActor to a FbxNode.

## Prerequisites ##

- VTK >= 5.8
- FBX SDK (tested with 2013.1)
- Boost

## Converter tool ##

The executabel `vtk_fbx_converter` can be used to convert Vtk files (*.vt**) to a FBX file directly.

Usage:

		vtk_fbx_converter output/dir/ inputfile.vtu


## Inclusion in other projects ##

		# Find package
		SET(VtkFbxConverter_DIR /build/dir) # Optional
		FIND_PACKAGE(VtkFbxConverter 0.1.0
			REQUIRED COMPONENTS lib
			CONFIG)

		IF(VTKFBXCONVERTER_FOUND)
			INCLUDE_DIRECTORIES(${VTKFBXCONVERTER_INCLUDE_DIRS})
			TARGET_LINK_LIBRARIES(target ${VTKFBXCONVERTER_LIBRARIES})
		ENDIF() # VTKFBXCONVERTER_FOUND
