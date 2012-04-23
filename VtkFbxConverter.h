/**
 * \file VtkFbxConverter.h
 * 2012-04-20 LB Initial implementation
 *
 */

#ifndef VTKFBXCONVERTER_H
#define VTKFBXCONVERTER_H

 #include <fbxsdk/fbxsdk_version.h>

class vtkActor;
class vtkPolyData;
class vtkUnsignedCharArray;

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

	bool convert();
	FBXSDK_NAMESPACE::FbxNode* getNode() const;

protected:
	vtkPolyData* getPolyData();
	FBXSDK_NAMESPACE::FbxTexture* getTexture();
	FBXSDK_NAMESPACE::FbxSurfacePhong* getMaterial();
	vtkUnsignedCharArray* getColors(vtkPolyData* pd);

private:
	vtkActor* _actor;
	FBXSDK_NAMESPACE::FbxNode* _node;
	FBXSDK_NAMESPACE::FbxScene* _scene;
};

#endif // VTKFBXCONVERTER_H