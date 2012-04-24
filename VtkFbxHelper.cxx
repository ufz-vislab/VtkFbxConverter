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
        vtkSmartPointer<vtkImageDataGeometryFilter> geoFilter =
                vtkSmartPointer<vtkImageDataGeometryFilter>::New();
        geoFilter->SetInputConnection(reader->GetOutputPort());
        mapper->SetInputConnection(geoFilter->GetOutputPort());
    }
    if (fileExt.find("vtr") != string::npos)
    {
        reader = vtkXMLRectilinearGridReader::New();
        vtkSmartPointer<vtkGeometryFilter> geoFilter =
                vtkSmartPointer<vtkGeometryFilter>::New();
        geoFilter->SetInputConnection(reader->GetOutputPort());
        mapper->SetInputConnection(geoFilter->GetOutputPort());
    }
    else if (fileExt.find("vts") != string::npos)
    {
        reader = vtkXMLStructuredGridReader::New();
        vtkSmartPointer<vtkGeometryFilter> geoFilter =
                vtkSmartPointer<vtkGeometryFilter>::New();
        geoFilter->SetInputConnection(reader->GetOutputPort());
        mapper->SetInputConnection(geoFilter->GetOutputPort());
    }
    else if (fileExt.find("vtp") != string::npos)
    {
        reader = vtkXMLPolyDataReader::New();
        mapper->SetInputConnection(reader->GetOutputPort());
    }
    else if (fileExt.find("vtu") != string::npos)
    {
        reader = vtkXMLUnstructuredGridReader::New();
        vtkSmartPointer<vtkGeometryFilter> geoFilter =
                vtkSmartPointer<vtkGeometryFilter>::New();
        geoFilter->SetInputConnection(reader->GetOutputPort());
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

    if (fileExt.find("vtk") == string::npos)
    {
        reader->SetFileName(filename.c_str());
        reader->Update();
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