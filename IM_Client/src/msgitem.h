#pragma once
#include <QWidget>
#include <QLabel>

class MsgItem : public QWidget
{
	Q_OBJECT
public:
	explicit MsgItem(QWidget* parent);
	void initUi();
private:
	QLabel*        m_pImage;
	QLabel*        m_pFirstStr;
	QLabel*        m_pSecondStr;
};