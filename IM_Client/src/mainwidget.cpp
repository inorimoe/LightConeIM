#include "mainwidget.h"
#include "../lib/common.h"
#include "../lib/stylesheet.h"
#include <QVBoxLayout>
#include <QPainter>
#include <QMouseEvent>
#include <QApplication>

MainWidget::MainWidget(NetSocket* pNetSocket, Login* pLogin, QWidget* parent)
	: QWidget(parent)
	, m_pLogin(pLogin)
	, m_pNetSocket(pNetSocket)
	, m_pTitleWidget(nullptr)
	, m_pMainBarWidget(nullptr)
	, m_pMsgItemView(nullptr)
	, m_pChat(nullptr)
	, m_pChatLogoWidget(nullptr)
	, m_pAddFriendWidget(nullptr)
{
	iniUi();
	connect(m_pLogin, &Login::_startMainWidget, this, &MainWidget::onLoadData);
	connect(m_pTitleWidget, &TitleWidget::_closeMainWidget, this, &MainWidget::close);
	connect(m_pTitleWidget, &TitleWidget::_narrowMainWidget, this, &MainWidget::showMinimized);
	connect(m_pMsgItemView, &MsgItemView::_setChatObjectData, m_pChat, &Chat::onChatting);
	connect(m_pMsgItemView, &MsgItemView::_setChatObjectData, this, &MainWidget::onCloseLogoWdt);
}

void MainWidget::mousePressEvent(QMouseEvent* event)
{
	QWidget* curWdg = QApplication::widgetAt(QCursor::pos());
	if (curWdg == m_pTitleWidget || curWdg == m_pMainBarWidget)
	{
		m_pressFlag = true;
		m_beginDrag = event->pos();
	}
	QWidget::mousePressEvent(event);
}

void MainWidget::mouseReleaseEvent(QMouseEvent* event)
{
	m_pressFlag = false;
	QWidget::mouseReleaseEvent(event);
}

void MainWidget::mouseMoveEvent(QMouseEvent* event)
{
	if (m_pressFlag)
	{
		QPoint relaPos(QCursor::pos() - m_beginDrag);
		move(relaPos);
	}
	QWidget::mouseMoveEvent(event);
}
MainWidget::~MainWidget()
{
}

void MainWidget::iniUi()
{
	setWindowFlags(Qt::WindowMinMaxButtonsHint | Qt::FramelessWindowHint);
	setFixedSize(mainWdgWidth, mainWdgHeight);

	m_pTitleWidget = new TitleWidget(this);
	m_pMainBarWidget = new MainBarWidget(this);
	m_pMsgItemView = new MsgItemView(this, m_pNetSocket);
	m_pChat = new Chat(this, m_pNetSocket);
	m_pChatLogoWidget = new ChatLogoWidget(this);
	m_pAddFriendWidget = new AddFriendWdt(m_pNetSocket,this);

	m_pChat->hide();

	QVBoxLayout* pMainLayout = new QVBoxLayout(this);
	pMainLayout->setContentsMargins(0, 0, 0, 0);
	pMainLayout->setSpacing(0);
	QHBoxLayout* pChatLayout = new QHBoxLayout;
	pChatLayout->addWidget(m_pMainBarWidget);
	
	QVBoxLayout* pItemLayout = new QVBoxLayout;
	pItemLayout->addWidget(m_pAddFriendWidget);
	pItemLayout->addWidget(m_pMsgItemView);

	pChatLayout->addLayout(pItemLayout);
	pChatLayout->addWidget(m_pChat);
	pChatLayout->addWidget(m_pChatLogoWidget);
	pChatLayout->addStretch();

	pMainLayout->addWidget(m_pTitleWidget);
	pMainLayout->addLayout(pChatLayout);
	pMainLayout->addStretch();  
}

void MainWidget::onLoadData()
{
	m_pNetSocket->loadMsgItemData();
	//emit _loadDataReady();
	show();
}

void MainWidget::onCloseLogoWdt()
{
	m_pChatLogoWidget->hide();
	m_pChat->show();
}

///////////////////////TitleWidget///////////////////////////////////////////
TitleWidget::TitleWidget(QWidget* parent)
	: QWidget(parent)
	, m_pCloseBtn(nullptr)
	, m_pNarrowBtn(nullptr)
{
	iniUi();
	
	connect(m_pCloseBtn, &QPushButton::clicked, this, &TitleWidget::_closeMainWidget);
	connect(m_pNarrowBtn, &QPushButton::clicked, this, &TitleWidget::_narrowMainWidget);
}

