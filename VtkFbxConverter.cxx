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