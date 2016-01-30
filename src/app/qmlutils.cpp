#include "qmlutils.h"
#include <QClipboard>
#include <QApplication>
#include <QDebug>
#include <qredisclient/utils/text.h>

bool QmlUtils::isBinaryString(const QVariant &value)
{
    if (!value.canConvert(QVariant::ByteArray)) {
        return false;
    }
    QByteArray val = value.toByteArray();
    return isBinary(val);
}

QVariant QmlUtils::valueToBinary(const QVariant &value)
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

QVariant QmlUtils::binaryListToValue(const QVariantList &binaryList)
{
    QByteArray value;
    foreach (QVariant v, binaryList) {
        value.append((unsigned char)v.toInt());
    }
    return value;
}

QVariant QmlUtils::printable(const QVariant &value)
{
    if (!value.canConvert(QVariant::ByteArray)) {
        return QVariant();
    }
    QByteArray val = value.toByteArray();
    return printableString(val);
}

QVariant QmlUtils::printableToValue(const QVariant &printable)
{
    if (!printable.canConvert(QVariant::String)) {
        return QVariant();
    }
    QString val = printable.toString();
    return printableStringToBinary(val);
}

QVariant QmlUtils::toUtf(const QVariant &value)
{
    if (!value.canConvert(QVariant::ByteArray)) {
        return QVariant();
    }
    QByteArray val = value.toByteArray();
    QString result = QString::fromUtf8(val.constData(), val.size());
    return QVariant(result);
}

QString QmlUtils::getPathFromUrl(const QUrl &url)
{
    return url.path();
}

void QmlUtils::copyToClipboard(const QString &text)
{
    QClipboard* cb = QApplication::clipboard();

    if (!cb)
        return;

    cb->clear();
    cb->setText(text);
}
