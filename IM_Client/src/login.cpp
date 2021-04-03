#include "login.h"
#include "../lib/common.h"
#include "../lib/stylesheet.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineedit>
#include <QPushButton>
#include <QCoreApplication>
#include <QMouseEvent>
#include <QPalette>
#include <QPoint>


Login::Login(NetSocket* pNetSocket, QWidget *parent)
    : QWidget(parent)
    , m_pNetSocket(pNetSocket)
    , m_pLogo(nullptr)
    , m_pAccount(nullptr)
    , m_pPassword(nullptr)
    , m_pCloseBtn(nullptr)
    , m_pLoginBtn(nullptr)
    , m_pSignUpBtn(nullptr)
    , m_pSignUpText(nullptr)
    , m_pLoginText(nullptr)
    , m_pShowSignUpState(nullptr)
    , m_pSignUpSucessed(nullptr)
    , m_pSignUpFailed(nullptr)
{
    setWindowFlags(Qt::WindowMinMaxButtonsHint | Qt::FramelessWindowHint);
    m_canSignUp = true;
    initUi();
    //show();
    connect(m_pCloseBtn, &QPushButton::clicked, this, &Login::close);
    connect(m_pLoginBtn, &QPushButton::clicked, this, &Login::onLogin);
    connect(m_pSignUpText, &QPushButton::clicked, this, &Login::onShowSignUpWdt);
    connect(m_pLoginText, &QPushButton::clicked, this, &Login::onShowLoginWdt);
    connect(m_pSignUpBtn, &QPushButton::clicked, this, &Login::onSignUp);
    connect(m_pNetSocket, &NetSocket::_dataParseReady, this, &Login::onCheckReceivedData);
    connect(m_pNetSocket, &NetSocket::_signUpDataReady, this, &Login::onReactSignUp);
}

Login::~Login()
{
}

