
#include "FbxTestFixture.h"
#include "Common.h"

#include <fbxsdk.h>

FbxTestFixture::FbxTestFixture()
: _scene(NULL), _sdkManager(NULL)
{
    InitializeSdkObjects(_sdkManager, _scene);
}

FbxTestFixture::~FbxTestFixture()
{
	// TODO: Crashes!
	// DestroySdkObjects(_sdkManager);
}