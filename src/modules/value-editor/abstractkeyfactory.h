#pragma once 

#include <QString>
#include <functional>
#include "model.h"

namespace ValueEditor {

class AbstractKeyFactory
{
public:
    virtual void loadKey(QString keyFullPath, int dbIndex,
                         std::function<void(QSharedPointer<Model>)> callback) = 0;

};

}
