#include "callback.h"
#include "../rapidjson/document.h"
#include "../rapidjson/prettywriter.h"

using namespace std;
using namespace rapidjson;

#define NoMessage                 0
#define LOGIN                     1
#define MsgItemData               2
#define InsertChatData            3  
#define SendChatedDatas           4
#define SendNewChatData           5
#define SignUp                    6
#define AddFriend                 7

void onParseData(struct http_request *httpRequest,
                           struct received_data* receivedData,
                           int* getSqlDataType)
{
    map<string, string>& httpHeaders = receivedData->kValues;
    for(int i = 0; i < httpRequest->request_headers_number; ++i ){
        string key = httpRequest->request_headers[i].key;
        string value = httpRequest->request_headers[i].value;
        httpHeaders[key] = value;
    }

    map<string, string>::iterator  messageType;
    if((messageType = httpHeaders.find("MessageType")) == httpHeaders.end()){
        *getSqlDataType = NoMessage;
        buffer_append_string(receivedData->data, "Haha!");
        //buffer_copy(receivedData->data, httpRequest->body);
        return;
    }
    //auto messageType = httpHeaders.at(string("MessageType"));
    if( messageType->second == "login" ){
        *getSqlDataType = LOGIN;
        buffer_copy(receivedData->data, httpRequest->body);
    }
    if( messageType->second == "msgItemData" ){
        *getSqlDataType = MsgItemData;
        buffer_copy(receivedData->data, httpRequest->body);
    }
    if( messageType->second == "senNewChatData" ){
        *getSqlDataType = SendNewChatData;
        buffer_copy(receivedData->data, httpRequest->body);
    }
    if( messageType->second == "getChatedData" ){
        *getSqlDataType = SendChatedDatas;
        buffer_copy(receivedData->data, httpRequest->body);
    }
    if( messageType->second == "signUp" ){
        *getSqlDataType = SignUp;
        buffer_copy(receivedData->data, httpRequest->body);
    }
    if( messageType->second == "addFriend"){
        *getSqlDataType = AddFriend;
        buffer_copy(receivedData->data, httpRequest->body);
    }
};                          

struct send_data* onDealData(struct received_data* receivedData,
                                      int* getSqlDataType,                                  
                                      struct tcp_connection *tcpConnection)
{
    struct http_server* httpServer = static_cast<struct http_server*>(tcpConnection->data);

    struct sqlData* sqlData = httpServer->sqlData;
    struct sqlQueryData* queryData = new_queryData();
    struct send_data* sendData = send_data_new();

    switch(*getSqlDataType){
        case LOGIN:
            onLogin(sqlData, queryData, receivedData, sendData, httpServer, tcpConnection);
            sendData->sendType = send_type::SendToSource;
            break;
        case MsgItemData:
            onMsgItemData(sqlData, queryData, receivedData, sendData);
            sendData->sendType = send_type::SendToSource;
            break;
        case SendNewChatData:
            onSendNewChatData(sqlData, queryData, receivedData, sendData, tcpConnection);
            //onInsertChatData(sqlData, queryData, receivedData, sendData);
            break;
        case SendChatedDatas:
            onSendChatedData(sqlData, queryData, receivedData, sendData);
            sendData->sendType = send_type::SendToSource;
            break;
        case SignUp:
            onSignUp(sqlData, queryData, receivedData, sendData);
            sendData->sendType = send_type::SendToSource;
            break;
        case AddFriend:
            onAddFriendData(sqlData, queryData, receivedData, sendData);
            sendData->sendType = send_type::SendToSource;
            break;
        default:
            onNoMessage(receivedData, sendData);
            sendData->sendType = send_type::NotSend;
    }

    destroy_queryData(queryData);
    return sendData;
};                                   
            
void onFormatData(struct http_response *httpResponse, struct send_data *sendData)
{
    if(sendData->sendType == send_type::NotSend){
        return;
    }
        
    httpResponse->statusCode = OK;
    httpResponse->statusMessage = "OK";
    httpResponse->contentType = "text/plain";

    for(auto iter = sendData->kValues.begin(); iter != sendData->kValues.end(); ++iter){
        int i = httpResponse->response_headers_number++;

        char* key = (char*)malloc((*iter).first.size()+1);
        memcpy(key, (*iter).first.data(), (*iter).first.size()+1);
        httpResponse->response_headers[i].key = key;

        char* value = (char*)malloc((*iter).second.size()+1);
        memcpy(value, (*iter).second.data(), (*iter).second.size()+1);
        httpResponse->response_headers[i].value = value;
    }
    buffer_append_string(httpResponse->body, buffer_get_string(sendData->data));
    buffer_append_char(httpResponse->body, '\0');//end;
};

