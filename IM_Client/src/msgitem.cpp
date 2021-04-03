#include "msgitem.h"
#include "../lib/common.h"
#include "../lib/stylesheet.h"
#include <QHBoxLayout>
#include <QDebug>

MsgItem::MsgItem(QWidget* parent)
	: QWidget(parent)
	, m_pImage(nullptr)
	, m_pFirstStr(nullptr)
	, m_pSecondStr(nullptr)
{
	initUi();
}

void MsgItem::initUi()
{
	setStyleSheet(qItemBackgroundQSS
			.arg(itemBackgroundColor)
			.arg(itemBackgroundHoverColor));
	setFixedSize(itemWidth, itemHeight);
	QPixmap image;
	image.load(getImagePath() + "itemDefaultImage.png");
	
	m_pImage = new QLabel(this);
	m_pImage->setPixmap(image);
	m_pImage->setFixedSize(itemImageLenth, itemImageLenth);

	m_pFirstStr = new QLabel(this);
	m_pFirstStr->setStyleSheet(qItemStringQSS
							  .arg(12)
							  .arg(itemFirstColor));
	m_pFirstStr->setText("Test");
	m_pSecondStr = new QLabel(this);
	m_pSecondStr->setStyleSheet(qItemStringQSS
							  .arg(10)
		                      .arg(itemSecondColor));
	m_pSecondStr->setText("Test");

	QHBoxLayout* pMainLayout = new QHBoxLayout(this);
	pMainLayout->setContentsMargins(0, 0, 0, 0);
	pMainLayout->setSpacing(0);

	QVBoxLayout* pStringLayout = new QVBoxLayout(this);
	pStringLayout->setContentsMargins(0, 0, 0, 0);
	pStringLayout->setSpacing(0);

	pStringLayout->addWidget(m_pFirstStr);
	pStringLayout->addWidget(m_pSecondStr);

	pMainLayout->addWidget(m_pImage);
	pMainLayout->addLayout(pStringLayout);
}