#pragma once

#include "modules/value-editor/abstractkeyfactory.h"

class KeyFactory : public ValueEditor::AbstractKeyFactory
{
public:
    virtual void loadKey(QString keyFullPath, std::function<void(QSharedPointer<Model>)> callback) override;
};


