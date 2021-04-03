#pragma once
#include <QWidget>
#include <QListWidget>
#include <QResizeEvent>
#include <QTextEdit>
#include "netsocket.h"
#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>
#include "chatmsgitem.h"
#include "msgitemview.h"
#include <QDateTime>
#include <QScrollBar>
#include <tuple> 
#include <list>

class ChatTitle :public QWidget {
	Q_OBJECT
public:
	explicit ChatTitle(QWidget* parent);
	~ChatTitle();
public:
	void initUi();
	void setTitle(QString text);
private:
	QLabel*              m_pTitle;
};

class Chat :public QWidget {
	Q_OBJECT
public:
	explicit Chat(QWidget* parent, NetSocket* pNetSocket);
	~Chat();
public:
	void initUi();
	void insertChatMsg(QString msg, ChatMsgItem::User_Type type);
public slots:
	void onSendChatMsg();
	void onChatting(MsgItemData var);
	void onLoadChatedData();
	void onLoadChattingData();
protected:
	virtual void resizeEvent(QResizeEvent* event) override;
private:
	NetSocket*           m_pNetSocket;
	ChatTitle*           m_pChatTitle;
	QListWidget*         m_pListWidget;
	QTextEdit*           m_pTextEdit;
	QPushButton*         m_pSendMsgBtn;

	QString              m_ChatObjName;
	QPixmap              m_ChatObjImage;
	int                  m_ChatObjId;
};