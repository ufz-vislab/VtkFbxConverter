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
  vtkTypeMacro(vtkFbxExporter,vtkExporter);

  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // Specify the name of the VRML file to write.
  vtkSetStringMacro(FileName);
  vtkGetStringMacro(FileName);

  // Description:
  // Specify binary writing mode.
  vtkSetMacro(BinaryMode, bool);
  vtkGetMacro(BinaryMode, bool);
  vtkBooleanMacro(BinaryMode, bool);

  void SetUseVertexColors(int value) {UseVertexColors = value;}

protected:
  vtkFbxExporter();
  ~vtkFbxExporter();

  void WriteData();
  char *FileName;
  bool BinaryMode;
  int UseVertexColors;

private:
  vtkFbxExporter(const vtkFbxExporter&);  // Not implemented.
  void operator=(const vtkFbxExporter&);  // Not implemented.
};

#endif
