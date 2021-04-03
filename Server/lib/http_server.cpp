#include <assert.h>
#include "http_server.h"
#include "sql.h"

//连接建立之后的callback
int http_onConnectionCompleted(struct tcp_connection *tcpConnection) {
    yolanda_msgx("connection completed");
    struct http_request *httpRequest = http_request_new();
    tcpConnection->request = httpRequest;
    struct http_response *httpResponse = http_response_new();
    tcpConnection->response = httpResponse;
    struct sqlQueryData *queryData = new_queryData();
    
    return 0;
}

int process_status_line(char *start, char *end, struct http_request *httpRequest) {
    int size = end - start;
    //method
    char *space = memmem(start, end - start, " ", 1);
    assert(space != NULL);
    int method_size = space - start;
    httpRequest->method = malloc(method_size + 1);
    strncpy(httpRequest->method, start, space - start);
    httpRequest->method[method_size + 1] = '\0';

    //url
    start = space + 1;
    space = memmem(start, end - start, " ", 1);
    assert(space != NULL);
    int url_size = space - start;
    httpRequest->url = malloc(url_size + 1);
    strncpy(httpRequest->url, start, space - start);
    httpRequest->url[url_size + 1] = '\0';

    //version
    start = space + 1;
    httpRequest->version = malloc(end - start + 1);
    strncpy(httpRequest->version, start, end - start);
    httpRequest->version[end - start + 1] = '\0';
    assert(space != NULL);
    return size;
}

int parse_http_request(struct buffer *input, struct http_request *httpRequest) {
    int ok = 1;
    while (httpRequest->current_state != REQUEST_DONE) {
        yolanda_msgx("parse http");
        if (httpRequest->current_state == REQUEST_STATUS) {
            char *crlf = buffer_find_CRLF(input);
            if (crlf) {
                int request_line_size = process_status_line(input->data + input->readIndex, crlf, httpRequest);
                if (request_line_size) {
                    input->readIndex += request_line_size;  // request line size
                    input->readIndex += 2;  //CRLF size
                    httpRequest->current_state = REQUEST_HEADERS;
                }
                yolanda_msgx("httpRequest->current_state -> crlf");
            }
            yolanda_msgx("httpRequest->current_state == REQUEST_STATUS");
        } else if (httpRequest->current_state == REQUEST_HEADERS) {
            char *crlf = buffer_find_CRLF(input);
            if (crlf) {
                /**
                 *    <start>-------<colon>:-------<crlf>
                 */
                char *start = input->data + input->readIndex;
                int request_line_size = crlf - start;
                char *colon = memmem(start, request_line_size, ": ", 2);
                if (colon != NULL) {
                    char *key = malloc(colon - start + 1);
                    strncpy(key, start, colon - start);
                    key[colon - start] = '\0';
                    char *value = malloc(crlf - colon - 2 + 1);
                    strncpy(value, colon + 2, crlf - colon - 2);
                    value[crlf - colon - 2] = '\0';

                    http_request_add_header(httpRequest, key, value);

                    input->readIndex += request_line_size;  //request line size
                    input->readIndex += 2;  //CRLF size
                } else {
                    //读到这里说明:没找到，就说明这个是最后一行
                    input->readIndex += 2;  //CRLF size
                    httpRequest->current_state = REQUEST_BODY;
                }
            }
            yolanda_msgx(buffer_get_string(input));
            yolanda_msgx("httpRequest->current_state == REQUEST_HEADERS");
        } else if (httpRequest->current_state == REQUEST_BODY) {
            int bodyLen = input->writeIndex - input->readIndex;
            if (bodyLen != 0) {
                buffer_append(httpRequest->body, buffer_get_string(input), buffer_readable_size(input));
                buffer_append_char(httpRequest->body, '\0');
                yolanda_msgx("body append receivedData!");
            }
            httpRequest->current_state = REQUEST_DONE;
        }
    }
    return ok;
}

