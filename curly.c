#include "http.h"
#if defined(__APPLE__) || defined(_WIN32)
#include "curl/curl.h"
#endif

void start_worker_thread_if_needed();

static CURLM *multi_handle = NULL;
static int no_of_handles_running;

typedef struct {
    char* data;
    int size;
    unsigned size_left;
	void (*on_http_request_completed)(curly_http_transaction_handle handle, int http_response_code, void* data, int size);
    curly_http_transaction_handle* handle;
} curly_http_transaction;

void init_curl_if_needed()
{
	if (multi_handle == NULL) {
		//If we need SSL we probably have to use CURL_GLOBAL_SSL later
#ifdef _WIN32
		curl_global_init(CURL_GLOBAL_WIN32);
#else
		curl_global_init(0);
#endif
		multi_handle = curl_multi_init();
	}
}

void curly_dispose()
{
    curl_multi_cleanup(multi_handle);
    curl_global_cleanup();
	multi_handle = NULL;
}

curly_http_transaction* create_transaction(void* data, int size, void* cb)
{
    curly_http_transaction* transaction = calloc(1, sizeof(curly_http_transaction));
    CURL *curl_handle = NULL;

	init_curl_if_needed();
    transaction->data = malloc(size);
    memcpy(transaction->data, data, size);
    transaction->size = size;
    transaction->size_left = size;
    curl_handle = curl_easy_init();
    if (!curl_handle) {
        free(transaction->data);
        return NULL;
    }
    transaction->handle = curl_handle;
	transaction->on_http_request_completed = cb;
    return transaction;
}

static void cleanup_transaction(curly_http_transaction* transaction) {
	curl_multi_remove_handle(multi_handle, transaction->handle);
	curl_easy_cleanup(transaction->handle);
	if (transaction->data) {
		free(transaction->data);
	}
	free(transaction);
}

static int poll() {
	int numfds = 0;
	CURLMsg *cmsg = NULL;
	CURLcode easy_status = CURLE_OK;
	int msgs_in_queue = 0;
	int res = curl_multi_wait(multi_handle, NULL, 0, 100, &numfds);
	if (res != CURLM_OK) {
		fprintf(stderr, "error: curl_multi_wait() returned %d\n", res);
		 return EXIT_FAILURE;
	}

	curl_multi_perform(multi_handle, &no_of_handles_running);

	while (cmsg = curl_multi_info_read(multi_handle, &msgs_in_queue)) {
		if (cmsg->msg == CURLMSG_DONE) {
			CURL *easy_handle = cmsg->easy_handle;
			curly_http_transaction* transaction = NULL;
			int http_response_code = 0;
			if (cmsg->data.result != CURLE_OK) {
				printf("Error: result != CURLE_OK", cmsg->data.result);
				continue;
			}

			easy_status = curl_easy_getinfo(easy_handle, CURLINFO_PRIVATE, &transaction);
			if (easy_status != CURLE_OK) {
				printf("Error retreiving private pointer");
				continue;
			}

			curl_easy_getinfo(easy_handle, CURLINFO_RESPONSE_CODE, &http_response_code);
			if (transaction->on_http_request_completed) {
				transaction->on_http_request_completed(transaction->handle, http_response_code, transaction->data, transaction->size);
			}
			
			cleanup_transaction(transaction);
		}
	}
	return no_of_handles_running;
}

static size_t write_callback(char *ptr, size_t size, size_t nmemb, void *userdata) {
	size_t realsize = size * nmemb;
	curly_http_transaction *transaction = (curly_http_transaction*)userdata;

	transaction->data = realloc(transaction->data, transaction->size + realsize + 1);
	if (transaction->data == NULL) {
		/* out of memory! */
		printf("not enough memory (realloc returned NULL)\n");
		return 0;
	}

	memcpy(&(transaction->data[transaction->size]), ptr, realsize);
	transaction->size += realsize;
	transaction->data[transaction->size] = '\0';
	//printf("%s", transaction->data);
	return realsize;
}

