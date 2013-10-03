#ifndef KEYSFILTER_H
#define KEYSFILTER_H

#include <QSortFilterProxyModel>

class KeysFilter : public QSortFilterProxyModel
{
	Q_OBJECT

public:
	KeysFilter(QObject *parent);
	~KeysFilter();

	void filter(QRegExp&);

protected:
	bool filterAcceptsRow(int, const QModelIndex &) const;
	void hideEmptyNamespaces(QModelIndex&);
private:
	
};

#endif // KEYSFILTER_H
