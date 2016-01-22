
#include "gtest/gtest.h"
#include "curly.h"
#include "Helpers.h"



class putFixture : public ::testing::Test
{
public:
	putFixture() {}
	~putFixture() {}

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

int putFixture::lastHttpRequestStatus = 0;
int putFixture::completedSuccessfullTransfers = 0;
int putFixture::completedFailedTransfers = 0;

TEST_F(putFixture, singlePut)
{
	char* data = "testput";
	curly_http_transaction_handle handle = curly_http_put("http://httpbin.org/put", data, strlen(data), &onHttpRequestCompleted);
	WAITUNTIL(3000, lastHttpRequestStatus == 200);
}

TEST_F(putFixture, multiPut)
{
	int NUMBER_OF_PUTS = 1000;
	char* data = "testput";
	for (int i = 0; i < NUMBER_OF_PUTS; i++) {
		curly_http_transaction_handle handle = curly_http_put("http://httpbin.org/put", data, strlen(data), &onHttpRequestCompleted);
	}
	WAITUNTIL(35000, completedSuccessfullTransfers == NUMBER_OF_PUTS);
}