void Login::initUi()
{
    setWindowFlags(Qt::WindowMinMaxButtonsHint | Qt::FramelessWindowHint);
    setFixedSize(loginWidgetWidth, loginWidgetHeight);
    setStyleSheet(QString("QWidget{background-color:%0;}").arg(_backgroundColor));

    m_pCloseBtn = new QPushButton(this);
    m_pCloseBtn->setFixedSize(closeBtnLenth, closeBtnLenth);
    m_pCloseBtn->setStyleSheet(qLoginCloseBtnQSS
        .arg("closeBtn.png")
        .arg("closeBtn_hov.png"));

    m_pLogo = new QLabel(this);
    m_pLogo->setFixedSize(logoLenth, logoLenth);
    m_pLogo->setStyleSheet(qLoginLogoQSS.arg("logo.png"));
   
    m_pAccount = new QLineEdit(this);
    m_pAccount->setFixedSize(lineEditWidth, lineEditHeight);
    m_pAccount->setStyleSheet(qLoginLineEditQSS
        .arg(lineEditFontSize)
        .arg(defaultColor)
        .arg(fontColor));
    m_pAccount->setTextMargins(textMarginLeft, 0, 0, 0);
    m_pAccount->setPlaceholderText(tr("account"));
    QPalette paletteAccount = m_pAccount->palette();
    paletteAccount.setColor(QPalette::Normal, QPalette::PlaceholderText, defaultColor);
    m_pAccount->setPalette(paletteAccount);

    m_pPassword = new QLineEdit(this);
    m_pPassword->setFixedSize(lineEditWidth, lineEditHeight);
    m_pPassword->setStyleSheet(qLoginLineEditQSS
        .arg(lineEditFontSize)
        .arg(defaultColor)
        .arg(fontColor));
    m_pPassword->setTextMargins(textMarginLeft, 0, 0, 0);
    //m_pPassword->setEchoMode(QLineEdit::Password);
    m_pPassword->setPlaceholderText(tr("password"));
    QPalette palettePassword = m_pPassword->palette();
    palettePassword.setColor(QPalette::Normal, QPalette::PlaceholderText, defaultColor);
    m_pPassword->setPalette(palettePassword);

    m_pLoginBtn = new QPushButton(this);
    m_pLoginBtn->setFixedSize(loginBtnLenth, loginBtnLenth);
    m_pLoginBtn->setStyleSheet(qLoginBtnQSS
        .arg("loginBtn.png")
        .arg("loginBtn_hov.png"));

    m_pSignUpBtn = new QPushButton(this);
    m_pSignUpBtn->setFixedSize(loginBtnLenth, loginBtnLenth);
    m_pSignUpBtn->setStyleSheet(qLoginBtnQSS
        .arg("signUpBtn.png")
        .arg("signUpBtn_hov.png"));
    m_pSignUpBtn->hide();

    m_pSignUpText = new QPushButton(this);
    m_pSignUpText->setStyleSheet(qSignUpTextQSS
        .arg(defaultColor)
        .arg(fontColor)
        .arg(signUpTextSize));
    m_pSignUpText->setText(tr("signup"));

    m_pLoginText = new QPushButton(this);
    m_pLoginText->setStyleSheet(qSignUpTextQSS
        .arg(defaultColor)
        .arg(fontColor)
        .arg(signUpTextSize));
    m_pLoginText->setText(tr("login"));
    m_pLoginText->hide();

    m_pShowSignUpState = new QLabel(this);
    m_pShowSignUpState->setText(tr("inSignup"));
    m_pShowSignUpState->setStyleSheet(qSignUpTextStateQSS
        .arg(signUpStateColor)
        .arg(signUpTextStateSize));
    m_pShowSignUpState->move(signUpTextStateLeftMargin
                , signUpTextStateTopMargin - m_pShowSignUpState->height()/2);
    m_pShowSignUpState->hide();

    m_pSignUpSucessed = new QLabel(this);
    m_pSignUpSucessed->setText(tr("signUpSuccessed"));
    m_pSignUpSucessed->setStyleSheet(qSignUpTextStateQSS
        .arg(signUpSuccessed)
        .arg(signUpTextStateSize));
    m_pSignUpSucessed->move(signUpTextStateLeftMargin
        , signUpTextStateTopMargin - m_pShowSignUpState->height() / 2);
    m_pSignUpSucessed->hide();

    m_pSignUpFailed = new QLabel(this);
    m_pSignUpFailed->setText(tr("signUpFailed"));
    m_pSignUpFailed->setStyleSheet(qSignUpTextStateQSS
        .arg(signUpFailed)
        .arg(signUpTextStateSize));
    m_pSignUpFailed->move(signUpTextStateLeftMargin
        , signUpTextStateTopMargin - m_pShowSignUpState->height() / 2);
    m_pSignUpFailed->hide();

    QVBoxLayout* pMainLayout = new QVBoxLayout(this);
    pMainLayout->setContentsMargins(0, 0, 0, 0);
    pMainLayout->setSpacing(0);

    QHBoxLayout* pTitleLayout = new QHBoxLayout;
    pTitleLayout->setContentsMargins(0, closeBtnMargin, closeBtnMargin, 0);
    pTitleLayout->setSpacing(0);
    pTitleLayout->addStretch();
    pTitleLayout->addWidget(m_pCloseBtn);

    QHBoxLayout* pLogoLayout = new QHBoxLayout;
    pLogoLayout->addStretch();
    pLogoLayout->addWidget(m_pLogo);
    pLogoLayout->addStretch();

    QHBoxLayout* pAccountLayout = new QHBoxLayout;
    pAccountLayout->addStretch();
    pAccountLayout->addWidget(m_pAccount);
    pAccountLayout->addStretch();

    QHBoxLayout* pPasswordLayout = new QHBoxLayout;
    pPasswordLayout->addStretch();
    pPasswordLayout->addWidget(m_pPassword);
    pPasswordLayout->addStretch();

    QHBoxLayout* pLoginBtnLayout = new QHBoxLayout;
    pLoginBtnLayout->addStretch();
    pLoginBtnLayout->addWidget(m_pLoginBtn);
    pLoginBtnLayout->addWidget(m_pSignUpBtn);
    pLoginBtnLayout->addSpacing(loginBtnRightMargin);

    QHBoxLayout* pSignUpLayout = new QHBoxLayout;
    pSignUpLayout->addStretch();
    pSignUpLayout->addWidget(m_pSignUpText);
    pSignUpLayout->addWidget(m_pLoginText);
    pSignUpLayout->addSpacing(signUpTextRightMargin);

    pMainLayout->addLayout(pTitleLayout);
    pMainLayout->addSpacing(100);
    pMainLayout->addLayout(pLogoLayout);
    pMainLayout->addSpacing(50);
    pMainLayout->addLayout(pAccountLayout);
    pMainLayout->addSpacing(10);
    pMainLayout->addLayout(pPasswordLayout);
    pMainLayout->addSpacing(20);
    pMainLayout->addLayout(pLoginBtnLayout);
    pMainLayout->addStretch();
    pMainLayout->addLayout(pSignUpLayout);
    pMainLayout->addSpacing(20);

    setLayout(pMainLayout);
}

