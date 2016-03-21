/*
 * curly.h
 * Wrapper code for libcurl.
 *
 *  Created on: 31/03/2015
 *      Author: johan
 */

#ifndef CURLY_H_
#define CURLY_H_

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Bitmasks for logging options.
 */
#define CURLY_LOG_API                   0x01
#define CURLY_LOG_DATA                  0x02
#define CURLY_LOG_HEADERS               0x04
#define CURLY_LOG_INFO                  0x08
    
/**
 * Transaction handle to the http operation. 
 * Returned by the GET, PUT and POST functions
 * and references in the callbacks.
 */
typedef void* curly_http_transaction_handle;
    
/**
 * Optional callback that curly will call once a transaction completes.
 * @param handle                The transaction identified previously returned by
 *                              #curly_http_get, #curly_http_get or #curly_http_post
 * @param http_response_code    The http response code such as 200 etc
 * @param data                  Retrieved data in the case of a GET operation
 * @param size                  Size of the data in bytes
 */
typedef	void(*on_http_request_completed)(curly_http_transaction_handle handle, int http_response_code, void* data, int size);

typedef struct curly_config{
    /**
     * Callback for receiving log putput from curly and libcurl.
     *
     * Default: NULL
     *
     */
    void(*log_cb)(char* msg);
    
    /**
     * Bitmap of log options to apply if log_cb is assigned.
     * Set to for instance CURLY_LOG_INFO | CURLY_LOG_DATA 
     * to see info messages from libcurl as well as the actual data.
     *
     * Default: 0
     *
     */
    int log_options;
    
    /**
     * If server uses a self signed certificate you might have to
     * either provide a .crt file or turn off the peer verification.
     *
     * Default: 0
     *
     */
    int do_not_verify_peer;
    
    /**
     * File system path to the certificate if needed.
     *
     * Default: NULL
     *
     */
    char* certificate_path;
} curly_config;
    
/**
 * Assign default values to the curly configuration.
 * Recoemmended to call before assigning your own custom parameters.
 *
 * @param cfg               The configuration
 *
 */
void curly_config_default(curly_config* cfg);
    
/**
 * Initialize curly
 * Unless you want to provide a config with non default values
 * this method is optional.
 *
 * @param cfg               The configuration
 *
 */
void curly_init(curly_config* cfg);
    
/**
 * Dispose curly
 */
void curly_dispose();

/**
 * Perform http get
 * @param url               The complete url including http or https
 * @param headers_json      List of optional headers as a json array for instance:
 *                          [\"Accept: application/json\",\"Content-Type: application/json\"]
 * @param cb                Optional callback function as defined by #on_http_request_completed
 * 
 * @return                  Transaction handle as defined by #curly_http_transaction_handle
 */
curly_http_transaction_handle curly_http_get(char* url, char* headers_json, void* cb);
    
/**
 * Perform http put
 * @param url               The complete url including http or https
 * @param data              The data to send to the server
 * @param size              Size of the data in bytes
 * @param headers_json      List of optional headers as a json array for instance:
 *                          [\"Accept: application/json\",\"Content-Type: application/json\"]
 * @param cb                Optional callback function as defined by #on_http_request_completed
 *
 * @return                  Transaction handle as defined by #curly_http_transaction_handle
 */
curly_http_transaction_handle curly_http_put(char* url, void* data, int size, char* headers_json, void* cb);

#ifdef __cplusplus
}
#endif
#endif //CURLY_H_

