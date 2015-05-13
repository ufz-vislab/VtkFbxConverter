INCLUDE_DIRECTORIES (${CMAKE_SOURCE_DIR} ${GTEST_INCLUDE_DIRS} ${GTEST_INCLUDE_DIRS}/gtest)

ADD_EXECUTABLE (testrunner testrunner.cxx
	FbxTestFixture.h FbxTestFixture.cxx
	TestSimpleTexture.cxx
	TestColoredMesh.cxx
)

TARGET_LINK_LIBRARIES (testrunner VtkFbxConverter ${GTEST_LIBRARY} ${LINK_LIBRARIES})

ADD_CUSTOM_TARGET (tests ${CMAKE_CURRENT_BINARY_DIR}/testrunner ${CMAKE_SOURCE_DIR}/data
	DEPENDS testrunner)