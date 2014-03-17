#author: Carolin Helbig 2013-07-23, Lars Bilke 2014-03-17
import os
try: paraview.simple
except: from paraview.simple import *
paraview.simple._DisableFirstRenderCameraReset()

exporters=servermanager.createModule("exporters")

view = GetActiveView()
objects = GetSources()

# Open a directory dialog in users home directory
# For Windows binaries see http://www.riverbankcomputing.com/software/pyqt/download
import PyQt4.QtGui
dir = str(PyQt4.QtGui.QFileDialog.getExistingDirectory(None, "Select a directory", PyQt4.QtCore.QDir.home().canonicalPath()))

for tuple, object in objects.items():
	name = tuple[0]
	index = tuple[1]
	#print '{0} has id {1}'.format(name, index)
	displayProperties = GetDisplayProperties(object)
	if not displayProperties.Visibility:
		continue
	print 'Exporting {0}'.format(name)
	filename = os.path.join(dir, '{0}.fbx'.format(name))
	exporter = exporters.FbxExporter(FileName = filename)
	exporter.SetView(view)
	exporter.Write()