void Login::onLogin()
{
    m_errCode = checkInputCorrect();
    showErrorMsg(m_errCode);
    if (m_errCode == ErrCode::noError) {
        m_pNetSocket->connectServer();
        m_pNetSocket->sendLoginData(m_account, m_password);
    }
}

void Login::onCheckReceivedData()
{
    bool id = ParseData::parseHttpFromLogin(m_pNetSocket);
    if (id == false) {
        m_errCode = 1;//1 is ID error;
        showErrorMsg(m_errCode);
    }
    else {
        close();
        emit _startMainWidget();
    }
}

void Login::mousePressEvent(QMouseEvent* event)
{
    m_pressFlag = true;
    m_beginDrag = event->pos();
    QWidget::mousePressEvent(event);
}

void Login::mouseReleaseEvent(QMouseEvent* event)
{
    m_pressFlag = false;
    QWidget::mouseReleaseEvent(event);
}

void Login::mouseMoveEvent(QMouseEvent* event)
{
    if (m_pressFlag)
    {
        QPoint relaPos(QCursor::pos() - m_beginDrag);
        move(relaPos);
    }
    QWidget::mouseMoveEvent(event);
}

int Login::checkInputCorrect()
{
    //error input
    //...code...

    //correct input
    m_account = m_pAccount->text();
    m_password = m_pPassword->text();
    if (m_account.isEmpty() || m_password.isEmpty()) {
        return ErrCode::inputNull;
    }


    if (0) {//TEST
        m_account = QString("root");
        m_password = QString("root");
    }
    return ErrCode::noError;
}

void Login::showErrorMsg(int errCode)
{
}

void Login::onShowSignUpWdt()
{
    m_pLoginBtn->hide();
    m_pSignUpBtn->show();
    m_pSignUpText->hide();
    m_pLoginText->show();
    m_pShowSignUpState->show();

    m_pAccount->setText("");
    m_pPassword->setText("");
}

void Login::onSignUp()
{
    if (m_canSignUp 
        && !m_pAccount->text().isEmpty()
        && !m_pPassword->text().isEmpty())
    {
        m_account = m_pAccount->text();
        m_password = m_pPassword->text();
        m_pNetSocket->connectServer();
        m_canSignUp = false;
        m_pNetSocket->signUpAccount(m_account, m_password);
    }
}

void Login::onReactSignUp()
{
    bool signUpOk = ParseData::parseSignUpData(m_pNetSocket);
    if (signUpOk){
        m_pShowSignUpState->hide();
        m_pSignUpSucessed->show();

        QTimer::singleShot(3000, this, [&](){
            m_pSignUpSucessed->hide();
            m_pLoginBtn->show();
            m_pSignUpBtn->hide();
            m_pSignUpText->show();
            m_pLoginText->hide();

            m_pAccount->clear();
            m_pPassword->clear();
            m_canSignUp = true;
        });
    }
    else {
        m_pShowSignUpState->hide();
        m_pSignUpFailed->show();

        QTimer::singleShot(3000, this, [&]() {
            m_pSignUpFailed->hide();
            m_pShowSignUpState->show();
            m_canSignUp = true;
        });
    }
}

void Login::onShowLoginWdt()
{
    m_pShowSignUpState->hide();
    m_pSignUpSucessed->hide();
    m_pSignUpFailed->hide();
    m_pLoginBtn->show();
    m_pSignUpBtn->hide();
    m_pSignUpText->show();
    m_pLoginText->hide();

    m_pAccount->clear();
    m_pPassword->clear();
    m_canSignUp = true;
}


