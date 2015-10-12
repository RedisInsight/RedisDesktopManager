#include "binary.h"
#include <qredisclient/utils/text.h>

bool BinaryUtils::isBinaryString(const QVariant &value)
{
    if (!value.canConvert(QVariant::ByteArray)) {
        return false;
    }
    QByteArray val = value.toByteArray();
    return isBinary(val);
}

QVariant BinaryUtils::valueToBinary(const QVariant &value)
{
    if (!value.canConvert(QVariant::ByteArray)) {
        return QVariant();
    }

    QByteArray val = value.toByteArray();
    QVariantList list;

    for(int index=0; index < val.length(); ++index) {
        list.append(QVariant((unsigned char)val.at(index)));
    }
    return QVariant(list);
}

QVariant BinaryUtils::binaryListToValue(const QVariantList &binaryList)
{
    QByteArray value;
    foreach (QVariant v, binaryList) {
        value.append((unsigned char)v.toInt());
    }
    return value;
}

QVariant BinaryUtils::printable(const QVariant &value)
{
    if (!value.canConvert(QVariant::ByteArray)) {
        return QVariant();
    }
    QByteArray val = value.toByteArray();
    return printableString(val);
}
