/**
 * \file testrunner.cpp
 * 29/4/2010 LB Initial implementation
 * 
 * Implementation of the googletest testrunner
 */

// ** INCLUDES **
#include <gtest.h>

int main(int argc, char* argv[])
{
  testing::InitGoogleTest ( &argc, argv );
  return RUN_ALL_TESTS();
}
