#include "parsedata.h"
#include "netsocket.h"
#include "msgitemview.h"
#include "../lib/common.h"
#include <QDeBug>
#include <QJsonArray>

HTTP ParseData::parseHttpResponse(QByteArray* pData)
{
	HTTP http;
	std::string data = pData->data();
	while (http.currentState != REQUEST_DONE)
	{
		if (http.currentState == REQUEST_STATUS)
		{
			string statusLine = data.substr(0, data.find(CRLF) + 2);
			data.erase(0, data.find(CRLF) + 2);
			parseStatusLine(statusLine, &http);
			http.currentState = REQUEST_HEADERS;
		}
		else if (http.currentState == REQUEST_HEADERS)
		{
			int lenth = data.find(CRLF);
			if (lenth)
			{
				string header = data.substr(0, data.find(CRLF));
				//data.erase(0, 2);
				data.erase(0, data.find(CRLF) + 2);
				string key = header.substr(0, header.find(": "));
				string value = header.erase(0, header.find(": ") + 2);
				http.headers[key] = value;
			}
			else
			{
				data.erase(0, 2);
				http.currentState = REQUEST_BODY;
			}
		}
		else if (http.currentState == REQUEST_BODY)
		{
			http.body = data;
			http.currentState = REQUEST_DONE;
		}
	}
	return http;
}

void ParseData::parseStatusLine(string& statusLine, HTTP* pHttp)
{
	std::string version = statusLine.substr(0, statusLine.find_first_of(" "));
	pHttp->version.append(version);

	statusLine.erase(0, statusLine.find_first_of(" ") + 1);
	std::string statusCode = statusLine.substr(0, statusLine.find_first_of(" "));
	pHttp->statusCode.append(statusCode);

	statusLine.erase(0, statusLine.find_first_of(" ") + 1);
	std::string statusMessage = statusLine.substr(0, statusLine.find_first_of(CRLF));
	pHttp->statusMessage.append(statusMessage);
}

bool ParseData::parseHttpFromLogin(NetSocket* netSocket)
{
	QByteArray json (netSocket->getHttp()->body.data());
	QJsonParseError parseJsonErr;
	QJsonDocument dom = QJsonDocument::fromJson(json, &parseJsonErr);

	if (parseJsonErr.error == QJsonParseError::NoError)
	{
		if (dom.isObject()) {
			QJsonObject obj = dom.object();
			QJsonValue query = obj.value("Query");

			if (query.toString() == "succeed")
			{
				QJsonValue userId = obj.value("User_id");
				netSocket->setUserId(userId.toInt());
				return true;
			}
		}
	}
	return false;
}

void ParseData::parseHttpToMsgItem(NetSocket* netSocket, QList<MsgItemData>* pDataList)
{
	QByteArray json(netSocket->getHttp()->body.data());
	QJsonParseError parseJsonErr;
	QJsonDocument dom = QJsonDocument::fromJson(json, &parseJsonErr);

	if (parseJsonErr.error == QJsonParseError::NoError)
	{
		if (dom.isObject()) {
			QJsonObject obj = dom.object();
			QJsonValue query = obj.value("Query");

			if (query.toString() == "succeed")
			{
				QJsonValue val = obj.value("Array");
				QJsonArray array = val.toArray();

				for (int i = 0; i < array.size(); ++i){
					MsgItemData itemData;
					QJsonObject obj = array.at(i).toObject();
					QJsonValue userId = obj.value("User_id");
					QJsonValue Account = obj.value("Account");
					itemData.friendId = userId.toInt();
					itemData.firstStr = Account.toString();
					pDataList->push_back(itemData);
				}
			}
		}
	}
}

void ParseData::parseChatedData(NetSocket* netSocket, QList<tuple<QString, ChatMsgItem::User_Type>> *msg)
{

	QByteArray data(netSocket->getHttp()->body.data());
	QString json = QString::fromLocal8Bit(data.data());
	QJsonParseError parseJsonErr;
	QJsonDocument dom = QJsonDocument::fromJson(json.toUtf8(), &parseJsonErr);
	int friendId = netSocket->getChatObjectId();

	if (parseJsonErr.error == QJsonParseError::NoError)
	{
		if (dom.isObject()) {
			QJsonObject obj = dom.object();
			QJsonValue query = obj.value("Query");

			if (query.toString() == "succeed")
			{
				QJsonValue val = obj.value("Array");
				QJsonArray array = val.toArray();

				for (int i = 0; i < array.size(); ++i) {
					MsgItemData itemData;
					QJsonObject obj = array.at(i).toObject();
					QJsonValue sender_id = obj.value("Sender_id");
					QJsonValue receiver_id = obj.value("Receiver_id");
					QJsonValue send_time = obj.value("Send_time");
					QJsonValue dataMsg = obj.value("Msg");

					if (sender_id.toInt() == friendId) {
						auto tu = forward_as_tuple(dataMsg.toString(), ChatMsgItem::User_She);
						msg->push_back(tu);
					}
					else {
						auto tu = forward_as_tuple(dataMsg.toString(), ChatMsgItem::User_Me);
						msg->push_back(tu);
					}
				}
			}
		}
	}
}

void ParseData::parseChattingData(NetSocket* netSocket, QString* msg, int* senderId)
{
	QByteArray data(netSocket->getHttp()->body.data());
	QString json = QString::fromLocal8Bit(data.data());
	QJsonParseError parseJsonErr;
	QJsonDocument dom = QJsonDocument::fromJson(json.toUtf8(), &parseJsonErr);

	if (parseJsonErr.error == QJsonParseError::NoError)
	{
		if (dom.isObject()) {
			QJsonObject obj = dom.object();
			QJsonValue sender_id = obj.value("Sender_id");
			QJsonValue receiver_id = obj.value("Receiver_id");
			QJsonValue send_time = obj.value("Send_time");
			QJsonValue dataMsg = obj.value("Msg");
			*senderId = sender_id.toInt();
			msg->append(dataMsg.toString());
		}
	}
}

bool ParseData::parseSignUpData(NetSocket* netSocket)
{
	QByteArray json(netSocket->getHttp()->body.data());
	QJsonParseError parseJsonErr;
	QJsonDocument dom = QJsonDocument::fromJson(json, &parseJsonErr);
	if (parseJsonErr.error == QJsonParseError::NoError)
	{
		if (dom.isObject()) {
			QJsonObject obj = dom.object();
			QJsonValue signUp = obj.value("SignUp");
			return signUp.toBool();
		}
	}
}

void ParseData::parseNewFriendData(NetSocket* netSocket, MsgItemData* item, QString* type)
{
	QByteArray data(netSocket->getHttp()->body.data());
	QString json = QString::fromLocal8Bit(data.data());
	QJsonParseError parseJsonErr;
	QJsonDocument dom = QJsonDocument::fromJson(json.toUtf8(), &parseJsonErr);
	type->clear();

	if (parseJsonErr.error == QJsonParseError::NoError)
	{
		if (dom.isObject()) {
			QJsonObject obj = dom.object();
			if (obj.value("FoundFriend").toBool()) {
				if (obj.value("RelationNotExist").toBool()) {
					item->friendId = obj.value("FriendID").toInt();;
					item->firstStr = obj.value("FriendAccound").toString();;
					type->append("AddFriendSuccess");
				}
				else {
					type->append("TheFriendHasExist");
				}
			}
			else {
				type->append("NotFoundFriend");
			}
		}
	}
}
