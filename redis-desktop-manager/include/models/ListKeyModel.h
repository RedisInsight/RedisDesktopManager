#ifndef LISTKEYMODEL_H
#define LISTKEYMODEL_H

#include "PaginatedModel.h"

class ListKeyModel : public PaginatedModel
{
public:
	ListKeyModel(QStringList& values);

	void setCurrentPage(int page);
};

#endif // LISTKEYMODEL_H
