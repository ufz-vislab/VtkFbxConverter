/**
 * \file TestSimpleTexture.cpp
 * 2012-04-24 LB Initial implementation
 * 
 * Tests a conversion of a simple textured cube.
 */

// ** INCLUDES **
#include <gtest.h>
#include <fbxsdk.h>

#include "FbxTestFixture.h"
#include "Common.h"
#include "VtkFbxConverter.h"
#include "VtkFbxHelper.h"

#include <vtkPNGReader.h>
#include <vtkPNGWriter.h>
#include <vtkImageData.h>
#include <vtkTexture.h>
#include <vtkActor.h>

extern char* g_dataPath;

TEST_F(FbxTestFixture, SimpleTexture)
{
	std::cout << "Test data path is " << g_dataPath << std::endl;
	std::string dataPath(g_dataPath);
	vtkActor* actor = VtkFbxHelper::readVtkFile(dataPath + std::string("/cube.vtp"));

	vtkPNGReader* pngReader = vtkPNGReader::New();
	pngReader->SetFileName((dataPath + std::string("/color-vertical.png")).c_str());
	pngReader->Update();
	vtkTexture* texture = vtkTexture::New();
	texture->SetInputConnection(pngReader->GetOutputPort());
	actor->SetTexture(texture);

	VtkFbxConverter converter(actor, _scene);
	converter.convert();
	FbxNode* node = converter.getNode();

	FbxMesh* mesh = node->GetMesh();
	if (node != NULL)
		_scene->GetRootNode()->AddChild(node);
	// Embed media files
	//(*(_sdkManager->GetIOSettings())).SetBoolProp(EXP_FBX_EMBEDDED, true);
	int fileFormat = _sdkManager->GetIOPluginRegistry()->FindWriterIDByDescription( "FBX binary (*.fbx)" );;
	std::cout << "File format is " << fileFormat << std::endl;
	SaveScene(_sdkManager, _scene, (dataPath + std::string("/out.fbx")).c_str(), fileFormat, true);

	ASSERT_EQ(24, mesh->GetControlPointsCount());
	ASSERT_EQ(24, mesh->GetLayer(0)->GetNormals()->GetDirectArray().GetCount());
	ASSERT_EQ(24, mesh->GetLayer(0)->GetUVs()->GetDirectArray().GetCount());
	ASSERT_EQ(NULL, mesh->GetElementVertexColor());
	ASSERT_EQ(6, mesh->GetPolygonCount());

	// FbxTexture* fbxTexture = node->GetMaterial(0)->GetSrcObject<FbxTexture>(0);

}