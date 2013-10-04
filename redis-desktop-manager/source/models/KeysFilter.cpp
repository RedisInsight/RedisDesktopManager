#include "KeysFilter.h"
#include "RedisConnectionsModel.h"
#include "RedisKeyItem.h"
#include "RedisKeyNamespace.h"

KeysFilter::KeysFilter(QObject *parent)
	: QSortFilterProxyModel(parent), cacheBuilded(false)
{
	cache = new QHash<int, int>();
}

KeysFilter::~KeysFilter()
{
	delete cache;
}

void KeysFilter::filter(const QRegExp &pattern)
{
	cache->clear();
	cacheBuilded = false;

	setFilterRegExp(pattern);

	cacheBuilded = true;

	setFilterRegExp(pattern);

	hideEmptyNamespaces();
}

void KeysFilter::updateFilter()
{
	filter(filterRegExp());
}

void KeysFilter::hideEmptyNamespaces()
{	
	RedisConnectionsModel * model = (RedisConnectionsModel *)sourceModel();

	int rowsCount = model->rowCount();

	for (int i = 0; i < rowsCount; i++) 
	{
		QStandardItem * item = model->item(i);

		hideEmptyNamespacesWalker(item);
	}
}

void KeysFilter::hideEmptyNamespacesWalker(QStandardItem * parent)
{	
	if (parent == nullptr || parent->type() == RedisKeyItem::TYPE)
	{
		return;
	}

	int rowsCount = parent->rowCount();

	for (int i = 0; i < rowsCount; i++) 
	{
		QStandardItem * child = parent->child(i);
		
		hideEmptyNamespacesWalker(child);
	}

	if (parent->rowCount() == 0 
		&& parent->type() == RedisKeyNamespace::TYPE) {
		sourceModel()->removeRow(parent->row());
		return;
	}

	return;
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

		bool result = key->getFullName().contains(filterRegExp());

		if (!cacheBuilded && !result) {
			if (cache->contains(sourceParent.row())) {
				int count = 1 + cache->value(sourceParent.row());
				cache->insert(sourceParent.row(), count);
			} else {
				cache->insert(sourceParent.row(), 1);
			}		
		}

		return result;
	} else if (currItem != nullptr 
		&& currItem->type() == RedisKeyNamespace::TYPE
		&& cacheBuilded) {

			if (cache->contains(sourceRow) && cache->value(sourceRow) == currItem->rowCount()) {
				return false;
			}

			return true;

	} else {
		return true;
	}

}