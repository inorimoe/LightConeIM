#pragma once
#include "common.h"
#include "QDir"

QString getImagePath()
{
	QString str = QDir::currentPath();

	return str + "/resources/image/";
}

const char* CRLF = "\r\n";