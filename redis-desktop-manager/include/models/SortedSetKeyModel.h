#ifndef SORTEDSETKEYMODEL_H
#define SORTEDSETKEYMODEL_H

#include <QStandardItemModel>

class SortedSetKeyModel : public QStandardItemModel
{
public:
	SortedSetKeyModel(QStringList values);
};

#endif // SORTEDSETKEYMODEL_H
