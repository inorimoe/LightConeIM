#pragma once
#include "../lib/common.h"
#include "../lib/stylesheet.h"
#include "netsocket.h"

#include <QListView>
#include <QStyledItemDelegate>
#include <QStandardItemModel>
#include <QSortFilterProxyModel>
#include <QMetaType>
#include <QPixmap>
#include <QString>
#include <QVariant>

typedef struct {
	int     friendId;
	QPixmap image;
	QString firstStr;
	QString secondStr;
} MsgItemData;

Q_DECLARE_METATYPE(MsgItemData);

class MsgItemDelegate;
class MsgItemModel;
class MsgItemView : public QListView
{
	Q_OBJECT
public:
	MsgItemView(QWidget* parent, NetSocket* pNetSocket);
	~MsgItemView();
signals:
	void _setChatObjectData(MsgItemData var);
public slots:
	void onLoadData();
	void onOpenChat(const QModelIndex& index);
	void onAddItemData();
public:
	void addItem(const QVariant& value, int role = Qt::UserRole + 1);
private:
	NetSocket* m_pNetSocket;
	MsgItemModel* m_pSourceModel;
	QSortFilterProxyModel* m_pProxyModel;
	MsgItemDelegate* m_pDelegate;
};

class MsgItemDelegate : public QStyledItemDelegate
{
	Q_OBJECT
public:
	MsgItemDelegate(QObject* parent);
	~MsgItemDelegate();
protected:
	void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
	QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const override;

private:

};

class MsgItemModel : public QStandardItemModel
{
	Q_OBJECT
public:
	MsgItemModel(QObject* parent);
	~MsgItemModel();
};