#ifndef KEYSFILTER_H
#define KEYSFILTER_H

#include <QSortFilterProxyModel>
#include <QStandardItem>

class KeysFilter : public QSortFilterProxyModel
{
	Q_OBJECT

public:
	KeysFilter(QObject *parent);
	~KeysFilter();

	void filter(const QRegExp&);

	void updateFilter();

protected:
	bool filterAcceptsRow(int, const QModelIndex &) const;
	void hideEmptyNamespaces();
	void hideEmptyNamespacesWalker(QStandardItem * parent);
private:
	QHash<QStandardItem *, int> * cache;
	bool cacheBuilded;

	
};

#endif // KEYSFILTER_H
