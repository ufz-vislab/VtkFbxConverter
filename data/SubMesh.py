try: paraview.simple
except: from paraview.simple import *
paraview.simple._DisableFirstRenderCameraReset()

Box1 = Box()

RenderView1 = GetRenderView()

DataRepresentation1 = Show()
DataRepresentation1.ScaleFactor = 0.1
DataRepresentation1.SelectionPointFieldDataArrayName = 'Normals'
DataRepresentation1.EdgeColor = [0.0, 0.0, 0.5000076295109483]

RenderView1.CameraPosition = [0.0, 0.0, 3.964592284436349]
RenderView1.CameraClippingRange = [2.9299463615919854, 5.276561168702894]
RenderView1.CameraParallelScale = 1.0366549443070232

Elevation1 = Elevation()

Elevation1.LowPoint = [-0.5, -0.5, -0.5]
Elevation1.HighPoint = [0.5, 0.5, 0.5]

# toggle the 3D widget visibility.
active_objects.source.SMProxy.InvokeEvent('UserEvent', 'ShowWidget')
RenderView1.CameraClippingRange = [2.900576837470701, 5.318021827312761]

DataRepresentation2 = Show()
DataRepresentation2.EdgeColor = [0.0, 0.0, 0.5000076295109483]
DataRepresentation2.SelectionPointFieldDataArrayName = 'Elevation'
DataRepresentation2.ColorArrayName = ('POINT_DATA', 'Elevation')
DataRepresentation2.ScaleFactor = 0.1

a1_Elevation_PVLookupTable = GetLookupTableForArray( "Elevation", 1, RGBPoints=[0.0, 0.23, 0.299, 0.754, 0.5, 0.865, 0.865, 0.865, 1.0, 0.706, 0.016, 0.15], VectorMode='Magnitude', NanColor=[0.25, 0.0, 0.0], ColorSpace='Diverging', ScalarRangeInitialized=1.0 )

a1_Elevation_PiecewiseFunction = CreatePiecewiseFunction( Points=[0.0, 0.0, 0.5, 0.0, 1.0, 1.0, 0.5, 0.0] )

DataRepresentation1.Visibility = 0

DataRepresentation2.LookupTable = a1_Elevation_PVLookupTable

a1_Elevation_PVLookupTable.ScalarOpacityFunction = a1_Elevation_PiecewiseFunction

Sphere1 = Sphere()

DataRepresentation3 = Show()
DataRepresentation3.ScaleFactor = 0.1
DataRepresentation3.SelectionPointFieldDataArrayName = 'Normals'
DataRepresentation3.EdgeColor = [0.0, 0.0, 0.5000076295109483]

RenderView1.CameraClippingRange = [2.9299463615919854, 5.276561168702894]

Transform1 = Transform( Transform="Transform" )

Transform1.Transform = "Transform"

# toggle the 3D widget visibility.
active_objects.source.SMProxy.InvokeEvent('UserEvent', 'ShowWidget')
DataRepresentation4 = Show()
DataRepresentation4.ScaleFactor = 0.1
DataRepresentation4.SelectionPointFieldDataArrayName = 'Normals'
DataRepresentation4.EdgeColor = [0.0, 0.0, 0.5000076295109483]

DataRepresentation3.Visibility = 0

Transform1.Transform.Translate = [1.0, 0.0, 0.0]

DataRepresentation4.Opacity = 0.5
DataRepresentation4.DiffuseColor = [0.8192721446555276, 0.10840009155413138, 0.14145113298237583]

Render()
