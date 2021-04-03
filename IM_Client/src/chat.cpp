#include "chat.h"
#include "parsedata.h"
#include "../lib/common.h"
#include "../lib/stylesheet.h"

ChatTitle::ChatTitle(QWidget* parent)
	: QWidget(parent)
	, m_pTitle(nullptr)
{
	m_pTitle = new QLabel(this);
	m_pTitle->setText("TEXT_TEXT_TEXT!");
	initUi();
}
ChatTitle::~ChatTitle() 
{
};

void ChatTitle::initUi() 
{
	setFixedSize(chatWdtWidth, chatTitleHeight);
	setContentsMargins(0, 0, 0, 10);
	setStyleSheet(QString("ChatTitle{background:%0;}").arg(chatTitleColor));
	m_pTitle->setStyleSheet(QString("QLabel{font-size:18px;font-weight:bold;color:#808080;}"));
	QHBoxLayout* pMainLayout = new QHBoxLayout(this);
	pMainLayout->setContentsMargins(0, 0, 0, 0);

	pMainLayout->addSpacing(30);
	pMainLayout->addWidget(m_pTitle);
	pMainLayout->addStretch();
}

void ChatTitle::setTitle(QString text)
{
	m_pTitle->setText(text);
}

///////////////////////////Chat/////////////////////////////////////////
 
Chat::Chat(QWidget* parent, NetSocket* pNetSocket)
	: QWidget(parent)
	, m_pNetSocket(pNetSocket)
	, m_pListWidget(nullptr)
	, m_pTextEdit(nullptr)
	, m_pChatTitle(nullptr)
	, m_pSendMsgBtn(nullptr)
{
	m_pListWidget = new QListWidget(this);
	m_pTextEdit = new QTextEdit(this);
	m_pChatTitle = new ChatTitle(this);
	m_pSendMsgBtn = new QPushButton(this);

	initUi();
	connect(m_pSendMsgBtn, &QPushButton::clicked, this, &Chat::onSendChatMsg);
	connect(m_pNetSocket, &NetSocket::_chatedDataReady, this, &Chat::onLoadChatedData);
	connect(m_pNetSocket, &NetSocket::_chattingDataReady, this, &Chat::onLoadChattingData);
}

Chat::~Chat()
{

}

void Chat::initUi()
{
	setFixedSize(chatWdtWidth, chatWdtHeight);

	m_pTextEdit->setFixedSize(inputTextChatWidth, inputTextChatHeight);
	m_pTextEdit->setStyleSheet(qChatTextEditQSS);

	m_pListWidget->setFixedSize(chatListWidgetWidth, chatListWidgetHeight);
	m_pListWidget->setStyleSheet(qChatListWidgetQSS);
	m_pListWidget->setContentsMargins(0, 0, 0, 0);
	m_pListWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	m_pListWidget->verticalScrollBar()->setStyleSheet(qChatListWdtVerScrollBarQSS);
	m_pListWidget->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
	m_pListWidget->setFocusPolicy(Qt::NoFocus);

	m_pSendMsgBtn->setFixedSize(60, 25);
	m_pSendMsgBtn->setStyleSheet(qSendMsgBtnQSS);
	m_pSendMsgBtn->setText(tr("send"));
	int sendMsgX = m_pTextEdit->x() + inputTextChatWidth - 80;
	int sendMsgY = m_pTextEdit->y() + chatWdtHeight - 40;
	m_pSendMsgBtn->move(sendMsgX, sendMsgY);

	QVBoxLayout* pMainLayout = new QVBoxLayout(this);
	pMainLayout->setContentsMargins(0, 0, 0, 0);
	pMainLayout->setSpacing(0);

	pMainLayout->addWidget(m_pChatTitle);
	pMainLayout->addWidget(m_pListWidget);
	pMainLayout->addWidget(m_pTextEdit);

}

void Chat::insertChatMsg(QString msg, ChatMsgItem::User_Type type)
{
	QString time = QString::number(QDateTime::currentDateTime().toTime_t()); //时间戳
	bool isSending = true;
	qDebug() << "addMessage" << msg << time << m_pListWidget->count();

	ChatMsgItem* msgItem = new ChatMsgItem(m_pListWidget);
	QListWidgetItem* item = new QListWidgetItem(m_pListWidget);
	item->setFlags(item->flags() & ~Qt::ItemIsSelectable);

	msgItem->setFixedWidth(this->width());
	QSize size = msgItem->fontRect(msg);
	item->setSizeHint(size);
	msgItem->setText(msg, time, size, type);
	m_pListWidget->setItemWidget(item, msgItem);
	m_pListWidget->setCurrentRow(m_pListWidget->count() - 1);
	m_pListWidget->update();
}

void Chat::onSendChatMsg() 
{
	QString msg = m_pTextEdit->toPlainText();
	m_pTextEdit->setText("");
	insertChatMsg(msg, ChatMsgItem::User_Me);
	m_pNetSocket->sendChatObjData(msg);
}

void Chat::onChatting(MsgItemData var)
{
	if (m_ChatObjId != var.friendId) 
	{
		m_ChatObjName = var.firstStr;
		//m_ChatObjImage = var.image;
		m_ChatObjId = var.friendId;

		m_pChatTitle->setTitle(m_ChatObjName);
		m_pNetSocket->loadChatObjDatas();
	}
}

void Chat::onLoadChatedData()
{
	QList<tuple<QString, ChatMsgItem::User_Type>> msg;
	ParseData::parseChatedData(m_pNetSocket, &msg);

	m_pListWidget->clear();
	for (int i = 0; i < msg.size(); ++i)
	{
		insertChatMsg(std::get<0>(msg[i]), std::get<1>(msg[i]));
	}
}

void Chat::onLoadChattingData()
{
	QString msg;
	int senderId;
	ParseData::parseChattingData(m_pNetSocket, &msg, &senderId);
	if(senderId == m_pNetSocket->getChatObjectId())
		insertChatMsg(msg, ChatMsgItem::User_She);
	//还可以设置Msgitem的第二行
}

void Chat::resizeEvent(QResizeEvent* event)
{
	QWidget::resizeEvent(event);
}

