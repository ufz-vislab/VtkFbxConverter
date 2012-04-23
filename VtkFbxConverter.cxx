/**
 * \file vtkOsgConverter.cpp
 * 2012-04-20 LB Initial implementation
 *
 * Implementation of vtkFbxConverter class
 */

// ** INCLUDES **
#include "VtkFbxConverter.h"

#include <vtkActor.h>
#include <vtkCellArray.h>
#include <vtkCellData.h>
#include <vtkCompositeDataGeometryFilter.h>
#include <vtkDataArray.h>
#include <vtkDataSet.h>
#include <vtkDataSetMapper.h>
#include <vtkGeometryFilter.h>
#include <vtkImageData.h>
#include <vtkMapper.h>
#include <vtkPointData.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkSmartPointer.h>
#include <vtkTexture.h>
#include <vtkUnsignedCharArray.h>
#include <vtkCellDataToPointData.h>
#include <vtkLookupTable.h>
#include <vtkDiscretizableColorTransferFunction.h>
#include <vtkPNGWriter.h>
#include <vtkProperty.h>
#include <vtkLookupTable.h>

#include <fbxsdk.h>

VtkFbxConverter::VtkFbxConverter(vtkActor* actor, FbxScene* scene)
: _actor(actor), _scene(scene)
{

}

VtkFbxConverter::~VtkFbxConverter()
{
	delete _node;
}

FbxNode* VtkFbxConverter::getNode() const
{
	return _node;
}

bool VtkFbxConverter::convert()
{
	// dont export when not visible
	if (_actor->GetVisibility() == 0)
		return false;

	vtkPolyData* pd = getPolyData();
	if (pd == NULL)
		return false;
	vtkPointData* pntData = pd->GetPointData();

	FbxMesh* mesh = FbxMesh::Create(_scene, "mesh_name");

	// -- Vertices --
	vtkIdType numVertices = pd->GetNumberOfPoints();
	if (numVertices == 0)
		return false;
	mesh->InitControlPoints(numVertices);
	FbxVector4* controlPoints = mesh->GetControlPoints();
	for (int i = 0; i < numVertices; i++)
	{
		double* aVertex = pd->GetPoint(i);
		controlPoints[i] = FbxVector4(aVertex[0], aVertex[1], aVertex[2]);
	}

	// -- Normals --
	vtkDataArray* vtkNormals = NULL;
	// TODO: normals on cell data: pd->GetCellData()->GetNormals();
	vtkNormals = pntData->GetNormals();
	if (vtkNormals == NULL)
		return false;

	// Set the normals on Layer 0.
    FbxLayer* layer = mesh->GetLayer(0);
    if (layer == NULL)
    {
        mesh->CreateLayer();
        layer = mesh->GetLayer(0);
    }

    // We want to have one normal for each vertex (or control point),
    // so we set the mapping mode to eByControlPoint.
    FbxLayerElementNormal* layerElementNormal= FbxLayerElementNormal::Create(mesh, "");

    layerElementNormal->SetMappingMode(FbxLayerElement::eByControlPoint);
	vtkIdType numNormals = vtkNormals->GetNumberOfTuples();
	for (int i = 0; i < numNormals; i++)
	{
		double* aNormal = vtkNormals->GetTuple(i);
		layerElementNormal->GetDirectArray().Add(FbxVector4(aNormal[0], aNormal[1], aNormal[2]));
	}

	layer->SetNormals(layerElementNormal);

	// -- Texture coordinates --
	vtkDataArray* vtkTexCoords = pntData->GetTCoords();
	if (vtkTexCoords != NULL)
	{
		// Create UV for Diffuse channel.
		FbxLayerElementUV* lUVDiffuseLayer = FbxLayerElementUV::Create(mesh, "DiffuseUV");
		lUVDiffuseLayer->SetMappingMode(FbxLayerElement::eByPolygonVertex);
		lUVDiffuseLayer->SetReferenceMode(FbxLayerElement::eIndexToDirect);
		layer->SetUVs(lUVDiffuseLayer, FbxLayerElement::eTextureDiffuse);

		int numCoords = vtkTexCoords->GetNumberOfTuples();
		for (int i = 0; i < numCoords; i++)
		{
			double texCoords[3];
			vtkTexCoords->GetTuple(i, texCoords);
			lUVDiffuseLayer->GetDirectArray().Add(FbxVector2(texCoords[1], texCoords[0])); // TODO: ordering?
		}

		//Now we have set the UVs as eIndexToDirect reference and in eByPolygonVertex  mapping mode
		//we must update the size of the index array.
		lUVDiffuseLayer->GetIndexArray().SetCount(numVertices);
		for (int i = 0; i < numVertices; i++)
			lUVDiffuseLayer->GetIndexArray().SetAt(i, i);
	}

	// -- Vertex Colors --
	FbxGeometryElementVertexColor* vertexColorElement = mesh->CreateElementVertexColor();
	vertexColorElement->SetMappingMode(FbxGeometryElement::eByControlPoint);
	vertexColorElement->SetReferenceMode(FbxGeometryElement::eDirect);

	vtkUnsignedCharArray* vtkColors = this->getColors(pd);
	vtkIdType numColors = vtkColors->GetNumberOfTuples();

	cout << "Num colors: " << numColors << std::endl;

	unsigned char aColor[4];
	for (int i = 0; i < numColors; i++)
	{
		vtkColors->GetTupleValue(i, aColor);
		float r = ((float) aColor[0]) / 255.0f;
		float g = ((float) aColor[1]) / 255.0f;
		float b = ((float) aColor[2]) / 255.0f;
		vertexColorElement->GetDirectArray().Add(FbxColor(r, g, b, 1.0));
	}

	// -- Polygons --
	vtkCellArray* pCells;
	vtkIdType npts, * pts;
	int prim = 0;

	pCells = pd->GetPolys();
	if (pCells->GetNumberOfCells() > 0)
	{
		for (pCells->InitTraversal(); pCells->GetNextCell(npts, pts);
		     prim++)
		{
			mesh->BeginPolygon(-1, -1, -1, false);
			for (int i = 0; i < npts; i++)
				mesh->AddPolygon(pts[i]);
			mesh->EndPolygon();
		}
	}

	// -- Node --
	_node = FbxNode::Create(_scene, "node_name");
	_node->SetNodeAttribute(mesh);

	// -- Material --
	_node->AddMaterial(this->getMaterial());

	return true;
}

