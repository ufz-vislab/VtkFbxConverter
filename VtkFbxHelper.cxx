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

using namespace std;

void replaceExt(string& s, const string& newExt)
{
    string::size_type i = s.rfind('.', s.length());
    if (i != string::npos)
        s.replace(i + 1, newExt.length(), newExt);
}

string getFileExt(const string& s)
{
    size_t i = s.rfind('.', s.length());
    if (i != string::npos)
        return s.substr(i + 1, s.length() - i);
    return "";
}

string getFilename(const string& s)
{
	char sep = '/';
#ifdef _WIN32
	sep = '\\';
#endif

	size_t i = s.rfind(sep, s.length());
	if (i != string::npos)
		return (s.substr(i + 1, s.length() - 1));

	return ("");
}

vtkActor* readVtkFile(const string& filename)
{
    cout << "Opening file " << filename << " ... " << endl << flush;
    string fileExt = getFileExt(filename);

    vtkPolyDataMapper* mapper = vtkPolyDataMapper::New();
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
        reader = vtkXMLUnstructuredGridReader::New();
        reader->SetFileName(filename.c_str());
        reader->Update();

        vtkSmartPointer<vtkGeometryFilter> geoFilter =
                vtkSmartPointer<vtkGeometryFilter>::New();
        geoFilter->MergingOff();
        geoFilter->SetInputConnection(reader->GetOutputPort());
        geoFilter->Update();

        if(!GetPointNormals(geoFilter->GetOutput()))
        {
            // Generate normals
            std::cout << "Generating normals ..." << std::endl;
            vtkSmartPointer<vtkPolyDataNormals> normalGenerator =
                vtkSmartPointer<vtkPolyDataNormals>::New();
            normalGenerator->SetInputConnection(geoFilter->GetOutputPort());
            normalGenerator->ComputePointNormalsOn();
            normalGenerator->ComputeCellNormalsOff();
            //normalGenerator->Update();
            mapper->SetInputConnection(normalGenerator->GetOutputPort());
        }
        else
            mapper->SetInputConnection(geoFilter->GetOutputPort());
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
#if VTK_MAJOR_VERSION <= 5
    normalGenerator->SetInput(polydata);
#else
    normalGenerator->SetInputData(polydata);
#endif
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
    {
    std::cout << "Point normals were found!" << std::endl;
    }
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
#if VTK_MAJOR_VERSION <= 5
    normalGenerator->SetInput(polydata);
#else
    normalGenerator->SetInputData(polydata);
#endif
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
    {
    std::cout << "Cell normals were found!" << std::endl;
    }
}
 
 
 
