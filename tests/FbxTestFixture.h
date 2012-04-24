/**
 * \file FbxTestFixture.h
 * 2012-04-24 LB Initial implementation
 *
 */

#ifndef FBXTESTFIXTURE_H
#define FBXTESTFIXTURE_H

#include <gtest.h>
#include <fbxsdk/fbxsdk_version.h>

 namespace FBXSDK_NAMESPACE {
	class FbxScene;
	class FbxManager;
}

class FbxTestFixture : public ::testing::Test
{
public:
	FbxTestFixture();
	virtual ~FbxTestFixture();

	FBXSDK_NAMESPACE::FbxScene* _scene;
	FBXSDK_NAMESPACE::FbxManager* _sdkManager;
};

#endif // FBXTESTFIXTURE_H