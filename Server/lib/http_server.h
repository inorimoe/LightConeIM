#ifndef HTTP_SERVER_H
#define HTTP_SERVER_H

#include <map>
#include <string>
#include "common.h"
#include "tcp_server.h"
#include "http_request.h"
#include "http_response.h"
#include "tcp_connection.h"
#include "sql.h"
#include "buffer.h"

typedef void (*parseData_callback)(struct http_request *httpRequest,
                                   struct received_data* receivedData,
                                   int* getSqlDataType);

typedef struct send_data* (*dealData_callback)(struct received_data* receivedData,
                                              int* getSqlDataType,
                                              struct tcp_connection* tcpConnection);
            
typedef void (*formatData_callback)(struct http_response *httpResponse, 
                                    struct send_data *sendData);



struct http_server {
    struct TCPserver *tcpServer;
    
    //uid <-> fd ，在线的用户Id与其tcp连接fd的映射；
    std::map<int,int> fd_pool; 

    parseData_callback parseDataCallback;
    dealData_callback dealDataCallback;
    formatData_callback formatDataCallback;
    struct sqlData *sqlData;
};

struct http_server *http_server_new(struct event_loop *eventLoop, int port,
                                    parseData_callback parseDataCallback,
                                    dealData_callback dealDataCallback,
                                    formatData_callback formatDataCallback,
                                    struct sqlData* sqlData,
                                    int threadNum);


void http_server_start(struct http_server *httpServer);

int parse_http_request(struct buffer *input, struct http_request *httpRequest);


/********************received_data***********************/
struct received_data {
    std::map<std::string, std::string> kValues;
    struct buffer *data;
};

struct received_data* received_data_new();
void received_data_reset(struct received_data* receivedData);
void received_data_free(struct received_data* receivedData);


/********************send_data***************************/
enum send_type{
    NotSend ,                  
    SendToSource ,             
    SendToTarget                        
};

struct send_data {
    int sendFd;
    send_type sendType;
    std::map<std::string, std::string> kValues;
    struct buffer *data;
};

struct send_data* send_data_new();
void send_data_reset(struct send_data* sendData);
void send_data_free(struct send_data* sendData);
#endif