#pragma once

#include "basetestcase.h"
#include "models/key-models/keyfactory.h"

class TestKeyModels : public BaseTestCase
{
    Q_OBJECT        
private slots:
    void testKeyFactory();
    void testKeyFactory_data();

    void testValueLoading();
    void testValueLoading_data();
private:
    QSharedPointer<ValueEditor::Model> getKeyModel(QSharedPointer<RedisClient::Connection> connection);
};

