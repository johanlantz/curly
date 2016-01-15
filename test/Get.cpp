
#include "gtest/gtest.h"

class getFixture : public ::testing::Test
{
public:
	getFixture()
	{
	}

	void SetUp()
	{
	
	}

	void TearDown()
	{
	
	}

	~getFixture()
	{
	}
};


TEST_F(getFixture, singleGet)
{
	int t = 0;
	t++;
}