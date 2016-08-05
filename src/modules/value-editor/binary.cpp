#include "binary.h"
#include <QClipboard>
#include <QApplication>
#include <qredisclient/utils/text.h>

bool BinaryUtils::isBinaryString(const QVariant &value)
{
    if (!value.canConvert(QVariant::ByteArray)) {
        return false;
    }
    QByteArray val = value.toByteArray();
    return isBinary(val);
}

long BinaryUtils::binaryStringLength(const QVariant &value)
{
    if (!value.canConvert(QVariant::ByteArray)) {
        return -1;
    }
    QByteArray val = value.toByteArray();
    return val.size();
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

QVariant BinaryUtils::printableToValue(const QVariant &printable)
{
    if (!printable.canConvert(QVariant::String)) {
        return QVariant();
    }
    QString val = printable.toString();
    return printableStringToBinary(val);
}

QVariant BinaryUtils::toUtf(const QVariant &value)
{
    if (!value.canConvert(QVariant::ByteArray)) {
        return QVariant();
    }
    QByteArray val = value.toByteArray();
    QString result = QString::fromUtf8(val.constData(), val.size());
    return QVariant(result);
}

QString BinaryUtils::getPathFromUrl(const QUrl &url)
{
#ifdef Q_OS_WIN
    return url.path().mid(1);
#else
    return url.path();
#endif
}

void BinaryUtils::copyToClipboard(const QString &text)
{
    QClipboard* cb = QApplication::clipboard();

    if (!cb)
        return;

    cb->clear();
    cb->setText(text);
}
