/**
 * \file testrunner.cpp
 * 29/4/2010 LB Initial implementation
 * 
 * Implementation of the googletest testrunner
 */

// ** INCLUDES **
#include <gtest.h>

char* g_dataPath;

int main(int argc, char* argv[])
{
	testing::InitGoogleTest ( &argc, argv );
	if (argc > 1)
		g_dataPath = argv[1];
	return RUN_ALL_TESTS();
}
