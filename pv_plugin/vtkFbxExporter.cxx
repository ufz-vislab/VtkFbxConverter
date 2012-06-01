/**
 * \file vtkFbxExporter.cxx
 * 2012-06-01 LB Initial implementation
 * 
 * Implementation of vtkFbxExporter class
 */

// ** INCLUDES **
#include "vtkFbxExporter.h"
#include "vtkFbxConverter.h"

#include "vtkAssemblyNode.h"
#include "vtkAssemblyPath.h"
#include "vtkObjectFactory.h"
#include "vtkRendererCollection.h"
#include "vtkRenderWindow.h"

#include <fbxsdk.h>

#include "../Common.h"

extern FbxManager* lSdkManager;
extern FbxScene* lScene;

vtkStandardNewMacro(vtkFbxExporter);
vtkCxxRevisionMacro(vtkFbxExporter, "$Revision$");

vtkFbxExporter::vtkFbxExporter()
{
	this->DebugOn();
	this->FileName = NULL;
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
	int count = 0;
	for (ac->InitTraversal(ait); (anActor = ac->GetNextActor(ait)); )
  	{
		for (anActor->InitPathTraversal(); (apath=anActor->GetNextPath()); )
		{
			aPart=static_cast<vtkActor *>(apath->GetLastNode()->GetViewProp());
			if (aPart->GetMapper() != NULL && aPart->GetVisibility() != 0)
	  		{
	  			VtkFbxConverter converter(aPart, lScene);
        		converter.convert();
        		FbxNode* node = converter.getNode();

       			if (node != NULL)
       			{
            		lScene->GetRootNode()->AddChild(node);
            		++count;
            	}
	  		}
		}
  	} 
	vtkDebugMacro(<< "Fbx converter starts writing file with " << count << " objects.");
	(*(lSdkManager->GetIOSettings())).SetBoolProp(EXP_FBX_EMBEDDED, true);
	bool lResult = SaveScene(lSdkManager, lScene, this->FileName);

	// if(lResult == false)
	// 	vtkDebugMacro(<< "An error occurred while saving the scene...");
 //    else
 //    	vtkDebugMacro(<< "Fbx converter finished.");

    // TODO: Remove nodes from scene for the next export???
	
	// OSG::SceneFileHandler::the().write(rootNode, this->FileName);
}

void vtkFbxExporter::PrintSelf(ostream& os, vtkIndent indent)
{
	this->Superclass::PrintSelf(os,indent);

	if (this->FileName)
		os << indent << "FileName: " << this->FileName << "\n";
	else
		os << indent << "FileName: (null)\n";
}
