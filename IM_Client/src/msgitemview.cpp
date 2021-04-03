#include "msgitemview.h"
#include "parsedata.h"
#include <QPainter>
#include <QList>
#include <QScrollBar>

MsgItemView::MsgItemView(QWidget* parent, NetSocket* pNetSocket)
	: QListView(parent)
	, m_pSourceModel(nullptr)
	, m_pProxyModel(nullptr)
	, m_pDelegate(nullptr)
    , m_pNetSocket(pNetSocket)
{
    setStyleSheet(qMsgItemViewQSS.arg(itemBackgroundColor));
    setEditTriggers(QAbstractItemView::NoEditTriggers);
    setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    verticalScrollBar()->setStyleSheet(qMsgItemViewvVerScrollBarQSS);
    setFixedWidth(itemWidth);

	m_pSourceModel = new MsgItemModel(this);

	//m_pProxyModel = new QSortFilterProxyModel(this);
 //   m_pProxyModel->setSourceModel(m_pSourceModel);
 //   m_pProxyModel->setFilterRole(Qt::UserRole + 1);
 //   m_pProxyModel->setDynamicSortFilter(true);
    //过滤数据使用自定义类QSortFilterProxyModel并重定义虚函数QSortFilterProxyModel::filterAcceptsRow
    //bool MySortFilterProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex & sourceParent) const

	m_pDelegate = new MsgItemDelegate(this);

	setItemDelegate(m_pDelegate);
	//setModel(m_pProxyModel);
    setModel(m_pSourceModel);
	setDragEnabled(false);

    connect(m_pNetSocket, &NetSocket::_msgItemDataReady, this, &MsgItemView::onLoadData);
    connect(this, &MsgItemView::clicked, this, &MsgItemView::onOpenChat);
    connect(m_pNetSocket, &NetSocket::_addFriendDataReady, this, &MsgItemView::onAddItemData);
}

MsgItemView::~MsgItemView()
{

}
void MsgItemView::onLoadData()
{
    QList<MsgItemData> dataList;
    ParseData::parseHttpToMsgItem(m_pNetSocket, &dataList);
    for (int i = 0; i < dataList.size(); ++i) {
        dataList[i].image.load(getImagePath() + "itemDefaultImage.png");
        dataList[i].secondStr = QString("test");
        addItem(QVariant::fromValue(dataList[i]), Qt::UserRole + 1);
    }
}

void MsgItemView::onAddItemData()
{
    MsgItemData item;
    QString msgType;
    ParseData::parseNewFriendData(m_pNetSocket, &item, &msgType);
    if (msgType == "AddFriendSuccess") {
        item.image.load(getImagePath() + "itemDefaultImage.png");
        item.secondStr = QString("test");
        addItem(QVariant::fromValue(item), Qt::UserRole + 1);
    }
    else {
        qDebug() << msgType;
    }
}

void MsgItemView::onOpenChat(const QModelIndex& index)
{
    QStandardItem* item = m_pSourceModel->itemFromIndex(index);
    if (item)
    {
        MsgItemData itemData = item->data().value<MsgItemData>();
        m_pNetSocket->setChatObjectId(itemData.friendId);
        emit _setChatObjectData(itemData);
    }
}

void MsgItemView::addItem(const QVariant& value, int role)
{
    QStandardItem* pItem = new QStandardItem;
	pItem->setData(value, role);
	m_pSourceModel->appendRow(pItem);
}

MsgItemDelegate::MsgItemDelegate(QObject* parent)
{

}

MsgItemDelegate::~MsgItemDelegate()
{

}

void MsgItemDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    if (index.isValid()) {
        painter->save();
        QVariant var = index.data(Qt::UserRole + 1);
        MsgItemData itemData = var.value<MsgItemData>();

        // item 矩形区域
        QRectF rect = option.rect;

        // 鼠标悬停或者选中时改变背景色
        
        if (option.state.testFlag(QStyle::State_Enabled)) {
            painter->fillRect(rect, itemBackgroundColor);
        }
        if (option.state.testFlag(QStyle::State_MouseOver)) {
            painter->fillRect(rect, itemBackgroundHoverColor);
        }
        if (option.state.testFlag(QStyle::State_Selected)) {
            painter->fillRect(rect, itemBackgroundHoverColor);
        }

        // 位置区域
        QRectF imageRect = QRect(rect.left() + 5, rect.top() + 5, itemImageLenth, itemImageLenth);
        QRectF firstStrRect = QRect(imageRect.right() + 5, imageRect.top(), rect.width() - 10 - imageRect.width(), 20);
        QRectF secondStrRect = QRect(firstStrRect.left(), firstStrRect.bottom() + 5, rect.width() - 10 - firstStrRect.width(), 20);

        painter->drawPixmap(imageRect, itemData.image, itemData.image.rect());
        painter->setFont(QFont("Microsoft Yahei", 10));
        painter->setPen(QPen(Qt::black));
        painter->drawText(firstStrRect, itemData.firstStr);
        painter->setPen(QPen(Qt::gray));
        painter->drawText(secondStrRect, itemData.secondStr);
        painter->restore();
    }
}

QSize MsgItemDelegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    return QSize(itemWidth, itemHeight);
}

MsgItemModel::MsgItemModel(QObject* parent)
{

}

MsgItemModel::~MsgItemModel()
{

}