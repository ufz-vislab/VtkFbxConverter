
#include <fbxsdk.h>

#include "Common.h"

#include "VtkFbxConverter.h"
#include "VtkFbxHelper.h"

#include <iostream>

#include <vtkActor.h>
#include <vtkPolyDataMapper.h>


#include <boost/filesystem/operations.hpp>
#include <boost/regex.hpp>
#include <vector>
using namespace boost::filesystem;
using namespace std;

#define SAMPLE_FILENAME "Sample.fbx"

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
    
    for (vector<string>::const_iterator it = filenames.begin(); it != filenames.end(); ++it)
    {
        string filename(*it);
        vtkActor* actor = readVtkFile(filename);

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

		// Coordinate System conversion
//		FbxAxisSystem SceneAxisSystem = lScene->GetGlobalSettings().GetAxisSystem();
//		FbxAxisSystem OurAxisSystem(FbxAxisSystem::eZAxis, FbxAxisSystem::eParityEven, FbxAxisSystem::eRightHanded);
//		if( SceneAxisSystem != OurAxisSystem )
//		{
//			cout << "Converting" << endl;
//			OurAxisSystem.ConvertScene(lScene);
//		}

        lResult = SaveScene(lSdkManager, lScene, filename.c_str());

        delete converter;

        actor->Delete();
    }

    if(lResult == false)
    {
        FBXSDK_printf("\n\nAn error occurred while saving the scene...\n");
        DestroySdkObjects(lSdkManager);
        return 1;
    }

    // Destroy all objects created by the FBX SDK.
    // DestroySdkObjects(lSdkManager); // Crashes??

    return 0;
}