bool GetPointNormals(vtkPolyData* polydata)
{
  std::cout << "In GetPointNormals: " << polydata->GetNumberOfPoints() << std::endl;
  std::cout << "Looking for point normals..." << std::endl;
 
  // Count points
  vtkIdType numPoints = polydata->GetNumberOfPoints();
  std::cout << "There are " << numPoints << " points." << std::endl;
 
  // Count triangles
  vtkIdType numPolys = polydata->GetNumberOfPolys();
  std::cout << "There are " << numPolys << " polys." << std::endl;
 
  ////////////////////////////////////////////////////////////////
  // Double normals in an array
  vtkDoubleArray* normalDataDouble =
    vtkDoubleArray::SafeDownCast(polydata->GetPointData()->GetArray("Normals"));
 
  if(normalDataDouble)
    {
    int nc = normalDataDouble->GetNumberOfTuples();
    std::cout << "There are " << nc
            << " components in normalDataDouble" << std::endl;
    return true;
    }
 
  ////////////////////////////////////////////////////////////////
  // Double normals in an array
  vtkFloatArray* normalDataFloat =
    vtkFloatArray::SafeDownCast(polydata->GetPointData()->GetArray("Normals"));
 
  if(normalDataFloat)
    {
    int nc = normalDataFloat->GetNumberOfTuples();
    std::cout << "There are " << nc
            << " components in normalDataFloat" << std::endl;
    return true;
    }
 
  ////////////////////////////////////////////////////////////////
  // Point normals
  vtkDoubleArray* normalsDouble =
    vtkDoubleArray::SafeDownCast(polydata->GetPointData()->GetNormals());
 
  if(normalsDouble)
    {
    std::cout << "There are " << normalsDouble->GetNumberOfComponents()
              << " components in normalsDouble" << std::endl;
    return true;
    }
 
  ////////////////////////////////////////////////////////////////
  // Point normals
  vtkFloatArray* normalsFloat =
    vtkFloatArray::SafeDownCast(polydata->GetPointData()->GetNormals());
 
  if(normalsFloat)
    {
    std::cout << "There are " << normalsFloat->GetNumberOfComponents()
              << " components in normalsFloat" << std::endl;
    return true;
    }
 
  /////////////////////////////////////////////////////////////////////
  // Generic type point normals
  vtkDataArray* normalsGeneric = polydata->GetPointData()->GetNormals(); //works
  if(normalsGeneric)
    {
    std::cout << "There are " << normalsGeneric->GetNumberOfTuples()
              << " normals in normalsGeneric" << std::endl;
 
    double testDouble[3];
    normalsGeneric->GetTuple(0, testDouble);
 
    std::cout << "Double: " << testDouble[0] << " "
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
  std::cout << "Normals not found!" << std::endl;
  return false;
 
}
 
 
bool GetCellNormals(vtkPolyData* polydata)
{
  std::cout << "Looking for cell normals..." << std::endl;
 
  // Count points
  vtkIdType numCells = polydata->GetNumberOfCells();
  std::cout << "There are " << numCells << " cells." << std::endl;
 
  // Count triangles
  vtkIdType numPolys = polydata->GetNumberOfPolys();
  std::cout << "There are " << numPolys << " polys." << std::endl;
 
  ////////////////////////////////////////////////////////////////
  // Double normals in an array
  vtkDoubleArray* normalDataDouble =
    vtkDoubleArray::SafeDownCast(polydata->GetCellData()->GetArray("Normals"));
 
  if(normalDataDouble)
    {
    int nc = normalDataDouble->GetNumberOfTuples();
    std::cout << "There are " << nc
            << " components in normalDataDouble" << std::endl;
    return true;
    }
 
  ////////////////////////////////////////////////////////////////
  // Double normals in an array
  vtkFloatArray* normalDataFloat =
    vtkFloatArray::SafeDownCast(polydata->GetCellData()->GetArray("Normals"));
 
  if(normalDataFloat)
    {
    int nc = normalDataFloat->GetNumberOfTuples();
    std::cout << "There are " << nc
            << " components in normalDataFloat" << std::endl;
    return true;
    }
 
  ////////////////////////////////////////////////////////////////
  // Point normals
  vtkDoubleArray* normalsDouble =
    vtkDoubleArray::SafeDownCast(polydata->GetCellData()->GetNormals());
 
  if(normalsDouble)
    {
    std::cout << "There are " << normalsDouble->GetNumberOfComponents()
              << " components in normalsDouble" << std::endl;
    return true;
    }
 
  ////////////////////////////////////////////////////////////////
  // Point normals
  vtkFloatArray* normalsFloat =
    vtkFloatArray::SafeDownCast(polydata->GetCellData()->GetNormals());
 
  if(normalsFloat)
    {
    std::cout << "There are " << normalsFloat->GetNumberOfComponents()
              << " components in normalsFloat" << std::endl;
    return true;
    }
 
  /////////////////////////////////////////////////////////////////////
  // Generic type point normals
  vtkDataArray* normalsGeneric = polydata->GetCellData()->GetNormals(); //works
  if(normalsGeneric)
    {
    std::cout << "There are " << normalsGeneric->GetNumberOfTuples()
              << " normals in normalsGeneric" << std::endl;
 
    double testDouble[3];
    normalsGeneric->GetTuple(0, testDouble);
 
    std::cout << "Double: " << testDouble[0] << " "
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
  std::cout << "Normals not found!" << std::endl;
  return false;
 
}
