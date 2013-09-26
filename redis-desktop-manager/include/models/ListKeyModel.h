#ifndef LISTKEYMODEL_H
#define LISTKEYMODEL_H

#include <QStandardItemModel>

class ListKeyModel : public QStandardItemModel
{
public:
	ListKeyModel(QStringList values);
};

#endif // LISTKEYMODEL_H
