#pragma once

#include "basetestcase.h"
#include "models/key-models/keyfactory.h"

class TestKeyModels : public BaseTestCase
{
    Q_OBJECT        
private slots:
    void testKeyFactory();
    void testKeyFactory_data();

    void testKeyFactoryAddKey();
    void testKeyFactoryAddKey_data();

    void testValueLoading();
    void testValueLoading_data();

    void testKeyModelModifyRows();
    void testKeyModelModifyRows_data();

private:
    QSharedPointer<ValueEditor::Model> getKeyModel(QSharedPointer<RedisClient::Connection> connection);
};

