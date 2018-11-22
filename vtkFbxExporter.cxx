/**
 * \file vtkFbxExporter.cxx
 * 2012-06-01 LB Initial implementation
 *
 * Implementation of vtkFbxExporter class
 */

// ** INCLUDES **
#include "vtkFbxExporter.h"
#include "VtkFbxConverter.h"
#include "VtkFbxHelper.h"

#include "vtkAssemblyNode.h"
#include "vtkAssemblyPath.h"
#include "vtkObjectFactory.h"
#include "vtkRendererCollection.h"
#include "vtkRenderWindow.h"

#include <fbxsdk.h>
#include <QDebug>
#include <QDir>

#include "Common.h"

extern FbxManager* lSdkManager;
extern FbxScene* lScene;

vtkStandardNewMacro(vtkFbxExporter);

vtkFbxExporter::vtkFbxExporter()
{
	this->DebugOn();
	this->FileName = NULL;
	this->BinaryMode = true;
	this->MaxPoints = 65000;
}

vtkFbxExporter::~vtkFbxExporter()
{
	if ( this->FileName )
		delete [] this->FileName;
}

void vtkFbxExporter::WriteData()
{
	// make sure the user specified a FileName or FilePointer
	if (this->FileName == NULL)
	{
		vtkErrorMacro(<< "Please specify FileName to use");
		return;
	}

	// get the renderer
	vtkRenderer *ren = this->RenderWindow->GetRenderers()->GetFirstRenderer();

	// make sure it has at least one actor
	if (ren->GetActors()->GetNumberOfItems() < 1)
	{
		vtkErrorMacro(<< "no actors found for writing Fbx file.");
		return;
	}

	// Create subdir in temp dir
	QDir tempDir = QDir::temp();
	tempDir.mkdir("fbx_exporter");
	bool isTemp = tempDir.cd("fbx_exporter");
	if(!isTemp)
		cout << "Error: could not create temporary directory to write textures to: "
		     << tempDir.path().toStdString() << endl;

	// do the actors now
	vtkActor *anActor, *aPart;
	vtkActorCollection *ac = ren->GetActors();
	ac->PrintSelf(std::cout, vtkIndent());
	vtkAssemblyPath *apath;
	vtkCollectionSimpleIterator ait;

	int count = 0;
	for (ac->InitTraversal(ait); (anActor = ac->GetNextActor(ait)); )
	{
		for (anActor->InitPathTraversal(); (apath=anActor->GetNextPath()); )
		{
			if (anActor->GetMapper() != NULL && anActor->GetVisibility() != 0)
			{
				aPart=dynamic_cast<vtkActor *>(apath->GetLastNode()->GetViewProp());
				if(!aPart)
					continue;
				VtkFbxConverter converter(aPart, lScene);
				if(isTemp)
					converter.setTempDirectory(tempDir.path().append(QDir::separator()).toStdString());
				if(converter.convert(VtkFbxHelper::extractBaseNameWithoutExtension(this->FileName), count, this->MaxPoints))
				{
					FbxNode* node = converter.getNode();

					if (node != NULL)
					{
						lScene->GetRootNode()->AddChild(node);
						++count;
					}
				}
			}
		}
	}
	cout << "Fbx converter starts writing file with " << count << " objects." << endl;
	// Possible values for file format:
	//  - FBX 6.0 binary (*.fbx) : works
	//  - FBX 6.0 ascii (*.fbx) : works
	//  - FBX binary (*.fbx) : Out of disk space error
	//  - FBX ascii (*.fbx) : Out of disk space error
	int lFormat;
	if(this->BinaryMode)
	 	lFormat = lSdkManager->GetIOPluginRegistry()->FindWriterIDByDescription("FBX 6.0 binary (*.fbx)");
	 else
	 	lFormat = lSdkManager->GetIOPluginRegistry()->FindWriterIDByDescription("FBX 6.0 ascii (*.fbx)");
	bool saveExitCode = SaveScene(lSdkManager, lScene, this->FileName, lFormat, true);
	cout << "Fbx converter finished writing with exit code: " << saveExitCode << endl;

	lScene->Clear();

	// Cleanup temp texture files
	if(isTemp)
	{
		tempDir.setNameFilters(QStringList() << "*.*");
		tempDir.setFilter(QDir::Files);
		foreach(QString dirFile, tempDir.entryList())
			tempDir.remove(dirFile);
	}
}

void vtkFbxExporter::PrintSelf(ostream& os, vtkIndent indent)
{
	this->Superclass::PrintSelf(os,indent);

	if (this->FileName)
		os << indent << "FileName: " << this->FileName << "\n";
	else
		os << indent << "FileName: (null)\n";
	os << indent << "BinaryMode: " << this->BinaryMode << "\n";
}