TitleWidget::~TitleWidget()
{

}

void TitleWidget::iniUi()
{
	setFixedHeight(mainTitleHeight);
	m_pNarrowBtn = new QPushButton(this);

	m_pNarrowBtn->setFixedSize(mainTitleBtnLenth, mainTitleBtnLenth);
	m_pNarrowBtn->setStyleSheet((qTitleWdgBtnQSS)
		.arg("mainWdgNarrowBtn.png")
		.arg("mainWdgNarrowBtn_hov.png"));

	m_pCloseBtn = new QPushButton(this);
	m_pCloseBtn->setFixedSize(mainTitleBtnLenth, mainTitleBtnLenth);
	m_pCloseBtn->setStyleSheet((qTitleWdgBtnQSS)
		.arg("mainWdgCloseBtn.png")
		.arg("mainWdgCloseBtn_hov.png"));

	QHBoxLayout* pMainLayout = new QHBoxLayout(this);
	pMainLayout->setContentsMargins(0, 0, 0, 0);
	pMainLayout->setSpacing(0);
	pMainLayout->addStretch();
	pMainLayout->addWidget(m_pNarrowBtn);
	pMainLayout->addSpacing(mainTitleSpacing);
	pMainLayout->addWidget(m_pCloseBtn);
	pMainLayout->addSpacing(mainTitleSpacing);
}

void TitleWidget::paintEvent(QPaintEvent* e)
{
	QPainter p(this);
	p.setPen(Qt::NoPen);
	p.setBrush(QColor(mainBarColor));
	p.drawRect(0, 0, mainBarWidth, mainTitleHeight);
	p.setBrush(QColor(mainItemColor));
	p.drawRect(mainBarWidth, 0, mainItemWidth, mainTitleHeight);
	p.setBrush(QColor(mainChatColor));
	p.drawRect(mainBarWidth + mainItemWidth, 0, width()-(mainBarWidth + mainItemWidth), mainTitleHeight);
	QWidget::paintEvent(e);
}


/////////////////////////////////MainBarWidget/////////////////////////////////////////////
MainBarWidget::MainBarWidget(QWidget* parent)
	:QWidget(parent)
	, m_pChatBtn(nullptr)
	, m_pPeopleBtn(nullptr)
	, m_pDocumentBtn(nullptr)
	, m_pSettingBtn(nullptr)
	, m_pBtnList(QList<QPushButton*>())
	, m_pPreBtn(nullptr)
{
	iniUi();
	m_pBtnList.push_back(m_pChatBtn);
	m_pBtnList.push_back(m_pPeopleBtn);
	m_pBtnList.push_back(m_pDocumentBtn);
	m_pBtnList.push_back(m_pSettingBtn);
	m_pPreBtn = m_pChatBtn;
	connect(m_pChatBtn, &QPushButton::clicked, this, &MainBarWidget::onChangeWdg);
	connect(m_pPeopleBtn, &QPushButton::clicked, this, &MainBarWidget::onChangeWdg);
	connect(m_pDocumentBtn, &QPushButton::clicked, this, &MainBarWidget::onChangeWdg);
	connect(m_pSettingBtn, &QPushButton::clicked, this, &MainBarWidget::onChangeWdg);
}

MainBarWidget::~MainBarWidget()
{

}

