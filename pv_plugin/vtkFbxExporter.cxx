/**
 * \file vtkFbxExporter.cxx
 * 2012-06-01 LB Initial implementation
 *
 * Implementation of vtkFbxExporter class
 */

// ** INCLUDES **
#include "vtkFbxExporter.h"
#include "vtkFbxConverter.h"
#include "vtkFbxHelper.h"

#include "vtkAssemblyNode.h"
#include "vtkAssemblyPath.h"
#include "vtkObjectFactory.h"
#include "vtkRendererCollection.h"
#include "vtkRenderWindow.h"

#include <fbxsdk.h>
#include <QDebug>

#include "../Common.h"

extern FbxManager* lSdkManager;
extern FbxScene* lScene;

vtkStandardNewMacro(vtkFbxExporter);

vtkFbxExporter::vtkFbxExporter()
{
	this->DebugOn();
	this->FileName = NULL;
	this->BinaryMode = true;
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

	// do the actors now
	vtkActor *anActor, *aPart;
	vtkActorCollection *ac = ren->GetActors();
	ac->PrintSelf(std::cout, vtkIndent());
	vtkAssemblyPath *apath;
	vtkCollectionSimpleIterator ait;

	FbxNode* propertiesNode = FbxNode::Create(lScene, "Properties");
	lScene->GetRootNode()->AddChild(propertiesNode);
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
				if(converter.convert(VtkFbxHelper::extractBaseNameWithoutExtension(this->FileName), count))
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
	vtkDebugMacro(<< "Fbx converter starts writing file with " << count << " objects.");
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
	vtkDebugMacro(<< "Fbx converter finished writing with exit code: " << saveExitCode);

	lScene->Clear();
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
