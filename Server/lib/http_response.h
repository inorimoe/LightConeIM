#include "buffer.h"

#ifndef HTTP_RESPONSE_H
#define HTTP_RESPONSE_H

struct response_header {
    char *key;
    char *value;
};

enum HttpStatusCode {
    Unknown,
    OK = 200,
    MovedPermanently = 301,
    BadRequest = 400,
    NotFound = 404,
};

struct http_response {
    enum HttpStatusCode statusCode;
    char *statusMessage;
    char *contentType;
    struct buffer *body;
    struct response_header *response_headers;
    int response_headers_number;
    int keep_connected;
};

struct http_response *http_response_new();

void http_response_encode_buffer(struct http_response *httpResponse, struct buffer *buffer);

//清除一个response对象
void http_response_clear(struct http_response *httpResponse);
//重置response对象
void http_response_reset(struct http_response *httpResponse);
#endif