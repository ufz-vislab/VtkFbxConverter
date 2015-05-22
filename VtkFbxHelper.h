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

namespace VtkFbxHelper
{
  /**
  * Extracts full path from given pathname prior to the basename.
  *
  * Returns a string containing everything before the last path separator.
  * If the the pathname does not contain a path separator original pathname is
  * returned.
  */
  std::string extractPath(std::string const& pathname);

	/**
	 * Extracts basename from given pathname with extension.
	 *
	 * Returns a string containing everything after the last path separator.
	 * If the the pathname does not contain a path separator original pathname is
	 * returned.
	 */
	std::string extractBaseName(std::string const& pathname);

	/**
	 * Extracts basename from given pathname without its extension.
	 *
	 *  Same as extractBaseName(), but drops the file extension too.
	 */
	std::string extractBaseNameWithoutExtension(std::string const& pathname);

	/**
	 * Extract extension from filename
	 */
	std::string getFileExtension(std::string const& filename);

	/** Returns a string with file extension as found by getFileExtension()
	 * dropped.
	 */
	std::string dropFileExtension(std::string const& filename);

	/// Reads a vtk file and returns an actor (with polydata mapper)
	vtkActor* readVtkFile(const std::string& filename);

	/// Subdivides an unstructured grid into a vector of subgrids.
	std::vector<vtkSmartPointer<vtkUnstructuredGrid> >
		subdivide(vtkUnstructuredGrid* grid, int divisions);
	std::vector<vtkSmartPointer<vtkPolyData> >
		subdivide(vtkPolyData* grid, int divisions);
	std::vector<vtkSmartPointer<vtkPolyData> >
		subdivideByMaxPoints(vtkPolyData* grid, int maxPoints);

	void TestPointNormals(vtkPolyData* polydata);
	void TestCellNormals(vtkPolyData* polydata);
	bool GetPointNormals(vtkPolyData* polydata);
	bool GetCellNormals(vtkPolyData* polydata);
};

#endif // VTKFBXHELPER_H