void MainBarWidget::iniUi()
{
	setFixedSize(mainBarWidth, mainWdgHeight - mainTitleHeight);

	m_pChatBtn = new QPushButton(this);
	m_pChatBtn->setFixedHeight(mainBarBtnHeight);
	m_pChatBtn->setStyleSheet(qMainBarWdgBtnQSS
		.arg("chatBtn.png")
		.arg("chatBtn_cur.png")
	);
	m_pChatBtn->setCheckable(true);
	m_pChatBtn->setChecked(true);

	m_pPeopleBtn = new QPushButton(this);
	m_pPeopleBtn->setFixedHeight(mainBarBtnHeight);
	m_pPeopleBtn->setStyleSheet(qMainBarWdgBtnQSS
		.arg("peopleBtn.png")
		.arg("peopleBtn_cur.png")
	);
	m_pPeopleBtn->setCheckable(true);

	m_pDocumentBtn = new QPushButton(this);
	m_pDocumentBtn->setFixedHeight(mainBarBtnHeight);
	m_pDocumentBtn->setStyleSheet(qMainBarWdgBtnQSS
		.arg("documentBtn.png")
		.arg("documentBtn_cur.png")
	);
	m_pDocumentBtn->setCheckable(true);

	m_pSettingBtn = new QPushButton(this);
	m_pSettingBtn->setFixedHeight(mainBarBtnHeight);
	m_pSettingBtn->setStyleSheet(qMainBarWdgBtnQSS
		.arg("settingBtn.png")
		.arg("settingBtn_cur.png")
	);
	m_pSettingBtn->setCheckable(true);

	QVBoxLayout* pMainLayout = new QVBoxLayout(this);
	pMainLayout->addWidget(m_pChatBtn);
	pMainLayout->addWidget(m_pPeopleBtn);
	pMainLayout->addWidget(m_pDocumentBtn);
	pMainLayout->addStretch();
	pMainLayout->addWidget(m_pSettingBtn);
	setLayout(pMainLayout);
}

void MainBarWidget::paintEvent(QPaintEvent* e)
{
	QPainter p(this);
	p.fillRect(rect(), mainBarColor);
	QWidget::paintEvent(e);
}
void MainBarWidget::onChangeWdg()
{
	QPushButton* pBtn = dynamic_cast<QPushButton*>(sender());
	pBtn->setChecked(true);
	if(pBtn != m_pPreBtn)
		m_pPreBtn->setChecked(false);
	m_pPreBtn = pBtn;
	emit _changeWdg();
}

ChatLogoWidget::ChatLogoWidget(QWidget* parent)
	: QWidget(parent)
	, m_pLogo(nullptr)
{
	setFixedSize(chatWdtWidth, chatWdtHeight);
	m_pLogo = new QLabel(this);
	m_pLogo->setFixedSize(logoLenth, logoLenth);
	m_pLogo->setStyleSheet(qLoginLogoQSS.arg("logo.png"));

	QHBoxLayout* midLayout = new QHBoxLayout(this);
	midLayout->setContentsMargins(0, 0, 0, 200);

	midLayout->addStretch();
	midLayout->addWidget(m_pLogo);
	midLayout->addStretch();

}

ChatLogoWidget::~ChatLogoWidget()
{

}

AddFriendWdt::AddFriendWdt(NetSocket* pNetSocket, QWidget* parent)
	: QWidget(parent)
	, m_pInputText(nullptr)
	, m_pAddFriendBtn(nullptr)
	, m_pNetSocket(pNetSocket)
{
	m_pInputText = new QLineEdit(this);
	m_pAddFriendBtn = new QPushButton(this);

	initUi();
	connect(m_pAddFriendBtn, &QPushButton::clicked, this, &AddFriendWdt::onAddBtn);
}

AddFriendWdt::~AddFriendWdt()
{
}

void AddFriendWdt::initUi()
{
	setFixedSize(mainItemWidth, chatTitleHeight);
	setContentsMargins(0, 0, 0, 0);
	setStyleSheet(QString("QWidget{background:transparent;}").arg(mainItemColor));

	m_pInputText->setFixedSize( 200, chatTitleHeight-5);
	m_pInputText->setStyleSheet(QString("QWidget{background:#DBD9D8;border:none;font-size:16px;border-radius:10px;}"));
	m_pInputText->setTextMargins(5, 0, 0, 0);

	m_pAddFriendBtn->setFixedSize(chatTitleHeight-7 , chatTitleHeight-7);
	m_pAddFriendBtn->setStyleSheet(qAddFriendBtnQSS);

	QHBoxLayout* pMainLayout = new QHBoxLayout(this);
	pMainLayout->setContentsMargins(0, 0, 0, 0);
	pMainLayout->addStretch();
	pMainLayout->addWidget(m_pInputText);
	pMainLayout->addSpacing(6);
	pMainLayout->addWidget(m_pAddFriendBtn);
	pMainLayout->addStretch();
}

void AddFriendWdt::paintEvent(QPaintEvent* event)
{
	QPainter p(this);
	p.fillRect(rect(), mainItemColor);
	QWidget::paintEvent(event);
}

void AddFriendWdt::onAddBtn() 
{
	m_text = m_pInputText->text();
	if (!m_text.isEmpty())
	{
		m_pNetSocket->addFriend(m_text);
	}
}