# FBX exporter plugin for ParaView

[![DOI](https://zenodo.org/badge/DOI/10.5281/zenodo.30345.svg)](https://doi.org/10.5281/zenodo.30345)

## Download, Installation and Usage

- Get the DLL from the [latest release](https://github.com/ufz-vislab/VtkFbxConverter/releases/latest) **OR** [the bleeding edge from the CI-server](https://jenkins.opengeosys.org/job/ufz-vislab/job/VtkFbxConverter/job/master/lastSuccessfulBuild/artifact/build/pv_plugin/FbxExporter.dll) (currently for ParaView 5.2)
- Copy the DLL into the `bin`-folder of your ParaView-installation
- Start ParaView, load the plugin via `Tools / Manage Plugins`
- All currently visible pipeline items are exported with `File / Export Scene`

## Build instructions (Windows)

Build ParaView-[Superbuild](http://www.paraview.org/Wiki/ParaView/Superbuild) with [ninja](http://martine.github.io/ninja/) and Visual Studio 2008:

    git clone http://paraview.org/ParaViewSuperbuild.git src
    [checkout a ParaView version, e.g. v4.3.1]
    mkdir build
    cd build
    cmake ..\src -G Ninja -DCMAKE_BUILD_TYPE=Release -DENABLE_paraview=ON -DENABLE_qt=ON -DENABLE_python=ON -Dparaviewsdk_ENABLED=ON
    ninja


Install the [FBX SDK 2015.1 for Visual Studio 2008 x64](http://images.autodesk.com/adsk/files/fbx20151_fbxsdk_vs2008_win.exe).

Build the ParaView plugin:

    git clone https://github.com/ufz-vislab/VtkFbxConverter.git
    mkdir build
    cd build
    cmake ../src -G "Visual Studio 9 2008 Win64" -DParaView_DIR=../../ParaView/build/install/lib/cmake/paraview-4.3 -DFBX_VERSION=2015.1
    cmake --build . --config Release


The plugin can be installed with `make install` inside the ParaView plugin-directory if the
`INSTALL_IN_PARAVIEW`-option was set:

    cmake -DParaView_DIR=~/paraview_build -DINSTALL_IN_PARAVIEW=ON ../path/to/sources
    make install

# VtkFbxConverter #

Converts a vtkActor to a FbxNode.

## Prerequisites ##

- VTK >= 6.0 or ParaView >= 4.0
- FBX SDK (tested with 2014.2)

## Converter tool ##

The executable `vtk_fbx_converter` can be used to convert Vtk files (*.vt**) to a FBX file directly.

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
