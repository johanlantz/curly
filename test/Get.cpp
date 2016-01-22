
#include "gtest/gtest.h"
#include "curly.h"
#include "Helpers.h"



class getFixture : public ::testing::Test
{
public:
	getFixture() {}
	~getFixture() {}

	void SetUp()
	{
		lastHttpRequestStatus = 0;
		completedSuccessfullTransfers = 0;
		completedFailedTransfers = 0;
	}

	void TearDown() {}

	static void onHttpRequestCompleted(curly_http_transaction_handle handle, int http_response_code, void* data, int size) {
		lastHttpRequestStatus = http_response_code;
		if (lastHttpRequestStatus == 200) {
			completedSuccessfullTransfers++;
		}
		else {
			completedFailedTransfers++;
		}
	}

protected:
	static int lastHttpRequestStatus;
	static int completedSuccessfullTransfers;
	static int completedFailedTransfers;
};

int getFixture::lastHttpRequestStatus = 0;
int getFixture::completedSuccessfullTransfers = 0;
int getFixture::completedFailedTransfers = 0;

TEST_F(getFixture, singleGet)
{
	curly_http_transaction_handle handle = curly_http_get("http://example.com", &onHttpRequestCompleted);
	WAITUNTIL(3000, lastHttpRequestStatus == 200);
}

TEST_F(getFixture, multiGet)
{
	int NUMBER_OF_GETS = 100;
	for (int i = 0; i < NUMBER_OF_GETS; i++) {
		curly_http_transaction_handle handle = curly_http_get("http://www.dn.se", &onHttpRequestCompleted);
	}
	WAITUNTIL(5000, completedSuccessfullTransfers == NUMBER_OF_GETS);
}