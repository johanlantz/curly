//
//  main.c
//  curlytest
//
//  Created by johan on 02/07/16.
//  Copyright © 2016 ilantz. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include "curly.h"
#include <time.h>
#include <sys/time.h>
#include <unistd.h>
#include <string.h>


static void curly_log_msg_cb(char* msg)
{
    printf("curly says: %s\n", msg);
}

static void on_http_get_request_completed(curly_http_transaction_handle handle, long http_response_code, void* data, long size) {
    printf("http get completed for handle %p:\n with code=%ld\n", handle, http_response_code);
    printf("%ld bytes downloaded with content \n %.*s", size, (int)size, data);
}

static void on_http_put_request_completed(curly_http_transaction_handle handle, long http_response_code, void* data, long size) {
    printf("http put completed for handle %p:\n with code=%ld\n", handle, http_response_code);
}

int main(int argc, const char * argv[]) {
    printf("Starting curly!\n");
    curly_config my_cfg;
    int loops = (argc >= 3) ? atoi(argv[2]) : 1;
    curly_config_default(&my_cfg);
    my_cfg.log_cb = &curly_log_msg_cb;
    //Valgrind complains about the logging callback
    //my_cfg.log_options = CURLY_LOG_API | CURLY_LOG_DATA | CURLY_LOG_HEADERS | CURLY_LOG_INFO;
    curly_init(&my_cfg);
    
    if (argv[1] != NULL && strcmp(argv[1], "put") == 0) {
        for (; loops > 0; loops--) {
            curly_http_put("https://httpbin.org/put", "abcd", 4, NULL, &on_http_put_request_completed);
        }
    } else {
        for (; loops > 0; loops--) {
            curly_http_get("http://google.com", NULL, &on_http_get_request_completed);
        }
    }

    getchar();
    return 0;
}
