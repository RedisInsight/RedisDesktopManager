#ifndef HASHKEYMODEL_H
#define HASHKEYMODEL_H

#include <QStandardItemModel>

class HashKeyModel : public QStandardItemModel
{
public:
	HashKeyModel(QStringList values);
};

#endif // HASHKEYMODEL_H
