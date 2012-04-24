INCLUDE_DIRECTORIES (${GTEST_INCLUDE_DIRS}/gtest)

ADD_EXECUTABLE (testrunner testrunner.cxx
	TestSimpleTexture.cxx
)

TARGET_LINK_LIBRARIES (testrunner GTest ${LINK_LIBRARIES})

ADD_CUSTOM_TARGET (tests ${CMAKE_CURRENT_BINARY_DIR}/testrunner)