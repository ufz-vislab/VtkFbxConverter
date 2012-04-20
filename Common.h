/****************************************************************************************

   Copyright (C) 2012 Autodesk, Inc.
   All rights reserved.

   Use of this software is subject to the terms of the Autodesk license agreement
   provided at the time of installation or download, or which otherwise accompanies
   this software in either electronic or hard copy form.

****************************************************************************************/
#ifndef _COMMON_H
#define _COMMON_H

#include <fbxsdk.h>
void InitializeSdkObjects(FbxManager*& pSdkManager, FbxScene*& pScene);
void DestroySdkObjects(FbxManager* pSdkManager);
void CreateAndFillIOSettings(FbxManager* pSdkManager);

bool SaveScene(FbxManager* pSdkManager, FbxDocument* pScene, const char* pFilename, int pFileFormat=-1, bool pEmbedMedia=false);
bool LoadScene(FbxManager* pSdkManager, FbxDocument* pScene, const char* pFilename);

#endif // #ifndef _COMMON_H


