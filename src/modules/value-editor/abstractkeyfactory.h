#pragma once 

#include <QString>
#include <functional>
#include "keymodel.h"

namespace ValueEditor {

class AbstractKeyFactory
{
public:
    virtual void loadKey(QString keyFullPath, int dbIndex,
                         std::function<void(QSharedPointer<Model>)> callback) = 0;

};

}
