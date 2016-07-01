#pragma once
#include <QObject>
#include <QVariant>
#include <QVariantList>

class BinaryUtils : public QObject
{
    Q_OBJECT
public:
    // binary utils
    Q_INVOKABLE bool isBinaryString(const QVariant &value);
    Q_INVOKABLE long binaryStringLength(const QVariant &value);
    Q_INVOKABLE QVariant valueToBinary(const QVariant &value);
    Q_INVOKABLE QVariant binaryListToValue(const QVariantList& binaryList);
    Q_INVOKABLE QVariant printable(const QVariant &value);
    Q_INVOKABLE QVariant printableToValue(const QVariant &printable);
    Q_INVOKABLE QVariant toUtf(const QVariant &value);
};
