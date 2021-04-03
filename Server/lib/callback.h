#ifndef _CALLBACK_H
#define _CALLBACK_H

#include "http_server.h"
#include "common.h"
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <unistd.h>
#include <mysql/mysql.h>
#include <map>
#include "sql.h"

//http_server callback
void onParseData(struct http_request *httpRequest, struct received_data* receivedData, int* getSqlDataType);

struct send_data* onDealData(struct received_data* receivedData, 
                             int* getSqlDataType, struct tcp_connection* tcpConnection);

void onFormatData(struct http_response *httpResponse, struct send_data *sendData);


//normal callfunction;
void onLogin(struct sqlData* sqlData, struct sqlQueryData* queryData,
             struct received_data* receivedData, struct send_data* sendData,
             struct http_server* httpServer, struct tcp_connection* tcpConnection);

void onSignUp(struct sqlData* sqlData, struct sqlQueryData* queryData,
              struct received_data* receivedData, struct send_data* sendData);

void onNoMessage(struct received_data* receivedData, struct send_data* sendData);

void onMsgItemData(struct sqlData* sqlData, struct sqlQueryData* queryData, 
                   struct received_data* receivedData, struct send_data* sendData);

void onInsertChatData(struct sqlData* sqlData, struct sqlQueryData* queryData, 
                      struct received_data* receivedData, struct send_data* sendData);

void onSendChatedData(struct sqlData* sqlData, struct sqlQueryData* queryData, 
                      struct received_data* receivedData, struct send_data* sendData);

void onSendNewChatData(struct sqlData* sqlData, struct sqlQueryData* queryData, 
                       struct received_data* receivedData, struct send_data* sendData,
                       struct tcp_connection* tcpConnection);
                    
void onInsertChatData(const std::string& query, struct sqlData* sqlData, struct sqlQueryData* queryData);

void onAddFriendData(struct sqlData* sqlData, struct sqlQueryData* queryData, 
                      struct received_data* receivedData, struct send_data* sendData);
#endif