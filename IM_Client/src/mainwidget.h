#pragma once
#include "login.h"
#include "msgitemview.h"
#include "netsocket.h"
#include "chat.h"

class TitleWidget;
class MainBarWidget;
class ChatLogoWidget;
class AddFriendWdt;

class MainWidget : public QWidget
{
	Q_OBJECT
public:
	explicit MainWidget(NetSocket* pNetSocket, Login* pLogin, QWidget* parent = nullptr);
	~MainWidget();
signals:
	void _loadDataReady();
public:
	void iniUi();
public slots:
	void onLoadData();
	void onCloseLogoWdt();
protected:
	void mousePressEvent(QMouseEvent* event) override;
	void mouseReleaseEvent(QMouseEvent* event) override;
	void mouseMoveEvent(QMouseEvent* event) override;
private:
	Login*              m_pLogin;
	TitleWidget*        m_pTitleWidget;
	MainBarWidget*      m_pMainBarWidget;
	AddFriendWdt*       m_pAddFriendWidget;
	MsgItemView*        m_pMsgItemView;
	ChatLogoWidget*     m_pChatLogoWidget;
	Chat*               m_pChat;
	NetSocket*          m_pNetSocket;

	bool                m_pressFlag;
	QPoint              m_beginDrag;
};

class TitleWidget : public QWidget
{
	Q_OBJECT
public:
	explicit TitleWidget(QWidget* parent = nullptr);
	~TitleWidget();
	void iniUi();
signals:
	void _closeMainWidget();
	void _narrowMainWidget();
protected:
	void paintEvent(QPaintEvent* e) override;
private:
	QPushButton*         m_pCloseBtn;
	QPushButton*         m_pNarrowBtn;
};

class MainBarWidget :public QWidget
{
	Q_OBJECT
public:
	explicit MainBarWidget(QWidget* parent = nullptr);
	~MainBarWidget();
	void iniUi();
signals:
	void _changeWdg();
public slots:
	void onChangeWdg();
protected:
	void paintEvent(QPaintEvent* e) override;
private:
	QPushButton*           m_pChatBtn;
	QPushButton*           m_pPeopleBtn;
	QPushButton*           m_pDocumentBtn;
	QPushButton*           m_pSettingBtn;
	QList<QPushButton*>    m_pBtnList;
	QPushButton*           m_pPreBtn;
};

class ChatLogoWidget : public  QWidget
{
	Q_OBJECT
public:
	explicit ChatLogoWidget(QWidget* parent = nullptr);
	~ChatLogoWidget();
private:
	QLabel*               m_pLogo;
};

class AddFriendWdt :public QWidget
{
	Q_OBJECT
public:
	explicit AddFriendWdt(NetSocket* pNetSocket, QWidget* parent = nullptr);
	~AddFriendWdt();
public slots:
	void onAddBtn();
public:
	void initUi();
protected:
	virtual void paintEvent(QPaintEvent* event) override;
private:
	QString               m_text;
	QLineEdit*            m_pInputText;
	QPushButton*          m_pAddFriendBtn;
	NetSocket*            m_pNetSocket;
};