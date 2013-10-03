#include "KeysFilter.h"
#include "RedisConnectionsModel.h"
#include "RedisKeyItem.h"
#include "RedisKeyNamespace.h"

KeysFilter::KeysFilter(QObject *parent)
	: QSortFilterProxyModel(parent)
{

}

KeysFilter::~KeysFilter()
{

}


void KeysFilter::filter(QRegExp &pattern)
{
	setFilterRegExp(pattern);

	//todo: hide empty namespaces
}

void KeysFilter::hideEmptyNamespaces(QModelIndex& parent = QModelIndex())
{
	RedisConnectionsModel * model = (RedisConnectionsModel *)sourceModel();


	model->rowCount(parent);

	//todo: complete this method

}

bool KeysFilter::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
	RedisConnectionsModel * model = (RedisConnectionsModel *)sourceModel();

	QStandardItem * parent = model->itemFromIndex(sourceParent);

	if (parent == nullptr) {
		return true;
	}

	QStandardItem * currItem = parent->child(sourceRow);

	if (currItem != nullptr && currItem->type() == RedisKeyItem::TYPE) {

		RedisKeyItem * key = (RedisKeyItem *) currItem;

		return key->getFullName().contains(filterRegExp());
	} else {
		return true;
	}

}