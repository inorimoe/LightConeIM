#include "stylesheet.h"
#include <QString>
//Login
QString qLoginCloseBtnQSS = QString(
	"QPushButton{border:none;background-image:url(./resources/image/%0);"
	"background-repeat:no-repeat;background-position:center center;}"
	"QPushButton:hover{background-image:url(./resources/image/%1);}"
);

QString qLoginLogoQSS = QString(
	"QLabel{background-image:url(./resources/image/%0);"
	"background-repeat:no-repeat;background-position:center center;}"
);

QString qLoginLineEditQSS = QString{
	"QLineEdit{border-top-color:transparent;font-size:%0px;border:2px solid transparent;"
	"border-bottom-color:%1;color:%2;}"
};

QString qLoginBtnQSS = QString{
	"QPushButton{border:none;background-image:url(./resources/image/%0);"
	"background-repeat:no-repeat;background-position:top right;}"
	"QPushButton:hover{background-image:url(./resources/image/%1);}"
};

QString qSignUpTextQSS = QString{
	"QPushButton{border:none;background-color:transparent;color:%0;font-size:%2px;}"
	"QPushButton:hover{color:%1;}"
};

QString qSignUpTextStateQSS = QString{
	"QLabel{color: %0;background-color:transparent;font-size:%1px;}"
};

QString qTitleWdgBtnQSS = QString{
	"QPushButton{border:none;background-image:url(./resources/image/%0);"
	"background-repeat:no-repeat;background-position:center center;}"
	"QPushButton:hover{background-image:url(./resources/image/%1);}"
};

QString qMainBarWdgBtnQSS = QString{
	"QPushButton{border:none;background-image:url(./resources/image/%0);"
	"background-repeat:no-repeat;background-position:center center;}"
	"QPushButton:hover,QPushButton:checked{background-image:url(./resources/image/%1);}"
};

QString qItemStringQSS = QString{
	"QLabel{background-color:transparent;font-size:%0px;color:%1;}"
};

QString qItemBackgroundQSS = QString{
	"QWidget{background-color:%0;}"
	"QWidget:hover{background-color:%1;}"
};

QString qMsgItemViewQSS = QString{
	"QListView{border:none;background:%0;}"
};

QString qMsgItemViewvVerScrollBarQSS = QString{
	"QScrollBar:vertical{width:10px;background:transparent;}"
	"QScrollBar::add-page,QScrollBar::sub-page{background:none;}"
	"QScrollBar::add-line,QScrollBar::sub-line{background:none;border:none;}"
	"QScrollBar::handle:vertical{background:#DBD9D8;border-radius:5px;}"
	"QScrollBar::handle:vertical:hover{background:#bdbdbd;}"
};

QString qChatTextEditQSS = QString{
	"QTextEdit{border:none; background:#FFFFFF;}"
};

QString qChatListWidgetQSS = QString{
	"QListWidget{border:none; background:#F5F5F5;}"
	"QListWidget::item:hover{background:transparent;}"
};

QString qSendMsgBtnQSS = QString{
	"QPushButton{font-size:14px;color:#696969;background:#DCDCDC;border:1px solid #C0C0C0;border-radius:2px;}"
	"QPushButton:hover{background:#808080;color:#FFFFFF;}"
};

QString qChatListWdtVerScrollBarQSS = QString{
	"QScrollBar:vertical{width:10px;background:transparent;}"
	"QScrollBar::add-page,QScrollBar::sub-page{background:none;}"
	"QScrollBar::add-line,QScrollBar::sub-line{background:none;border:none;}"
	"QScrollBar::handle:vertical{background:#DCDCDC;border-radius:5px;}"
	"QScrollBar::handle:vertical:hover{background:#808080;}"
};

QString qAddFriendBtnQSS = QString{
	"QPushButton{border-radius:5px;background:#DBD9D8;border:none;"
	"background-image:url(./resources/image/add.png);background-repeat:no-repeat;background-position:center center;}"
	"QPushButton:hover{background:#A1A1A1;background-image:url(./resources/image/add.png);"
	"background-repeat:no-repeat;background-position:center center;}"
};