curly_http_transaction_handle curly_http_get(char* url, void* cb)
{
	CURLcode easy_status = CURLE_OK;
    CURLMcode status = CURLM_OK;
    curly_http_transaction* transaction = create_transaction(NULL, 0, cb);
    CURL *http_get_handle = transaction->handle;

    /* set options */
    curl_easy_setopt(http_get_handle, CURLOPT_URL, url);
#if defined (DEBUG) || defined (_DEBUG)
    curl_easy_setopt(http_get_handle, CURLOPT_VERBOSE, 1L);
#endif

	/* send all data to this function  */
	curl_easy_setopt(http_get_handle, CURLOPT_WRITEFUNCTION, &write_callback);

	/* we pass our 'chunk' struct to the callback function */
	curl_easy_setopt(http_get_handle, CURLOPT_WRITEDATA, (void *)transaction);

	easy_status = curl_easy_setopt(http_get_handle, CURLOPT_PRIVATE, (void*)transaction); 
	if (easy_status != CURLE_OK) {
		printf("Failed setting private data.");
	}

    /* add the individual transfers */
    status = curl_multi_add_handle(multi_handle, http_get_handle);
    if (status != CURLM_OK) {
        printf("curl_multi_add_handle failed with error %d", status);
        return NULL;
    }

    /* we start some action by calling perform right away */
    status = curl_multi_perform(multi_handle, &no_of_handles_running);
    if (status != CURLM_OK) {
       printf("curl_multi_perform failed with error %d", status);
        return NULL;
    }
	start_worker_thread_if_needed();
    return transaction;
}

//Read callback for fetching data to put or post
static size_t read_callback(void *ptr, size_t size, size_t nmemb, void *userp)
{
    curly_http_transaction *transaction = (curly_http_transaction*)userp;
    int bytes_read = 0;
    printf("We are asked to provide at most %d bytes", size*nmemb);
    if (size*nmemb < 1)
        return bytes_read;

    if (size*nmemb > transaction->size_left) {
        //This chunk can handle the whole transaction
		bytes_read = transaction->size_left;
		memcpy(ptr, transaction->data + (transaction->size - transaction->size_left), bytes_read);
        transaction->size_left = 0;
    } else {
        //This chunk can only handle part of the transaction
		bytes_read = (int)(size*nmemb);
		memcpy(ptr, transaction->data + (transaction->size - transaction->size_left), bytes_read);
        transaction->size_left = (int)(transaction->size_left - bytes_read);
    }
    return bytes_read;
}

curly_http_transaction_handle curly_http_put(char* url, void* data, int size, void* cb)
{
	CURLcode easy_status = CURLE_OK;
    CURLMcode status = CURLM_OK;
    curly_http_transaction* transaction = create_transaction(data, size, cb);
    CURL *http_put_handle = transaction->handle;

    /* set options */
    curl_easy_setopt(http_put_handle, CURLOPT_URL, url);
#if defined (DEBUG) || defined (_DEBUG)
    curl_easy_setopt(http_put_handle, CURLOPT_VERBOSE, 1L);
#endif
    /* enable uploading */
    curl_easy_setopt(http_put_handle, CURLOPT_UPLOAD, 1L);

    /* HTTP PUT please */
    curl_easy_setopt(http_put_handle, CURLOPT_PUT, 1L);

    /* we want to use our own read function */
    curl_easy_setopt(http_put_handle, CURLOPT_READFUNCTION, read_callback);

    /* pointer to pass to our read function */
    curl_easy_setopt(http_put_handle, CURLOPT_READDATA, transaction);

	/* Store our transaction pointer */
	easy_status = curl_easy_setopt(http_put_handle, CURLOPT_PRIVATE, (void*)transaction);
	if (easy_status != CURLE_OK) {
		printf("Failed setting private data.");
	}

    /* Add the easy handle to the multi handle */
    status = curl_multi_add_handle(multi_handle, http_put_handle);
    if (status != CURLM_OK) {
		printf("curl_multi_add_handle failed with error %d", status);
        return NULL;
    }

    /* Let's go */
    status = curl_multi_perform(multi_handle, &no_of_handles_running);
    if (status != CURLM_OK) {
		printf("curl_multi_perform failed with error %d", status);
        return NULL;
    }
	start_worker_thread_if_needed();
    return transaction;
}

/*
 * Internal worker thread handling. 
 * Only active if there are transfers in progress.
 */
#ifdef _WIN32
HANDLE thread_handle = NULL;
DWORD WINAPI worker_thread(LPVOID lpParam)
{
	do {
		poll();
		Sleep(100);
	} while (no_of_handles_running > 0);
	printf("stopping worker thread");
	CloseHandle(thread_handle);
	thread_handle = NULL;
	return 0;
}
void start_worker_thread_if_needed() {
	if (thread_handle == NULL) {
		printf("starting worker thread");
		thread_handle = CreateThread(NULL, 0, worker_thread, NULL, 0, NULL);
	}
}
#else 
//TODO posix
#endif
