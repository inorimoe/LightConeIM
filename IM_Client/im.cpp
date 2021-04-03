#pragma once
#include "im.h"
#include "./src/login.h"
#include <QtWidgets/QApplication>
#include <QTranslator>
#include "./src/mainwidget.h"
#include "./src/netsocket.h"

IM::IM(int argc, char* argv[])
{
	QApplication a(argc, argv);
	QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

	QTranslator translator;
	translator.load("im_zh.qm");
	qApp->installTranslator(&translator);
	
	NetSocket netSocket;

	Login login(&netSocket);
	login.show();

	MainWidget mainWidget(&netSocket, &login);
	a.exec();
}

IM::~IM()
{
}
