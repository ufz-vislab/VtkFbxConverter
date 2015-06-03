# VTK FBX Converter

## Introduction

This project provides an open source set of cross-platform tools to
convert/export visualization models created using
[The Visualization Toolkit (VTK)](http://www.vtk.org/) into Autodesk's
[FBX](http://www.autodesk.com/products/fbx/overview) file format.

VTK provides a variety of advanced algorithms for
surface reconstruction, implicit modelling, decimation, etc. Autodesk's FBX file
format is a widely adopted file format for importing data models into virtual
simulation engines. This work aims to bridge the gap between advanced scientific
model simulations and realistic graphics rendering.

Currently, the project builds a stand-alone executable that can be used to
convert a VTK data file to an FBX file. In addition, the software builds a
plugin library for [ParaView](http://www.paraview.org/) that helps export the
scene set up in ParaView to an FBX file.

This work is a fork of the project
[ufz-vislab/VtkFbxConverter](https://github.com/ufz-vislab/VtkFbxConverter).

## Prerequisites

VtkFbxConverter requires the following software to be built and installed
for the operating system of your choice.

- [CMake](http://www.cmake.org/) (Version 2.8.3 or later)
- [ParaView](http://www.paraview.org/) (Version 4.0 or later)
- [FBX SDK](http://www.autodesk.com/products/fbx/overview) (Version 2014.2 or
later)

## Get the source

    git clone https://github.com/sankhesh/VtkFbxConverter.git

## Configure

Configure the build system using CMake.

Set the following CMake variables:

* FBX\_INCLUDE\_DIR: Path to the ``include`` directory of your FBX SDK
   installation

* FBX\_LIBRARY: Path to the FBX SDK release library (libfbxsdk.a or
                                                       libfbxsdk-md.dll).
Usually located under
`<FBX SDK installation>/lib/<compiler>/<architecture (x86 or x64)>/release/`.

* FBX\_LIBRARY\_DEBUG: Path to the FBX SDK debug library (libfbxsdk.a or
                                                            libfbxsdk-md.dll).
Usually located under
`<FBX SDK installation>/lib/<compiler>/<architecture (x86 or x64)>/debug/`.

* ParaView\_DIR: Path to ParaView build or install directory.

* INSTALL\_IN\_PARAVIEW (Optional flag): ON. If provided, the plugin will be
installed in the ParaView build directory.

Once all the required variables are set, hit ``Generate`` to generate the
build configuration.

## Build

Build using the *make* tool of choice selected in the [Configure](#configure)
  section.

    make

The plugin can be installed with `make install` inside the ParaView plugin-directory if the
`INSTALL_IN_PARAVIEW` option was set in the [Configure](#configure) section.

    make install

## Usage

### ParaView FBX Exporter plugin

- Start ParaView, load the plugin via `Tools / Manage Plugins`
- All currently visible pipeline items are exported with `File / Export Scene`
- Each visible vtkActor is converted to a FbxNode

### Stand-alone VTK FBX converter

The executable `vtk_fbx_converter` can be used to convert Vtk files (.vt\*)
 to a FBX file directly.

    vtk_fbx_converter output/dir/ inputfile.vtu


## Inclusion in other projects

    # Find package
    SET(VtkFbxConverter_DIR /build/dir) # Optional
    FIND_PACKAGE(VtkFbxConverter 0.1.0
    	REQUIRED COMPONENTS lib
    	CONFIG)

    IF(VTKFBXCONVERTER_FOUND)
    	INCLUDE_DIRECTORIES(${VTKFBXCONVERTER_INCLUDE_DIRS})
    	TARGET_LINK_LIBRARIES(target ${VTKFBXCONVERTER_LIBRARIES})
    ENDIF() # VTKFBXCONVERTER_FOUND

## License

All work in this project is distributed under the MIT License. See
[LICENSE.txt](https://github.com/sankhesh/VtkFbxConverter/blob/master/LICENSE.txt)
for details.
