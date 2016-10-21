#include "qmlutils.h"
#include <QClipboard>
#include <QApplication>
#include <QDebug>
#include <qredisclient/utils/text.h>
#include <QtCharts/QDateTimeAxis>
#include <QDateTime>

bool QmlUtils::isBinaryString(const QVariant &value)
{
    if (!value.canConvert(QVariant::ByteArray)) {
        return false;
    }
    QByteArray val = value.toByteArray();
    return isBinary(val);
}

long QmlUtils::binaryStringLength(const QVariant &value)
{
    if (!value.canConvert(QVariant::ByteArray)) {
        return -1;
    }
    QByteArray val = value.toByteArray();
    return val.size();
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
    return url.isLocalFile() ? url.toLocalFile() : url.path();
}

void QmlUtils::copyToClipboard(const QString &text)
{
    QClipboard* cb = QApplication::clipboard();

    if (!cb)
        return;

    cb->clear();
    cb->setText(text);
}

QtCharts::QDateTimeAxis* findDateTimeAxis(QtCharts::QXYSeries *series)
{
    using namespace QtCharts;

    QList<QAbstractAxis*> axes = series->attachedAxes();

    QDateTimeAxis* ax = nullptr;

    for (QAbstractAxis* axis : axes) {
        if (axis->type() == QAbstractAxis::AxisTypeDateTime) {
            ax = qobject_cast<QDateTimeAxis* >(axis);
            return ax;
        }
    }

    return ax;
}

void QmlUtils::addNewValueToDynamicChart(QtCharts::QXYSeries *series, double value)
{    
    using namespace QtCharts;

    QDateTimeAxis* ax = findDateTimeAxis(series);

    if (series->count() == 0 && ax) {
        ax->setMin(QDateTime::currentDateTime());
    }

    series->append(QDateTime::currentDateTime().toMSecsSinceEpoch(), value);

    if (series->attachedAxes().size() > 0 && ax) {
        ax->setMax(QDateTime::currentDateTime());
    }
}