void onNoMessage(struct received_data* receivedData, struct send_data* sendData){
    buffer_copy(sendData->data, receivedData->data);
};

void onLogin(struct sqlData* sqlData, struct sqlQueryData* queryData,
             struct received_data* receivedData, struct send_data* sendData,
             struct http_server* httpServer, struct tcp_connection *tcpConnection)
{
    //json
    StringBuffer jsonStrBuff;
    PrettyWriter<StringBuffer> writer(jsonStrBuff);
    writer.StartObject();

    // unsigned int num_fields;
    string query = string("select * from User where Account = '") + receivedData->kValues.at("Account")
                 + string("'and Password = '") + receivedData->kValues.at("Password")
                 + string("';");

    sendData->kValues["MessageType"] = "login";
    
    pthread_mutex_lock(&sqlData->mutex);
    if(mysql_real_query(&(sqlData->mysql), query.c_str(), query.size())){
        pthread_mutex_unlock(&sqlData->mutex);
        yolanda_msgx("Query failed(%s)", mysql_error(&(sqlData->mysql)));

        writer.Key("Query");
        writer.String("Failed");
    }
    else{
        yolanda_msgx("Query success!");

        queryData->result = mysql_store_result(&(sqlData->mysql));
        pthread_mutex_unlock(&sqlData->mutex);
        // num_fields = mysql_num_fields(queryData->result);
        // yolanda_msgx("results num_fields:%d", num_fields);

        MYSQL_FIELD *fields = mysql_fetch_fields(queryData->result);
        MYSQL_ROW currentROW =  mysql_fetch_row(queryData->result);
        if(currentROW != NULL){
            yolanda_msgx(currentROW[1]);
            writer.Key("Query");
            writer.String("succeed");
            writer.Key(fields[0].name);
            writer.Int(stoi(currentROW[0]));
            writer.Key(fields[1].name);
            writer.String(currentROW[1]);
            //password
            //writer.Key(fields[2].name);
            //writer.String(currentROW[2]);

            //build uid->fd 映射
            tcpConnection->userId = stoi(currentROW[0]);
            httpServer->fd_pool[tcpConnection->userId] = tcpConnection->channel->fd;
        }else{
            writer.Key("Query");
            writer.String("Account or password error.");
        }
    }
    yolanda_msgx("writer.String(currentROW[1]);");
    writer.EndObject();

    buffer_append_string(sendData->data, const_cast<char*>(jsonStrBuff.GetString()));
    buffer_append_char(sendData->data, '\0');//end;
};

void onMsgItemData(struct sqlData* sqlData, struct sqlQueryData* queryData,
             struct received_data* receivedData, struct send_data* sendData){
    StringBuffer jsonStrBuff;
    PrettyWriter<StringBuffer> writer(jsonStrBuff);
    writer.StartObject();

    string query = string("select User_id, Account from User where User_id in (select UserSecond_id as User_id from Friend  where UserFirst_id = ")
                 + string(receivedData->kValues.at("User_id").data())
                 + string(" UNION select UserFirst_id as User_id from Friend where UserSecond_id = ")
                 + string(receivedData->kValues.at("User_id").data())
                 + string(");");
    sendData->kValues["MessageType"] = "msgItemData";

    pthread_mutex_lock(&sqlData->mutex);
    if(mysql_real_query(&(sqlData->mysql), query.data(), strlen(query.data()))){
        pthread_mutex_unlock(&sqlData->mutex);
        yolanda_msgx("Query failed(%s)", mysql_error(&(sqlData->mysql)));

        writer.Key("Query");
        writer.String("msgItemData failed.");
    }
    else{
        yolanda_msgx("Query success!");
        writer.Key("Query");
        writer.String("succeed");

        queryData->result = mysql_store_result(&(sqlData->mysql));
        pthread_mutex_unlock(&sqlData->mutex);
        // num_fields = mysql_num_fields(queryData->result);

        MYSQL_FIELD *fields = mysql_fetch_fields(queryData->result);
        MYSQL_ROW currentROW;
        writer.Key("Array");
        writer.StartArray();
        while(currentROW =  mysql_fetch_row(queryData->result))
        {
            writer.StartObject();
            writer.Key(fields[0].name);
            writer.Int(stoi(currentROW[0]));
            writer.Key(fields[1].name);
            writer.String(currentROW[1]);
            writer.EndObject();
        }
        writer.EndArray();
    }
    writer.EndObject();
    buffer_append_string(sendData->data, const_cast<char*>(jsonStrBuff.GetString()));
    buffer_append_char(sendData->data, '\0');//end;
};

