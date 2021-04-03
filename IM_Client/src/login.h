#pragma once

#include <QWidget>
#include <QString>
#include <QLabel>
#include <QLineedit>
#include <QPushButton>
#include <QTimer>
#include "netsocket.h"
#include "parsedata.h"

enum ErrCode {
    noError,
    inputNull,
    accountError,
    passwordError
};

class Login : public QWidget 
{
    Q_OBJECT
public:
    explicit Login(NetSocket* pNetSocket, QWidget *parent = nullptr);
    friend class MainWidget;
    ~Login();
    void initUi();
signals:
    void _startMainWidget();
public slots:
    void onLogin();
    void onCheckReceivedData();
    void onSignUp();
    void onShowSignUpWdt();
    void onReactSignUp();
    void onShowLoginWdt();
protected:
    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
private:
    int checkInputCorrect();
    void showErrorMsg(int errCode);
private:
    NetSocket*     m_pNetSocket;
    int            m_errCode;
    QString        m_account;
    QString        m_password;
    bool           m_pressFlag;
    QPoint         m_beginDrag;
    bool           m_canSignUp;

    QLabel*        m_pLogo;
    QLineEdit*     m_pAccount;
    QLineEdit*     m_pPassword;
    QPushButton*   m_pCloseBtn;
    QPushButton*   m_pLoginBtn;
    QPushButton*   m_pSignUpBtn;
    QPushButton*   m_pSignUpText;
    QPushButton*   m_pLoginText;
    QLabel*        m_pShowSignUpState;
    QLabel*        m_pSignUpSucessed;
    QLabel*        m_pSignUpFailed;
};
