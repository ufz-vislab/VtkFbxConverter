/**
 * \file VtkFbxConverter.h
 * 2012-04-20 LB Initial implementation
 *
 */

#ifndef VTKFBXCONVERTER_H
#define VTKFBXCONVERTER_H

#include <fbxsdk/fbxsdk_version.h>
#include <string>

class vtkActor;
class vtkPolyData;
class vtkUnsignedCharArray;
class vtkTexture;
class vtkProperty;

namespace FBXSDK_NAMESPACE {
	class FbxNode;
	class FbxScene;
	class FbxTexture;
	class FbxSurfacePhong;
}

class VtkFbxConverter
{
public:
	VtkFbxConverter(vtkActor* actor, FBXSDK_NAMESPACE::FbxScene* scene);
	virtual ~VtkFbxConverter();

	bool convert(std::string name = "FBXObject");
	FBXSDK_NAMESPACE::FbxNode* getNode() const;

protected:
	vtkPolyData* getPolyData();
	static FBXSDK_NAMESPACE::FbxTexture* getTexture(vtkTexture* texture,
		FBXSDK_NAMESPACE::FbxScene* scene);
	static FBXSDK_NAMESPACE::FbxSurfacePhong* getMaterial(vtkProperty* prop, vtkTexture* texture,
		FBXSDK_NAMESPACE::FbxScene* scene, std::string name = "FBXObject");
	vtkUnsignedCharArray* getColors(vtkPolyData* pd, bool convertCellToPointData = false);

private:
	vtkActor* _actor;
	FBXSDK_NAMESPACE::FbxNode* _node;
	FBXSDK_NAMESPACE::FbxScene* _scene;
};

#endif // VTKFBXCONVERTER_H