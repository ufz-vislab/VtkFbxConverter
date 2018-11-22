/**
 * \file pqFbxStarter.cxx
 * 2012-06-01 LB Initial implementation
 *
 * Implementation of pqFbxStarter class
 */

// ** INCLUDES **
#include "pqFbxStarter.h"

#include <QDebug>

#include "Common.h"

#include <fbxsdk.h>

// Globals
FbxManager* lSdkManager = NULL;
FbxScene* lScene = NULL;

pqFbxStarter::pqFbxStarter(QObject* parent)
  : QObject(parent)
{
}

pqFbxStarter::~pqFbxStarter()
{
}

void pqFbxStarter::onStartup()
{
	// Init fbx
	InitializeSdkObjects(lSdkManager, lScene);

	qWarning() << "Fbx inited.";
}

void pqFbxStarter::onShutdown()
{
	// Destroy all objects created by the FBX SDK.
	DestroySdkObjects(lSdkManager, true); // Crashes??
	qWarning() << "Fbx exited.";
}
