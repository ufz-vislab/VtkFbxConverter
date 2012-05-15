/**
 * \file TestColoredMesh.cpp
 * 2012-05-15 LB Initial implementation
 * 
 * Tests a conversion of a colored 2d mesh.
 */

// ** INCLUDES **
#include <gtest.h>
#include <fbxsdk.h>

#include "FbxTestFixture.h"
#include "Common.h"
#include "VtkFbxConverter.h"
#include "VtkFbxHelper.h"

#include <vtkActor.h>

extern char* g_dataPath;

TEST_F(FbxTestFixture, ColoredMesh)
{
	std::cout << "Test data path is " << g_dataPath << std::endl;
	std::string dataPath(g_dataPath);
	vtkActor* actor = readVtkFile(dataPath + std::string("/colored_mesh.vtu"));

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

	ASSERT_EQ(18597, mesh->GetControlPointsCount());
	ASSERT_EQ(18597, mesh->GetLayer(0)->GetNormals()->GetDirectArray().GetCount());
	ASSERT_EQ(33652, mesh->GetPolygonCount());
	ASSERT_EQ(33652, mesh->GetLayer(0)->GetVertexColors()->GetDirectArray().GetCount());
	ASSERT_EQ(FbxGeometryElement::eByPolygon, mesh->GetLayer(0)->GetVertexColors()->GetMappingMode());
}