// buffer是框架构建好的，并且已经收到部分数据的情况下
// 注意这里可能没有收到全部数据，所以要处理数据不够的情形
int http_onMessage(struct buffer *input, struct tcp_connection *tcpConnection) {
    yolanda_msgx("get message from tcp connection %s", tcpConnection->name);

    struct http_request *httpRequest = (struct http_request *) tcpConnection->request;
    struct http_response *httpResponse = (struct http_response *)tcpConnection->response;
    struct http_server *httpServer = (struct http_server *) tcpConnection->data;
    struct received_data *receivedData = received_data_new();
    struct send_data *sendData = NULL;
    struct buffer *httpResponseBuffer = buffer_new();

    if (parse_http_request(input, httpRequest) == 0) {
        char *error_response = "HTTP/1.1 400 Bad Request\r\n\r\n";
        tcp_connection_send_data(tcpConnection, error_response, sizeof(error_response));
        tcp_connection_shutdown(tcpConnection);
    }

    //处理完了所有的request数据，接下来进行编码和发送
    if (http_request_current_state(httpRequest) == REQUEST_DONE) {
        int getSqlDataType = -1;
        if (httpServer->parseDataCallback != NULL){
            httpServer->parseDataCallback(httpRequest, receivedData, &getSqlDataType);
            yolanda_msgx("httpServer->parseDataCallback!");
        }
        if (httpServer->dealDataCallback != NULL){
            
            sendData = httpServer->dealDataCallback(receivedData, &getSqlDataType, tcpConnection);
            yolanda_msgx("httpServer->getSqlDataCallback!!");
        }
        if (httpServer->formatDataCallback != NULL) {
            
            httpServer->formatDataCallback(httpResponse, sendData);
            yolanda_msgx("httpServer->formatDataCallback!!!");
        }
        
        if(sendData->sendType != send_type::NotSend){
            http_response_encode_buffer(httpResponse, httpResponseBuffer);
            yolanda_msgx("http_response_encode_buffer!!!");
            tcp_connection_send_buffer(tcpConnection, httpResponseBuffer);
        }

        yolanda_msgx("tcp_connection_send_buffer(tcpConnection, buffer);!!!");
        if (http_request_close_connection(httpRequest)) {
            tcp_connection_shutdown(tcpConnection);
        }
        yolanda_msgx("http_request_close_connection!!!");
    }

    http_request_reset(httpRequest);
    http_response_reset(httpResponse);
    received_data_free(receivedData);
    send_data_free(sendData);
    buffer_free(httpResponseBuffer);
}

//数据通过buffer写完之后的callback
int http_onWriteCompleted(struct tcp_connection *tcpConnection) {
    yolanda_msgx("write completed");
    return 0;
}

//连接关闭之后的callback
int http_onConnectionClosed(struct tcp_connection *tcpConnection) {
    yolanda_msgx("connection closed");
    if (tcpConnection->request != NULL) {
        http_request_clear(tcpConnection->request);
        tcpConnection->request = NULL;
    }
    if(tcpConnection->response != NULL){
        http_response_clear(tcpConnection->response);
        tcpConnection->response = NULL;
    }
    if( tcpConnection->userId > 0 ){
        struct http_server* httpServer = static_cast<struct http_server*>(tcpConnection->data);
        httpServer->fd_pool.erase(tcpConnection->userId);
    }
    return 0;
}


struct http_server *http_server_new(struct event_loop *eventLoop, int port,
                                    parseData_callback parseDataCallback,
                                    dealData_callback dealDataCallback,
                                    formatData_callback formatDataCallback,
                                    struct sqlData* sqlData,
                                    int threadNum) {
    //struct http_server *httpServer = malloc(sizeof(struct http_server));
    struct http_server *httpServer = new struct http_server;
    httpServer->parseDataCallback = parseDataCallback;
    httpServer->dealDataCallback = dealDataCallback;
    httpServer->formatDataCallback = formatDataCallback;
    //初始化acceptor
    struct acceptor *acceptor = acceptor_init(SERV_PORT);

    httpServer->tcpServer = tcp_server_init(eventLoop, acceptor, http_onConnectionCompleted, http_onMessage,
                                            http_onWriteCompleted,
                                            http_onConnectionClosed, threadNum);
    // for callback
    httpServer->tcpServer->data = httpServer;
    // init sql
    httpServer->sqlData = sqlData;
    return httpServer;
}


void http_server_start(struct http_server *httpServer) {
    tcp_server_start(httpServer->tcpServer);
}

struct received_data* received_data_new(){
    struct received_data *receivedData = new struct received_data;
    receivedData->data = buffer_new();
    return receivedData;
}

void received_data_reset(struct received_data* receivedData){
    receivedData->kValues.clear();
    buffer_data_reset(receivedData->data);
}

void received_data_free(struct received_data* receivedData){
    if(receivedData != NULL){
        buffer_free(receivedData->data);
        delete receivedData;
        receivedData = NULL;
    }
}

struct send_data* send_data_new(){
    struct send_data *sendData = new struct send_data;
    sendData->data = buffer_new();
    sendData->sendType = -1;
    return sendData;
}
void send_data_reset(struct send_data* sendData){
    sendData->kValues.clear();
    buffer_data_reset(sendData->data);
}
void send_data_free(struct send_data* sendData){
    if(sendData != NULL){
        buffer_free(sendData->data);
        delete sendData;
        sendData = NULL;
    }
}