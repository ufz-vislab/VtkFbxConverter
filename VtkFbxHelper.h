/**
 * \file VtkFbxHelper.h
 * 2012-04-24 LB Initial implementation
 *
 */

#ifndef VTKFBXHELPER_H
#define VTKFBXHELPER_H

 #include <string>
 #include <vector>
 #include <vtkSmartPointer.h>

 class vtkActor;
 class vtkPolyData;
 class vtkUnstructuredGrid;

class VtkFbxHelper
{
public:

	/// Replace file extension
	static void replaceExt(std::string& s, const std::string& newExt);

	/// Get file extension
	static std::string getFileExt(const std::string& s);

	/// Get file name from full path
	static std::string getFilename(const std::string& s);

	/// Reads a vtk file and returns an actor (with polydata mapper)
	static vtkActor* readVtkFile(const std::string& filename);

	/// Subdivides an unstructured grid into a vector of subgrids.
	static std::vector<vtkSmartPointer<vtkUnstructuredGrid> > subdivide(vtkUnstructuredGrid* grid, int divisions);

	static void TestPointNormals(vtkPolyData* polydata);
	static void TestCellNormals(vtkPolyData* polydata);
	static bool GetPointNormals(vtkPolyData* polydata);
	static bool GetCellNormals(vtkPolyData* polydata);
};

#endif // VTKFBXHELPER_H