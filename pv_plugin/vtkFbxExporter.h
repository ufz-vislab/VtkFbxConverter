/**
 * \file vtkFbxExporter.h
 * 2012-06-01 LB Initial implementation
 */

#ifndef __vtkFbxExporter_h
#define __vtkFbxExporter_h

#include "vtkExporter.h"

class vtkFbxExporter : public vtkExporter
{
public:
  static vtkFbxExporter *New();
  vtkTypeRevisionMacro(vtkFbxExporter,vtkExporter);
  
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // Specify the name of the VRML file to write.
  vtkSetStringMacro(FileName);
  vtkGetStringMacro(FileName);
  
protected:
  vtkFbxExporter();
  ~vtkFbxExporter();

  void WriteData();
  char *FileName;
  
private:
  vtkFbxExporter(const vtkFbxExporter&);  // Not implemented.
  void operator=(const vtkFbxExporter&);  // Not implemented.
};

#endif
