/**
 * \file vtkFbxHelper.cpp
 * 2012-04-24 LB Initial implementation
 *
 */

// ** INCLUDES **
#include "VtkFbxHelper.h"

#include <vtkActor.h>
#include <vtkPolyDataNormals.h>
 #include <vtkPolyDataMapper.h>
#include <vtkSmartPointer.h>
#include <vtkXMLImageDataReader.h>
#include <vtkXMLPolyDataReader.h>
#include <vtkXMLRectilinearGridReader.h>
#include <vtkXMLStructuredGridReader.h>
#include <vtkXMLUnstructuredGridReader.h>
#include <vtkDataSetMapper.h>
#include <vtkGenericDataObjectReader.h>
#include <vtkGeometryFilter.h>
#include <vtkImageDataGeometryFilter.h>
#include <vtkImageMapper.h>

#include <vtkCellData.h>
#include <vtkDoubleArray.h>
#include <vtkFloatArray.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkPointData.h>
#include <vtkExtractGeometry.h>
#include <vtkUnstructuredGrid.h>
#include <vtkDataSetMapper.h>
#include <vtkBox.h>
#include <vtkNew.h>
#include <vtkExtractPolyDataGeometry.h>
#include <vtkCleanPolyData.h>

using namespace std;

namespace VtkFbxHelper
{
size_t findLastPathSeparator(std::string const& path)
{
	return path.find_last_of("/\\");
}

size_t findLastDot(std::string const& path)
{
	return path.find_last_of(".");
}

std::string dropFileExtension(std::string const& filename)
{
	// Look for dots in filename.
	const size_t p = findLastDot(filename);
	if (p == std::string::npos)
		return filename;

	// Check position of the last path separator.
	const size_t s = findLastPathSeparator(filename);
	if (s != std::string::npos && p < s)
		return filename;

	return filename.substr(0, p);
}

std::string extractPath(std::string const& pathname)
{
  const size_t p = findLastPathSeparator(pathname);
  return pathname.substr(0, p);
}

std::string extractBaseName(std::string const& pathname)
{
	const size_t p = findLastPathSeparator(pathname);
	return pathname.substr(p + 1);
}

std::string extractBaseNameWithoutExtension(std::string const& pathname)
{
	std::string basename = extractBaseName(pathname);
	return dropFileExtension(basename);
}

std::string getFileExtension(const std::string &path)
{
	const std::string str = extractBaseName(path);
	const size_t p = findLastDot(str);
	if (p == std::string::npos)
		return std::string();
	return str.substr(p + 1);
}

vtkActor* readVtkFile(const string& filename)
{
	cout << "Opening file " << filename << " ... " << endl << flush;
	string fileExt = getFileExtension(filename);
	vtkMapper* mapper = vtkPolyDataMapper::New();
	vtkXMLDataReader* reader = NULL;
	vtkGenericDataObjectReader* oldStyleReader = NULL;
	if (fileExt.find("vti") != string::npos)
	{
		reader = vtkXMLImageDataReader::New();
		reader->SetFileName(filename.c_str());
		reader->Update();
		vtkSmartPointer<vtkImageDataGeometryFilter> geoFilter =
			vtkSmartPointer<vtkImageDataGeometryFilter>::New();
		geoFilter->SetInputConnection(reader->GetOutputPort());
		mapper->SetInputConnection(geoFilter->GetOutputPort());
	}
	if (fileExt.find("vtr") != string::npos)
	{
		reader = vtkXMLRectilinearGridReader::New();
		reader->SetFileName(filename.c_str());
		reader->Update();
		vtkSmartPointer<vtkGeometryFilter> geoFilter =
			vtkSmartPointer<vtkGeometryFilter>::New();
		geoFilter->SetInputConnection(reader->GetOutputPort());
		mapper->SetInputConnection(geoFilter->GetOutputPort());
	}
	else if (fileExt.find("vts") != string::npos)
	{
		reader = vtkXMLStructuredGridReader::New();
		reader->SetFileName(filename.c_str());
		reader->Update();
		vtkSmartPointer<vtkGeometryFilter> geoFilter =
			vtkSmartPointer<vtkGeometryFilter>::New();
		geoFilter->SetInputConnection(reader->GetOutputPort());
		mapper->SetInputConnection(geoFilter->GetOutputPort());
	}
	else if (fileExt.find("vtp") != string::npos)
	{
		reader = vtkXMLPolyDataReader::New();
		reader->SetFileName(filename.c_str());
		reader->Update();
		mapper->SetInputConnection(reader->GetOutputPort());
	}
	else if (fileExt.find("vtu") != string::npos)
	{
		mapper = vtkDataSetMapper::New();
		reader = vtkXMLUnstructuredGridReader::New();
		reader->SetFileName(filename.c_str());
		reader->Update();
		// vtkSmartPointer<vtkGeometryFilter> geoFilter =
		//         vtkSmartPointer<vtkGeometryFilter>::New();
		// geoFilter->MergingOff();
		// geoFilter->SetInputConnection(reader->GetOutputPort());
		// geoFilter->Update();
		// if(!GetPointNormals(geoFilter->GetOutput()))
		// {
		//     // Generate normals
		//     std::cout << "Generating normals ..." << std::endl;
		//     vtkSmartPointer<vtkPolyDataNormals> normalGenerator =
		//         vtkSmartPointer<vtkPolyDataNormals>::New();
		//     normalGenerator->SetInputConnection(geoFilter->GetOutputPort());
		//     normalGenerator->ComputePointNormalsOn();
		//     normalGenerator->ComputeCellNormalsOff();
		//     //normalGenerator->Update();
		//     mapper->SetInputConnection(normalGenerator->GetOutputPort());
		// }
		// else
			mapper->SetInputConnection(reader->GetOutputPort());
	}
	else if (fileExt.find("vtk") != string::npos)
	{
		oldStyleReader = vtkGenericDataObjectReader::New();
		oldStyleReader->SetFileName(filename.c_str());
		oldStyleReader->Update();
		if(oldStyleReader->IsFilePolyData())
			mapper->SetInputConnection(oldStyleReader->GetOutputPort());
		else
		{
			vtkSmartPointer<vtkGeometryFilter> geoFilter =
				vtkSmartPointer<vtkGeometryFilter>::New();
			geoFilter->SetInputConnection(oldStyleReader->GetOutputPort());
			mapper->SetInputConnection(geoFilter->GetOutputPort());
		}
	}
	else
	{
		cout << "Not a valid vtk file ending (vti, vtr, vts, vtp, vtu, vtk)" <<
		endl;
		return NULL;
	}
	vtkActor* actor = vtkActor::New();
	mapper->Update();
	actor->SetMapper(mapper);
	if (reader)
		reader->Delete();
	if (oldStyleReader)
		oldStyleReader->Delete();
	return actor;
}

void TestPointNormals(vtkPolyData* polydata)
{
	std::cout << "In TestPointNormals: " << polydata->GetNumberOfPoints() << std::endl;
	// Try to read normals directly
	bool hasPointNormals = GetPointNormals(polydata);

	if(!hasPointNormals)
	{
		std::cout << "No point normals were found. Computing normals..." << std::endl;

		// Generate normals
		vtkSmartPointer<vtkPolyDataNormals> normalGenerator = vtkSmartPointer<vtkPolyDataNormals>::New();
		normalGenerator->SetInputData(polydata);
		normalGenerator->ComputePointNormalsOn();
		normalGenerator->ComputeCellNormalsOff();
		normalGenerator->Update();
		/*
		// Optional settings
		normalGenerator->SetFeatureAngle(0.1);
		normalGenerator->SetSplitting(1);
		normalGenerator->SetConsistency(0);
		normalGenerator->SetAutoOrientNormals(0);
		normalGenerator->SetComputePointNormals(1);
		normalGenerator->SetComputeCellNormals(0);
		normalGenerator->SetFlipNormals(0);
		normalGenerator->SetNonManifoldTraversal(1);
		*/

		polydata = normalGenerator->GetOutput();

		// Try to read normals again
		hasPointNormals = GetPointNormals(polydata);

		std::cout << "On the second try, has point normals? " << hasPointNormals << std::endl;
	}
	else
		std::cout << "Point normals were found!" << std::endl;
}

void TestCellNormals(vtkPolyData* polydata)
{
	// Try to read normals directly
	bool hasCellNormals = GetCellNormals(polydata);

	if(!hasCellNormals)
	{
		std::cout << "No cell normals were found. Computing normals..." << std::endl;

		// Generate normals
		vtkSmartPointer<vtkPolyDataNormals> normalGenerator = vtkSmartPointer<vtkPolyDataNormals>::New();
		normalGenerator->SetInputData(polydata);
		normalGenerator->ComputePointNormalsOff();
		normalGenerator->ComputeCellNormalsOn();
		normalGenerator->Update();
		/*
		// Optional settings
		normalGenerator->SetFeatureAngle(0.1);
		normalGenerator->SetSplitting(1);
		normalGenerator->SetConsistency(0);
		normalGenerator->SetAutoOrientNormals(0);
		normalGenerator->SetComputePointNormals(1);
		normalGenerator->SetComputeCellNormals(0);
		normalGenerator->SetFlipNormals(0);
		normalGenerator->SetNonManifoldTraversal(1);
		*/

		polydata = normalGenerator->GetOutput();

		// Try to read normals again
		hasCellNormals = GetCellNormals(polydata);

		std::cout << "On the second try, has cell normals? " << hasCellNormals << std::endl;
	}
	else
		std::cout << "Cell normals were found!" << std::endl;
}

bool GetPointNormals(vtkPolyData* polydata)
{
	// Double normals in an array
	vtkDoubleArray* normalDataDouble =
		vtkDoubleArray::SafeDownCast(polydata->GetPointData()->GetArray("Normals"));

	if(normalDataDouble)
	{
		int nc = normalDataDouble->GetNumberOfTuples();
		std::cout << "    There are " << nc
			<< " components in normalDataDouble" << std::endl;
		return true;
	}

	// Double normals in an array
	vtkFloatArray* normalDataFloat =
		vtkFloatArray::SafeDownCast(polydata->GetPointData()->GetArray("Normals"));

	if(normalDataFloat)
	{
		int nc = normalDataFloat->GetNumberOfTuples();
		std::cout << "    There are " << nc
			<< " components in normalDataFloat" << std::endl;
		return true;
	}

	// Point normals
	vtkDoubleArray* normalsDouble =
		vtkDoubleArray::SafeDownCast(polydata->GetPointData()->GetNormals());

	if(normalsDouble)
	{
		std::cout << "    There are " << normalsDouble->GetNumberOfComponents()
			<< " components in normalsDouble" << std::endl;
		return true;
	}

	// Point normals
	vtkFloatArray* normalsFloat =
		vtkFloatArray::SafeDownCast(polydata->GetPointData()->GetNormals());

	if(normalsFloat)
	{
		std::cout << "    There are " << normalsFloat->GetNumberOfComponents()
			<< " components in normalsFloat" << std::endl;
		return true;
	}

	// Generic type point normals
	vtkDataArray* normalsGeneric = polydata->GetPointData()->GetNormals(); //works
	if(normalsGeneric)
	{
		std::cout << "    There are " << normalsGeneric->GetNumberOfTuples()
			<< " normals in normalsGeneric" << std::endl;

		double testDouble[3];
		normalsGeneric->GetTuple(0, testDouble);

		std::cout << "    Double: " << testDouble[0] << " "
			<< testDouble[1] << " " << testDouble[2] << std::endl;

		// Can't do this:
		/*
		float testFloat[3];
		normalsGeneric->GetTuple(0, testFloat);

		std::cout << "Float: " << testFloat[0] << " "
							<< testFloat[1] << " " << testFloat[2] << std::endl;
		*/
		return true;
	}

	std::cout << "    Normals not found!" << std::endl;
	return false;
}

bool GetCellNormals(vtkPolyData* polydata)
{
	// Double normals in an array
	vtkDoubleArray* normalDataDouble =
		vtkDoubleArray::SafeDownCast(polydata->GetCellData()->GetArray("Normals"));

	if(normalDataDouble)
	{
		int nc = normalDataDouble->GetNumberOfTuples();
		std::cout << "    There are " << nc
			<< " components in normalDataDouble" << std::endl;
		return true;
	}

	// Double normals in an array
	vtkFloatArray* normalDataFloat =
		vtkFloatArray::SafeDownCast(polydata->GetCellData()->GetArray("Normals"));

	if(normalDataFloat)
	{
		int nc = normalDataFloat->GetNumberOfTuples();
		std::cout << "    There are " << nc
			<< " components in normalDataFloat" << std::endl;
		return true;
	}

	// Point normals
	vtkDoubleArray* normalsDouble =
		vtkDoubleArray::SafeDownCast(polydata->GetCellData()->GetNormals());

	if(normalsDouble)
	{
		std::cout << "    There are " << normalsDouble->GetNumberOfComponents()
			<< " components in normalsDouble" << std::endl;
		return true;
	}

	// Point normals
	vtkFloatArray* normalsFloat =
		vtkFloatArray::SafeDownCast(polydata->GetCellData()->GetNormals());

	if(normalsFloat)
	{
		std::cout << "    There are " << normalsFloat->GetNumberOfComponents()
			<< " components in normalsFloat" << std::endl;
		return true;
	}

	// Generic type point normals
	vtkDataArray* normalsGeneric = polydata->GetCellData()->GetNormals(); //works
	if(normalsGeneric)
	{
		std::cout << "    There are " << normalsGeneric->GetNumberOfTuples()
			<< " normals in normalsGeneric" << std::endl;

		double testDouble[3];
		normalsGeneric->GetTuple(0, testDouble);

		std::cout << "    Double: " << testDouble[0] << " "
			<< testDouble[1] << " " << testDouble[2] << std::endl;

		// Can't do this:
		/*
		float testFloat[3];
		normalsGeneric->GetTuple(0, testFloat);

		std::cout << "Float: " << testFloat[0] << " "
							<< testFloat[1] << " " << testFloat[2] << std::endl;
		*/
		return true;
	}

	// If the function has not yet quit, there were none of these types of normals
	std::cout << "    Normals not found!" << std::endl;
	return false;
}

std::vector<vtkSmartPointer<vtkUnstructuredGrid> > subdivide(vtkUnstructuredGrid* grid, int divisions)
{
	std::vector<vtkSmartPointer<vtkUnstructuredGrid> > subGrids;
	if(divisions == 1)
	{
		subGrids.push_back(grid);
		return subGrids;
	}
	vtkSmartPointer<vtkUnstructuredGrid> actualGrid = grid;
	cout << "Subdividing grid (" << divisions << "x" << divisions << ") ..." << endl;
	actualGrid->ComputeBounds();

	cout << "  Grid points: " << grid->GetNumberOfPoints() << endl;
	cout << "  grid cells: " << grid->GetNumberOfCells() << endl;
	double bounds[6];
	actualGrid->GetBounds(bounds);
	cout << "  Grid bounds: (" << bounds[0] << ", " << bounds[2] << ", " << bounds[4] <<
		") - (" << bounds[1] << ", " << bounds[3] << ", " << bounds[5] << ")" << endl;
	const double subgridXLength = (bounds[1] - bounds[0]) / divisions;
	const double subgridYLength = (bounds[3] - bounds[2]) / divisions;

	for(int x = 0; x < divisions; ++x)
	{
		for (int y = 0; y < divisions; ++y)
		{
			vtkNew<vtkExtractGeometry> extractFilterInner;
			extractFilterInner->ExtractInsideOn();
			extractFilterInner->ExtractBoundaryCellsOn();
			vtkNew<vtkExtractGeometry> extractFilterOuter;
			extractFilterOuter->ExtractInsideOff();
			extractFilterInner->ExtractBoundaryCellsOn();

			// Extract subgrid
			vtkNew<vtkBox> extractRegion;
			extractRegion->SetXMin(bounds[0] + subgridXLength * x,
			                       bounds[2] + subgridYLength * y,
			                       bounds[4]);
			extractRegion->SetXMax(bounds[0] + subgridXLength * (x + 1),
			                       bounds[2] + subgridYLength * (y + 1),
			                       bounds[5]);
			extractFilterInner->SetInputData(actualGrid);
			extractFilterInner->SetImplicitFunction(extractRegion.GetPointer());
			extractFilterInner->Update();
			vtkSmartPointer<vtkUnstructuredGrid> subGrid = extractFilterInner->GetOutput();
			cout << "    Subgrid (" << x << "," << y << ") points: " << subGrid->GetNumberOfPoints() << endl;
			cout << "    subgrid (" << x << "," << y << ") cells: " << subGrid->GetNumberOfCells() << endl;
			subGrids.push_back(subGrid);

			// Extract remaining grid
			extractFilterOuter->SetInputData(actualGrid);
			extractFilterOuter->SetImplicitFunction(extractRegion.GetPointer());
			extractFilterOuter->Update();
			actualGrid = extractFilterOuter->GetOutput();
		}
	}

	cout << "Subdivision finished.\n" << endl;
	return subGrids;
}

std::vector<vtkSmartPointer<vtkPolyData> > subdivideByMaxPoints(vtkPolyData* grid, int maxPoints)
{
	int numParts = ceil(grid->GetNumberOfCells() / (float)maxPoints);
	int subdivisions = ceil(sqrt((float)numParts));
	return subdivide(grid, subdivisions);
}

std::vector<vtkSmartPointer<vtkPolyData> > subdivide(vtkPolyData* grid, int divisions)
{
	std::vector<vtkSmartPointer<vtkPolyData> > subGrids;
	if(divisions == 1)
	{
		subGrids.push_back(grid);
		return subGrids;
	}
	vtkSmartPointer<vtkPolyData> actualGrid = grid;
	cout << "  Subdividing polydata (" << divisions << "x" << divisions << ") ..." << endl;
	actualGrid->ComputeBounds();

	cout << "    Polydata points: " << grid->GetNumberOfPoints() << endl;
	cout << "    Polydata cells: " << grid->GetNumberOfCells() << endl;
	double bounds[6];
	actualGrid->GetBounds(bounds);
	cout << "    Polydata bounds: (" << bounds[0] << ", " << bounds[2] << ", " << bounds[4] <<
		") - (" << bounds[1] << ", " << bounds[3] << ", " << bounds[5] << ")" << endl;
	const double subgridXLength = (bounds[1] - bounds[0]) / divisions;
	const double subgridYLength = (bounds[3] - bounds[2]) / divisions;

	for(int x = 0; x < divisions; ++x)
	{
		for (int y = 0; y < divisions; ++y)
		{
			vtkNew<vtkExtractPolyDataGeometry> extractFilterInner;
			extractFilterInner->ExtractInsideOn();
			extractFilterInner->ExtractBoundaryCellsOn();
			vtkNew<vtkExtractPolyDataGeometry> extractFilterOuter;
			extractFilterOuter->ExtractInsideOff();
			extractFilterInner->ExtractBoundaryCellsOn();

			// Extract subgrid
			vtkNew<vtkBox> extractRegion;
			extractRegion->SetXMin(bounds[0] + subgridXLength * x,
			                       bounds[2] + subgridYLength * y,
			                       bounds[4]);
			extractRegion->SetXMax(bounds[0] + subgridXLength * (x + 1),
			                       bounds[2] + subgridYLength * (y + 1),
			                       bounds[5]);
			extractFilterInner->SetInputData(actualGrid);
			extractFilterInner->SetImplicitFunction(extractRegion.GetPointer());
			extractFilterInner->Update();

			vtkNew<vtkCleanPolyData> cleanFilter;
			cleanFilter->SetInputData(extractFilterInner->GetOutput());
			cleanFilter->PointMergingOn();
			cleanFilter->Update();

			vtkSmartPointer<vtkPolyData> subGrid = cleanFilter->GetOutput();
			cout << "      Polydata (" << x << "," << y << ") points: " << subGrid->GetNumberOfPoints() << endl;
			cout << "      Polydata (" << x << "," << y << ") cells: " << subGrid->GetNumberOfCells() << endl;
			if(subGrid->GetNumberOfPoints() > 0 && subGrid->GetNumberOfCells() > 0)
				subGrids.push_back(subGrid);

			// Extract remaining grid
			extractFilterOuter->SetInputData(actualGrid);
			extractFilterOuter->SetImplicitFunction(extractRegion.GetPointer());
			extractFilterOuter->Update();
			actualGrid = extractFilterOuter->GetOutput();
		}
	}

	cout << "  Subdivision finished.\n" << endl;
	return subGrids;
}
} // namespace