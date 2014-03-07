/**
 * \file VtkFbxConverter.h
 * 2012-04-20 LB Initial implementation
 *
 */

#ifndef VTKFBXCONVERTER_H
#define VTKFBXCONVERTER_H

#include <fbxsdk/fbxsdk_version.h>
#include <string>
#include <vtkSmartPointer.h>

class vtkActor;
class vtkPolyData;
class vtkUnsignedCharArray;
class vtkTexture;
class vtkProperty;
class vtkCellArray;

namespace FBXSDK_NAMESPACE {
	class FbxNode;
	class FbxScene;
	class FbxTexture;
	class FbxSurfacePhong;
	class FbxMesh;
	class FbxColor;
}

class VtkFbxConverter
{
public:
	VtkFbxConverter(vtkActor* actor, FBXSDK_NAMESPACE::FbxScene* scene);
	virtual ~VtkFbxConverter();

	bool convert(std::string name = "FBXObject", int index = 0);
	FBXSDK_NAMESPACE::FbxNode* getNode() const;

	void addUserProperty(const std::string name, const bool value);
	void addUserProperty(const std::string name, const float value);
	void addUserProperty(const std::string name, const int value);
	void addUserProperty(const std::string name, const std::string value);
	void addUserProperty(const std::string name, const FBXSDK_NAMESPACE::FbxColor value);
	//void addUserProperty(FBXSDK_NAMESPACE::FbxNode *node, const std::string name, const FBXSDK_NAMESPACE::MyFbxVector4 value);

protected:
	vtkPolyData* getPolyData();
	FBXSDK_NAMESPACE::FbxTexture* getTexture(vtkTexture* texture,
		FBXSDK_NAMESPACE::FbxScene* scene);
	FBXSDK_NAMESPACE::FbxSurfacePhong* getMaterial(vtkProperty* prop, vtkTexture* texture,
		bool scalarVisibility, FBXSDK_NAMESPACE::FbxScene* scene);
	unsigned int createMeshStructure(vtkSmartPointer<vtkCellArray> cells,
	                                 FBXSDK_NAMESPACE::FbxMesh* mesh,
	                                 const bool flipOrdering = false) const;

private:
	vtkActor* _actor;
	FBXSDK_NAMESPACE::FbxNode* _node;
	FBXSDK_NAMESPACE::FbxScene* _scene;
	std::string _name;
	int _index;
	std::string _indexString;
};

#endif // VTKFBXCONVERTER_H
