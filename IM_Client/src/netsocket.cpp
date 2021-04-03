#include "netsocket.h"
#include <QDeBug>
#include <QHostAddress>
#include "login.h"
#include "parsedata.h"
#include "../lib/common.h"

NetSocket::NetSocket(QObject* parent)
	: QObject(parent)
	, m_pTcpSocket(nullptr)
    , m_connState(ConnectState::default)
{
	m_ip = "192.168.182.129";
	m_port = 43211;
	m_pTcpSocket = new QTcpSocket(this);
    m_pTcpSocket->setProxy(QNetworkProxy::NoProxy);

    m_chatObjectId = -1;
    m_hasRecvHttpHeaders = false;

    connect(m_pTcpSocket, &QTcpSocket::connected, this, &NetSocket::onConnect);
    connect(m_pTcpSocket, &QTcpSocket::readyRead, this, &NetSocket::onReceiveData);
}

NetSocket::~NetSocket()
{
    m_pTcpSocket->disconnectFromHost();
    m_pTcpSocket->close();
}

void NetSocket::connectServer()
{
    if (m_pTcpSocket->isOpen() == false) {
        m_pTcpSocket->connectToHost(QHostAddress(m_ip), m_port);
    }
}

void NetSocket::sendLoginData(QString account, QString password)
{
    QString http = "GET /? HTTP/1.1\r\n";
    http += QString("MessageType: login") + QString(CRLF);
    http += QString("Account: ") + account + QString(CRLF);
    http += QString("Password: ") + password + QString(CRLF);
    http += QString(CRLF);
    http += QString("connectServer!!!");

    m_pTcpSocket->write(http.toLatin1().constData());
}

void NetSocket::onReceiveData()
{

    if (m_hasRecvHttpHeaders == false) {
        m_data = m_pTcpSocket->readAll();
        m_http = ParseData::parseHttpResponse(&m_data);
        m_hasRecvHttpHeaders = true;
    }
    else if (m_hasRecvHttpHeaders == true) {
        m_http.body.append(m_pTcpSocket->readAll());
    }

    if (m_http.body.size() == stoi(m_http.headers.at("Content-Length"))) {
        m_hasRecvHttpHeaders = false;

        string MessageType = m_http.headers.at("MessageType");
        if (MessageType == "login") {
            emit _dataParseReady();
        }
        else if (MessageType == "msgItemData") {
            emit _msgItemDataReady();
        }
        else if (MessageType == "chatedData") {
            emit _chatedDataReady();
        }
        else if (MessageType == "chattingData") {
            emit _chattingDataReady();
        }
        else if (MessageType == "signUp") {
            emit _signUpDataReady();
        }
        else if (MessageType == "addFriend") {
            emit _addFriendDataReady();
        }
    }

}

void NetSocket::onConnect()
{
    m_connState = ConnectState::success;
    emit _connectSuccess();
}

HTTP* NetSocket::getHttp() 
{
    return &m_http;
}

void NetSocket::setUserId(int userId)
{
    m_userId = userId;
}

void NetSocket::setChatObjectId(int Id)
{
    m_chatObjectId = Id;
}

int NetSocket::getChatObjectId()
{
    return m_chatObjectId;
}

void NetSocket::loadMsgItemData()
{
    QString http = "GET /? HTTP/1.1\r\n";
    http += QString("MessageType: msgItemData") + QString(CRLF);
    http += QString("User_id: ") + QString::number(m_userId) + QString(CRLF);
    http += QString(CRLF);
    http += QString("loadMsgItemData!!!");

    m_pTcpSocket->write(http.toLocal8Bit().constData());
}

void NetSocket::loadChatObjDatas()
{
    QString http = "POST /? HTTP/1.1\r\n";
    http += QString("MessageType: getChatedData") + QString(CRLF);
    http += QString(CRLF);

    QJsonObject jsonObj;
    jsonObj.insert("User_id", m_userId);
    jsonObj.insert("Friend_id", m_chatObjectId);
    QJsonDocument dom(jsonObj);
    http += dom.toJson(QJsonDocument::Compact).data();

    m_pTcpSocket->write(http.toLocal8Bit().constData());
}

void NetSocket::sendChatObjData(const QString& msg)
{
    if (m_chatObjectId > 0) {
        QString http = "POST /? HTTP/1.1\r\n";
        http += QString("MessageType: senNewChatData") + QString(CRLF);
        http += QString(CRLF);

        QJsonObject jsonObj;
        jsonObj.insert("Sender_id", m_userId);
        jsonObj.insert("Received_id", m_chatObjectId);
        jsonObj.insert("Msg", msg);
        QJsonDocument dom(jsonObj);
        http += dom.toJson(QJsonDocument::Compact).data();

        m_pTcpSocket->write(http.toLocal8Bit().constData());
    }
}

void NetSocket::signUpAccount(QString account, QString password)
{
    QString http = "POST /? HTTP/1.1\r\n";
    http += QString("MessageType: signUp") + QString(CRLF);
    http += QString("Account: ") + account + QString(CRLF);
    http += QString("Password: ") + password + QString(CRLF);
    http += QString(CRLF);
    http += QString("signUpAccount!!!");

    m_pTcpSocket->write(http.toLocal8Bit().constData());
}

void NetSocket::addFriend(QString account)
{
    QString http = "POST /? HTTP/1.1\r\n";
    http += QString("MessageType: addFriend") + QString(CRLF);
    http += QString("User_id: ") + QString::number(m_userId) + QString(CRLF);
    http += QString("Friend_account: ") + account + QString(CRLF);
    http += QString(CRLF);
    http += QString("addFriend!!!");

    m_pTcpSocket->write(http.toLocal8Bit().constData());
}