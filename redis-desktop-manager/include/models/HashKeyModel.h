#ifndef HASHKEYMODEL_H
#define HASHKEYMODEL_H

#include <QStandardItemModel>
#include "PaginatedModel.h"

class HashKeyModel : public QStandardItemModel, public PaginatedModel
{
public:
	HashKeyModel(QStringList& values);

	void setCurrentPage(int);

};

#endif // HASHKEYMODEL_H
