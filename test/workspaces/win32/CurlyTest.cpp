// CurlyTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "gtest/gtest.h"
//#include "filter.h"

int _tmain(int argc, _TCHAR* argv[])
{
	::testing::GTEST_FLAG(filter) = "putFixture.multiPut";
	testing::InitGoogleTest(&argc, argv);
	RUN_ALL_TESTS();
	// keep console window open until Return keystroke}
#ifdef __GNUC__
	getchar();
#else
	int res = std::getchar();
#endif
	return 0;
}

