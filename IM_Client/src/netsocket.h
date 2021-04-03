#pragma once
#include <QTcpSocket>
#include <QNetworkProxy>
#include <QObject>
#include <QJsonDocument>
#include <QJsonParseError>
#include <QJsonObject>
#include <QJsonValue>
#include <string>
#include <map>
using namespace std;

enum HttpRequestState {
	REQUEST_STATUS,    //等待解析状态行
	REQUEST_HEADERS,   //等待解析headers
	REQUEST_BODY,      //等待解析请求body
	REQUEST_DONE       //解析完成
};

struct http {
	//request
	string method;
	string url;
	string version;//response
	string statusCode;
	string statusMessage;
	map<string, string> headers;
	string body;
	enum HttpRequestState currentState = REQUEST_STATUS;
};

typedef struct http HTTP;

enum ConnectState {
	default = 0,
	success,
	fail,
};

class NetSocket : public QObject {
	Q_OBJECT
public:
	NetSocket(QObject* parent = nullptr);
	~NetSocket();
	friend class Login;
	friend class MainWidget;
signals:
	void _connectSuccess();
	void _dataParseReady();
	void _msgItemDataReady();
	void _chatedDataReady();
	void _chattingDataReady();
	void _signUpDataReady();
	void _addFriendDataReady();
public slots:
	void onConnect();
	void onReceiveData();
public:
	void connectServer();
	HTTP* getHttp();
	void setUserId(int userId);
	void sendLoginData(QString account, QString password);
	void setChatObjectId(int Id);
	int  getChatObjectId();
	void loadMsgItemData();
	void sendChatObjData(const QString& msg);
	void loadChatObjDatas();
	void signUpAccount(QString account, QString password);
	void addFriend(QString account);
private:
	QTcpSocket*               m_pTcpSocket;
	QString                   m_ip;
	quint16                   m_port;
	enum ConnectState         m_connState;

	QString              m_account;
	QString              m_password;
	int                  m_userId;
	int                  m_chatObjectId;
	QByteArray           m_data; //receive data
	HTTP                 m_http;
	bool                 m_hasRecvHttpHeaders;
};