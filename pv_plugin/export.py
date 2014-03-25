#author: Carolin Helbig 2013-07-23, Lars Bilke 2014-03-17
import os
import PyQt4.QtGui
try: paraview.simple
except: from paraview.simple import *
paraview.simple._DisableFirstRenderCameraReset()

exporters=servermanager.createModule("exporters")

view = GetActiveView()
objects = GetSources()
reader = GetActiveSource()
tsteps = [view.ViewTime] # Initialize time steps array with the current time
if hasattr(reader, 'TimestepValues'):
	tsteps = reader.TimestepValues

# Open a directory dialog in users home directory
# For Windows binaries see http://www.riverbankcomputing.com/software/pyqt/download
dir = str(PyQt4.QtGui.QFileDialog.getExistingDirectory(None, "Select a directory", PyQt4.QtCore.QDir.home().canonicalPath()))

# Store visibility of all objects in visArray
visArray = []
for tmpTuple, tmpObject in objects.items():
	dp = GetDisplayProperties(tmpObject)
	visArray.append(dp.Visibility)
	dp.Visibility = 0

i = 0
for tuple, object in objects.items():
	i = i + 1
	if visArray[i-1] == 0:
		continue

	name = tuple[0]
	index = tuple[1]
	#print '{0} has id {1}'.format(name, index)

	# Enable visibility for current object
	displayProperties = GetDisplayProperties(object)
	displayProperties.Visibility = 1

	# Iterate over time steps
	for index in range(len(tsteps)):
		view.ViewTime = tsteps[index]
		tsteps_string = ''
		if len(tsteps) > 1:
			tsteps_string = '_ts_{0}'.format(index)
		print 'Exporting {0}, timestep {1}'.format(name, index)
		filename = os.path.join(dir, '{0}{1}.fbx'.format(name, tsteps_string))

		exporter = exporters.FbxExporter(FileName = filename)
		exporter.SetView(view)
		exporter.Write()

	# Disable visibility for current object
	displayProperties.Visibility = 0

# Re-enable visibility to the state before export
i = 0
for tmpTuple, tmpObject in objects.items():
	i = i + 1
	dp = GetDisplayProperties(tmpObject)
	dp.Visibility = visArray[i-1]
