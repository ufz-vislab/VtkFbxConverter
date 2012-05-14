/**
 * \file VtkFbxHelper.h
 * 2012-04-24 LB Initial implementation
 *
 */

#ifndef VTKFBXHELPER_H
#define VTKFBXHELPER_H

 #include <string>

 class vtkActor;
 class vtkPolyData;

// Replace file extension
void replaceExt(std::string& s, const std::string& newExt);

// Get file extension
std::string getFileExt(const std::string& s);

// Get file name from full path
std::string getFilename(const std::string& s);

// Reads a vtk file and returns an actor (with polydata mapper)
vtkActor* readVtkFile(const std::string& filename);

void TestPointNormals(vtkPolyData* polydata);
void TestCellNormals(vtkPolyData* polydata);
bool GetPointNormals(vtkPolyData* polydata);
bool GetCellNormals(vtkPolyData* polydata);

#endif // VTKFBXHELPER_H