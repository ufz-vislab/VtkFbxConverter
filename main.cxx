
#include <fbxsdk.h>

#include "Common.h"

#include "VtkFbxConverter.h"

#include <iostream>

#include <vtkActor.h>
#include <vtkDataSetMapper.h>
#include <vtkGenericDataObjectReader.h>
#include <vtkGeometryFilter.h>
#include <vtkImageDataGeometryFilter.h>
#include <vtkImageMapper.h>
#include <vtkPolyDataMapper.h>
#include <vtkPolyDataNormals.h>
#include <vtkSmartPointer.h>
#include <vtkXMLImageDataReader.h>
#include <vtkXMLPolyDataReader.h>
#include <vtkXMLRectilinearGridReader.h>
#include <vtkXMLStructuredGridReader.h>
#include <vtkXMLUnstructuredGridReader.h>

#include <boost/filesystem/operations.hpp>
#include <boost/regex.hpp>
#include <vector>
using namespace boost::filesystem;
using namespace std;

#define SAMPLE_FILENAME "Sample.fbx"

// Replace file extension
void replaceExt(string& s, const string& newExt)
{
    string::size_type i = s.rfind('.', s.length());
    if (i != string::npos)
        s.replace(i + 1, newExt.length(), newExt);
}

// Get file extension
string getFileExt(const string& s)
{
    size_t i = s.rfind('.', s.length());
    if (i != string::npos)
        return s.substr(i + 1, s.length() - i);
    return "";
}

// Get file name from full path
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

// No arguments: batch convert all vt* files
// switch argument: batch convert all vt* files into one osb file with a switch
// file argument: convert only the specified file
int main (int argc, char const* argv[])
{
    // Init fbx
    FbxManager* lSdkManager = NULL;
    FbxScene* lScene = NULL;
    bool lResult;
    InitializeSdkObjects(lSdkManager, lScene);

    string outputDirectory = "";
    vector<string> filenames;
    if (argc > 2)
    {
        outputDirectory = string(argv[1]);
        filenames.push_back(string(argv[2]));
    }

    // if (useSwitch || filenames.empty())
    // {
    //     const boost::regex e(".+\\.vt[a-z]");
    //     directory_iterator end;
    //     for (directory_iterator it("./"); it != end; ++it)
    //     {
    //         string curFile = it->path().filename().string();
    //         if (regex_match(curFile, e))
    //             filenames.push_back(curFile);
    //     }
    // }

    vtkPolyDataMapper* mapper = vtkPolyDataMapper::New();
    
    for (vector<string>::const_iterator it = filenames.begin(); it != filenames.end(); ++it)
    {
        string filename(*it);
        cout << "Opening file " << filename << " ... " << endl << flush;
        string fileExt = getFileExt(filename);

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
            return 1;
        }

        if (fileExt.find("vtk") == string::npos)
        {
            reader->SetFileName(filename.c_str());
            reader->Update();
        }

        vtkActor* actor = vtkActor::New();
        actor->SetMapper(mapper);
        
        VtkFbxConverter* converter = new VtkFbxConverter(actor, lScene);
        converter->convert();
        FbxNode* node = converter->getNode();

        if (node != NULL)
            lScene->GetRootNode()->AddChild(node);

        // Save the scene.
        replaceExt(filename, "fbx");
		string filenameWithoutPath = getFilename(filename);
        filename = outputDirectory.append(filenameWithoutPath);
        cout << "Saving to " << filename << " ..." << endl;

		// Embed media files
		(*(lSdkManager->GetIOSettings())).SetBoolProp(EXP_FBX_EMBEDDED, true);
        lResult = SaveScene(lSdkManager, lScene, filename.c_str());

        delete converter;

        actor->Delete();
        if (reader)
            reader->Delete();
        if (oldStyleReader)
            oldStyleReader->Delete();
    }

    if(lResult == false)
    {
        FBXSDK_printf("\n\nAn error occurred while saving the scene...\n");
        DestroySdkObjects(lSdkManager);
        return 1;
    }

    // Destroy all objects created by the FBX SDK.
    // DestroySdkObjects(lSdkManager); // Crashes??

    cout << "File conversion finished" << endl;

    return 0;
}