vtkPolyData* VtkFbxConverter::getPolyData()
{
	vtkMapper* actorMapper = _actor->GetMapper();
	// see if the actor has a mapper. it could be an assembly
	if (actorMapper == NULL)
		return NULL;

	vtkDataObject* inputDO = actorMapper->GetInputDataObject(0, 0);
	if (inputDO == NULL)
		return NULL;

	// Get PolyData. Convert if necessary becasue we only want polydata
	vtkSmartPointer<vtkPolyData> pd;
	if(inputDO->IsA("vtkCompositeDataSet"))
	{
		vtkCompositeDataGeometryFilter* gf = vtkCompositeDataGeometryFilter::New();
		gf->SetInput(inputDO);
		gf->Update();
		pd = gf->GetOutput();
		gf->Delete();
	}
	else if(inputDO->GetDataObjectType() != VTK_POLY_DATA)
	{
		vtkGeometryFilter* gf = vtkGeometryFilter::New();
		gf->SetInput(inputDO);
		gf->Update();
		pd = gf->GetOutput();
		gf->Delete();
	}
	else
		pd = static_cast<vtkPolyData*>(inputDO);

	return pd;
}

FbxTexture* VtkFbxConverter::getTexture()
{
	// -- Texture --
	vtkTexture* texture = _actor->GetTexture();
	if (!texture)
		return NULL;

//	vtkImageData* imageData = texture->GetInput();
//	int imageDims[3];
//	imageData->GetDimensions(imageDims);
//
//	vtkPointData* imagePointData = imageData->GetPointData();
//	vtkCellData* imageCellData = imageData->GetCellData();
//
//	vtkDataArray* imageDataArray = NULL;
//
//	if (imagePointData)
//		imageDataArray = imagePointData->GetScalars();
//	if (!imageDataArray && imageCellData)
//		imageDataArray = imageCellData->GetScalars();
//
//	if (imageDataArray)
//		return NULL;
//
//	int imageNumComponents = imageDataArray->GetNumberOfComponents();
//	int imageNumPixels = imageDataArray->GetNumberOfTuples();
//
//	if (imageNumPixels != (imageDims[0] * imageDims[1] * imageDims[2]))
//		return NULL;

	vtkPNGWriter* pngWriter = vtkPNGWriter::New();
	pngWriter->SetInputConnection(texture->GetOutputPort());
	pngWriter->SetFileName("vtkTexture.png");
	pngWriter->Write();

	FbxFileTexture* fbxTexture = FbxFileTexture::Create(_scene,"vtkTexture.png");
	fbxTexture->SetTextureUse(FbxTexture::eStandard);
	fbxTexture->SetMappingType(FbxTexture::eUV);
	fbxTexture->SetMaterialUse(FbxFileTexture::eModelMaterial);
	fbxTexture->SetAlphaSource (FbxTexture::eBlack);

	return fbxTexture;
}

