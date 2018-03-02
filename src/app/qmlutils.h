#pragma once
#include <QObject>
#include <QVariant>
#include <QVariantList>
#include <QUrl>
#include <QtCharts/QXYSeries>

class QmlUtils : public QObject
{
    Q_OBJECT
public:    
    Q_INVOKABLE bool isBinaryString(const QVariant &value);
    Q_INVOKABLE long binaryStringLength(const QVariant &value);
    Q_INVOKABLE QString humanSize(long size);
    Q_INVOKABLE QVariant valueToBinary(const QVariant &value);
    Q_INVOKABLE QVariant binaryListToValue(const QVariantList& binaryList);
    Q_INVOKABLE QVariant printable(const QVariant &value, bool htmlEscaped=false);
    Q_INVOKABLE QVariant printableToValue(const QVariant &printable);
    Q_INVOKABLE QVariant toUtf(const QVariant &value);
    Q_INVOKABLE QString getPathFromUrl(const QUrl &url);
    Q_INVOKABLE void copyToClipboard(const QString &text);
    Q_INVOKABLE void addNewValueToDynamicChart(QtCharts::QXYSeries* series, double value);
    Q_INVOKABLE QObject* wrapLargeText(const QByteArray &text);
    Q_INVOKABLE void deleteTextWrapper(QObject* w);
    Q_INVOKABLE QString escapeHtmlEntities(const QString& t);
};