void onInsertChatData(struct sqlData* sqlData, struct sqlQueryData* queryData,
                    struct received_data* receivedData, struct send_data* sendData)
{
    //没有转发消息
    Document dom;
    dom.Parse(buffer_get_string(receivedData->data));

    string sender_id = to_string(dom["Sender_id"].GetInt());
    string received_id = to_string(dom["Received_id"].GetInt());
    string msg = dom["Msg"].GetString();

    string insertDataQuery = string("insert into chats(Sender_id, Receiver_id, Msg) values (")
                      + sender_id + ", " + received_id + ", '" + msg + "');";

    pthread_mutex_lock(&sqlData->mutex);
    mysql_real_query(&(sqlData->mysql), insertDataQuery.data(), strlen(insertDataQuery.data()));
    pthread_mutex_unlock(&sqlData->mutex);

    //注意
    buffer_data_reset(sendData->data);//clear
};

void onInsertChatData(const std::string& query, struct sqlData* sqlData, struct sqlQueryData* queryData)
{
    pthread_mutex_lock(&sqlData->mutex);
    //mysql_real_query(&(sqlData->mysql), query.data(), strlen(query.data()));

    if(mysql_real_query(&(sqlData->mysql), query.data(), strlen(query.data()))){
        yolanda_msgx("Query  insert failed(%s)", mysql_error(&(sqlData->mysql)));
    }
    else{
        yolanda_msgx("Query insert success!");
    }
    pthread_mutex_unlock(&sqlData->mutex);
}


void onSendNewChatData(struct sqlData* sqlData, struct sqlQueryData* queryData, 
                       struct received_data* receivedData, struct send_data* sendData,
                       struct tcp_connection* tcpConnection)
{
    Document dom;
    dom.Parse(buffer_get_string(receivedData->data));

    string sender_id = to_string(dom["Sender_id"].GetInt());
    string receiver_id = to_string(dom["Received_id"].GetInt());
    string msg = dom["Msg"].GetString();

    string insertDataQuery = string("insert into chats(Sender_id, Receiver_id, Msg) values (")
                      + sender_id + ", " + receiver_id + ", '" + msg + "');";
    onInsertChatData(insertDataQuery, sqlData, queryData);

    int senderId = stoi(sender_id);
    int receiverId = stoi(receiver_id);

    struct http_server* httpServer = static_cast<struct http_server*>(tcpConnection->data);
    std::map<int, int>::iterator iter = httpServer->fd_pool.find(receiverId);

    if(iter == httpServer->fd_pool.end())
    {
        //chat object dont online
        sendData->sendType = send_type::NotSend;
        return;
    }
    else{
        //发消息给接收者
        int receiverFd = iter->second;
        tcp_connection_set_targetfd(tcpConnection, receiverFd);
        yolanda_msgx("send message obj");

        sendData->kValues["MessageType"] = "chattingData";

        StringBuffer jsonStrBuff;
        PrettyWriter<StringBuffer> writer(jsonStrBuff);

        //http表头先不写
        //先写json
        writer.StartObject();
        writer.Key("Sender_id");
        writer.Int(senderId);
        writer.Key("Received_id");
        writer.Int(receiverId);
        writer.Key("Msg");
        writer.String(msg.data());
        writer.EndObject();

        struct buffer* outData = buffer_new();
        buffer_append_string(outData, const_cast<char*>(jsonStrBuff.GetString()));
        buffer_append_char(outData, '\0');//end;
        //发送
        // write(receiverFd, buffer_get_string(outData), buffer_readable_size(outData));
        buffer_data_reset(sendData->data);//clear
        buffer_append_string(sendData->data, const_cast<char*>(jsonStrBuff.GetString()));
        buffer_append_char(sendData->data, '\0');//end;
        sendData->sendType = send_type::SendToTarget;
    }
};                       