FbxSurfacePhong* VtkFbxConverter::getMaterial()
{
	vtkProperty* prop = _actor->GetProperty();
	double* diffuseColor = prop->GetDiffuseColor();
	double* ambientColor = prop->GetAmbientColor();
	double* specularColor = prop->GetSpecularColor();
	double specularPower = prop->GetSpecularPower();
	double diffuse = prop->GetDiffuse();
	double ambient = prop->GetAmbient();
	double specular = prop->GetSpecular();
	double opacity = prop->GetOpacity();

    FbxSurfacePhong* material = FbxSurfacePhong::Create(_scene, "MaterialName");

    // Generate primary and secondary colors.
    material->Emissive.Set(FbxDouble3(0.0, 0.0, 0.0));
    material->Ambient.Set(FbxDouble3(ambientColor[0],
		ambientColor[1], ambientColor[2]));
    material->AmbientFactor.Set(ambient);

	// Add texture for diffuse channel
	FbxTexture* texture = this->getTexture();
	if (texture)
		material->Diffuse.ConnectSrcObject(texture);
	else
		material->Diffuse.Set(FbxDouble3(diffuseColor[0],
			diffuseColor[1], diffuseColor[2]));

    material->DiffuseFactor.Set(diffuse);
    material->TransparencyFactor.Set(opacity);
    material->ShadingModel.Set("Phong");
    material->Shininess.Set(specularPower);
    material->Specular.Set(FbxDouble3(specularColor[0],
		specularColor[1], specularColor[2]));
    material->SpecularFactor.Set(specular);

    return material;
}

vtkUnsignedCharArray* VtkFbxConverter::getColors(vtkPolyData* pd)
{
	vtkMapper* actorMapper = _actor->GetMapper();
	// Get the color range from actors lookup table
	double range[2];
	vtkLookupTable* actorLut = static_cast<vtkLookupTable*>(actorMapper->GetLookupTable());
	actorLut->GetTableRange(range);

	// Copy mapper to a new one
	vtkPolyDataMapper* pm = vtkPolyDataMapper::New();
	// Convert cell data to point data
	// NOTE: Comment this out to export a mesh
	if (actorMapper->GetScalarMode() == VTK_SCALAR_MODE_USE_CELL_DATA ||
		actorMapper->GetScalarMode() == VTK_SCALAR_MODE_USE_CELL_FIELD_DATA)
	{
		vtkCellDataToPointData* cellDataToPointData = vtkCellDataToPointData::New();
		cellDataToPointData->PassCellDataOff();
		cellDataToPointData->SetInput(pd);
		cellDataToPointData->Update();
		pd = cellDataToPointData->GetPolyDataOutput();
		cellDataToPointData->Delete();

		pm->SetScalarMode(VTK_SCALAR_MODE_USE_POINT_DATA);
	}
	else
		pm->SetScalarMode(actorMapper->GetScalarMode());

	pm->SetInput(pd);
	pm->SetScalarVisibility(actorMapper->GetScalarVisibility());

	vtkLookupTable* lut = NULL;
	// ParaView Exporter
	if (dynamic_cast<vtkDiscretizableColorTransferFunction*>(actorMapper->GetLookupTable()))
		lut = actorLut;
	// Clone the lut in OGS because otherwise the original lut gets destroyed
	else
	{
		lut = vtkLookupTable::New();
		lut->DeepCopy(actorLut);
		lut->Build();
	}
	pm->SetLookupTable(lut);
	pm->SetScalarRange(range);
	pm->Update();

	if(pm->GetScalarMode() == VTK_SCALAR_MODE_USE_POINT_FIELD_DATA ||
	   pm->GetScalarMode() == VTK_SCALAR_MODE_USE_CELL_FIELD_DATA )
	{
		if(actorMapper->GetArrayAccessMode() == VTK_GET_ARRAY_BY_ID )
			pm->ColorByArrayComponent(actorMapper->GetArrayId(),
			                          actorMapper->GetArrayComponent());
		else
			pm->ColorByArrayComponent(actorMapper->GetArrayName(),
			                          actorMapper->GetArrayComponent());
	}

	return pm->MapScalars(1.0);
}

