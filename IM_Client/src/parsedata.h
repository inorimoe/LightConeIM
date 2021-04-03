#pragma once
#pragma execution_character_set("utf-8")
#include <QByteArray>
#include "netsocket.h"
#include "msgitemview.h"
#include "chatmsgitem.h"

class ParseData
{
public:
	static HTTP parseHttpResponse(QByteArray* pData);
	static void parseStatusLine(std::string& statusLine, HTTP* pHttp);
	static bool parseHttpFromLogin(NetSocket* netSocket);
	static void parseHttpToMsgItem(NetSocket* netSocket, QList<MsgItemData>* dataList);
	static void parseChatedData(NetSocket* netSocket, QList<tuple<QString, ChatMsgItem::User_Type>> *msg);
	static void parseChattingData(NetSocket* netSocket, QString* msg, int* senderId);
	static bool parseSignUpData(NetSocket* netSocket);
	static void parseNewFriendData(NetSocket* netSocket, MsgItemData* item, QString* type);
};