void onSendChatedData(struct sqlData* sqlData, struct sqlQueryData* queryData,
                     struct received_data* receivedData, struct send_data* sendData)
{
    yolanda_msgx("onSendChatedData");
    Document dom;
    dom.Parse(buffer_get_string(receivedData->data));

    string User_id = to_string(dom["User_id"].GetInt());
    string Friend_id = to_string(dom["Friend_id"].GetInt());

    StringBuffer jsonStrBuff;
    PrettyWriter<StringBuffer> writer(jsonStrBuff);
    writer.StartObject();

    string query = string("select Sender_id,Receiver_id,Send_time,Msg from chats where Sender_id = ")
                           + User_id + string(" and Receiver_id = ") + Friend_id
                           + string(" union select Sender_id,Receiver_id,Send_time,Msg from chats where Sender_id = ")
                           + Friend_id + string(" and Receiver_id = ")+ User_id +string(" order by Send_time asc;");
    sendData->kValues["MessageType"] = "chatedData";

    pthread_mutex_lock(&sqlData->mutex);
    if(mysql_real_query(&(sqlData->mysql), query.data(), strlen(query.data()))){
        pthread_mutex_unlock(&sqlData->mutex);
        yolanda_msgx("Query failed(%s)", mysql_error(&(sqlData->mysql)));

        writer.Key("Query");
        writer.String("QueryChatedData failed.");
    }
    else{
        pthread_mutex_unlock(&sqlData->mutex);

        queryData->result = mysql_store_result(&(sqlData->mysql));
        yolanda_msgx("Query success!");
        writer.Key("Query");
        writer.String("succeed");
        
        MYSQL_FIELD *fields = mysql_fetch_fields(queryData->result);
        MYSQL_ROW currentROW;
        writer.Key("Array");
        writer.StartArray();
        while(currentROW =  mysql_fetch_row(queryData->result)){
            writer.StartObject();
            writer.Key(fields[0].name);
            writer.Int(stoi(currentROW[0]));
            writer.Key(fields[1].name);
            writer.Int(stoi(currentROW[1]));
            writer.Key(fields[2].name);
            writer.String(currentROW[2]);
            writer.Key(fields[3].name);
            writer.String(currentROW[3]);
            writer.EndObject();
        }
        writer.EndArray();
    }
    writer.EndObject();
    buffer_append_string(sendData->data, const_cast<char*>(jsonStrBuff.GetString()));
    buffer_append_char(sendData->data, '\0');//end; 
};

void onSignUp(struct sqlData* sqlData, struct sqlQueryData* queryData,
              struct received_data* receivedData, struct send_data* sendData)
{
    yolanda_msgx("onSignUp");
    string account = receivedData->kValues.at("Account");
    string password = receivedData->kValues.at("Password");
    int succeess = 0;
    uint64_t rowCounts;
    sendData->kValues["MessageType"] = "signUp";

    string query =string("INSERT IGNORE INTO User (Account, Password) VALUES(\'")
                 +account.data() + string("\', \'") + password.data() + string("\');");

    StringBuffer jsonStrBuff;
    PrettyWriter<StringBuffer> writer(jsonStrBuff);
    writer.StartObject();

    pthread_mutex_lock(&sqlData->mutex);
    yolanda_msgx("Query onSignUp(get lock)");
    succeess = !mysql_real_query(&(sqlData->mysql), query.data(), strlen(query.data()));
    pthread_mutex_unlock(&sqlData->mutex);

    if(succeess){
        rowCounts = mysql_affected_rows(&sqlData->mysql);
        writer.Key("SignUp");
        if(rowCounts > 0){
            //insert ok
            writer.Bool(true);
        }
        else{
            //account has exist
            writer.Bool(false);
        }
    }
    else{
        yolanda_msgx("Query failed(%s)", mysql_error(&(sqlData->mysql)));
    }
    writer.EndObject();
    buffer_append_string(sendData->data, const_cast<char*>(jsonStrBuff.GetString()));
    buffer_append_char(sendData->data, '\0');//end; 
}

void onAddFriendData(struct sqlData* sqlData, struct sqlQueryData* queryData, 
                      struct received_data* receivedData, struct send_data* sendData)
{
    yolanda_msgx("onAddFriendData begin");
    Document dom;
    dom.Parse(buffer_get_string(receivedData->data));
    sendData->kValues["MessageType"] = "addFriend";
    uint64_t rowCounts;
    MYSQL_ROW currentROW;
    int foundFriendId = 1;
    int foundRelation = 1;
    int insertRelation = 1;
    string account = receivedData->kValues.at("Friend_account");
    string userId = receivedData->kValues.at("User_id");
    StringBuffer jsonStrBuff;
    PrettyWriter<StringBuffer> writer(jsonStrBuff);
    writer.StartObject();

    string queryFriendId = string("SELECT User_id AS Friend_id FROM User WHERE Account = \'") + account + string("\';");

    yolanda_msgx("onAddFriendData init over");
    pthread_mutex_lock(&sqlData->mutex);
    foundFriendId = !mysql_real_query(&(sqlData->mysql), queryFriendId.data(), strlen(queryFriendId.data()));
    pthread_mutex_unlock(&sqlData->mutex);
    queryData->result = mysql_store_result(&(sqlData->mysql));

    yolanda_msgx("onAddFriendData foundFriendId %d",foundFriendId);
    if(foundFriendId == true){
        writer.Key("FoundFriend");
        rowCounts = mysql_affected_rows(&sqlData->mysql);
        yolanda_msgx("onAddFriendData rowCounts %d",rowCounts);
        if(rowCounts == -1 || rowCounts == 0){
            writer.Bool(false);
            writer.EndObject();
            buffer_append_string(sendData->data, const_cast<char*>(jsonStrBuff.GetString()));
            buffer_append_char(sendData->data, '\0');//end; 
            return;
        }
        writer.Bool(true);
        yolanda_msgx("onAddFriendData currentROW start");

        currentROW = mysql_fetch_row(queryData->result);
        string friendId = currentROW[0];
        if(queryData->result != NULL){
            mysql_free_result(queryData->result);
        }

        yolanda_msgx("onAddFriendData currentROW end");
        string queryRelation = string("select * from Friend  where ( UserSecond_id = ")
                             + friendId + string(" && UserFirst_id = ")
                             + userId + string(" ) or ( UserSecond_id = ")
                             + userId + string(" && UserFirst_id = ")
                             + friendId +string(" );");

        yolanda_msgx("onAddFriendData foundRelation start %s",queryRelation.data());
        pthread_mutex_lock(&sqlData->mutex);
        foundRelation = !mysql_real_query(&(sqlData->mysql), queryRelation.data(), strlen(queryRelation.data()));
        pthread_mutex_unlock(&sqlData->mutex);
        yolanda_msgx("onAddFriendData foundRelation end");

        writer.Key("RelationNotExist");
        queryData->result = mysql_store_result(&(sqlData->mysql));
        rowCounts = mysql_affected_rows(&sqlData->mysql);
        if(rowCounts > 0){
            writer.Bool(false);
            writer.EndObject();
            buffer_append_string(sendData->data, const_cast<char*>(jsonStrBuff.GetString()));
            buffer_append_char(sendData->data, '\0');//end; 
            return;
        }
        writer.Bool(true);
        if(queryData->result != NULL){
            mysql_free_result(queryData->result);
        }

        string queryInsertRelation = string("INSERT IGNORE INTO Friend(UserFirst_id, UserSecond_id) VALUES( ") + userId + " , " + friendId + " );";
        pthread_mutex_lock(&sqlData->mutex);
        insertRelation = !mysql_real_query(&(sqlData->mysql), queryInsertRelation.data(), strlen(queryInsertRelation.data()));
        pthread_mutex_unlock(&sqlData->mutex);
        queryData->result = mysql_store_result(&(sqlData->mysql));
        writer.Key("FriendAccound");
        writer.String(account.data());
        writer.Key("FriendID");
        writer.Int(stoi(friendId.data()));
        yolanda_msgx("onAddFriendData three");
    }
    yolanda_msgx("onAddFriendData end");
    if( !foundFriendId || !foundRelation || !insertRelation){
        yolanda_msgx("Query failed(%s)", mysql_error(&(sqlData->mysql)));
    }
    yolanda_msgx("onAddFriendData end2");
    writer.EndObject();
    yolanda_msgx("onAddFriendData end3");
    buffer_append_string(sendData->data, const_cast<char*>(jsonStrBuff.GetString()));
    buffer_append_char(sendData->data, '\0');//end; 
    yolanda_msgx(jsonStrBuff.GetString());
}                      