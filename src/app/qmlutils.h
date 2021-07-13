#pragma once
#include <QObject>
#include <QVariant>
#include <QVariantList>
#include <QUrl>
#include <QtCharts/QXYSeries>
#include <QKeySequence>

class QmlUtils : public QObject
{
    Q_OBJECT
public:
    Q_INVOKABLE bool isBinaryString(const QVariant &value);
    Q_INVOKABLE long binaryStringLength(const QVariant &value);    
    Q_INVOKABLE QVariant b64toByteArray(const QVariant &value);
    Q_INVOKABLE QByteArray minifyJSON(const QVariant &value);
    Q_INVOKABLE QByteArray prettyPrintJSON(const QVariant &value);
    Q_INVOKABLE bool isJSON(const QVariant &value);

    Q_INVOKABLE unsigned isCompressed(const QVariant &value);
    Q_INVOKABLE QVariant decompress(const QVariant &value);
    Q_INVOKABLE QVariant compress(const QVariant &value, unsigned alg);
    Q_INVOKABLE QString compressionAlgName(unsigned alg);

    Q_INVOKABLE QString humanSize(long size);
    Q_INVOKABLE QVariant valueToBinary(const QVariant &value);
    Q_INVOKABLE QVariant binaryListToValue(const QVariantList& binaryList);
    Q_INVOKABLE QVariant printable(const QVariant &value, bool htmlEscaped=false, int maxLength=-1);
    Q_INVOKABLE QVariant printableToValue(const QVariant &printable);
    Q_INVOKABLE QVariant toUtf(const QVariant &value);
    Q_INVOKABLE QString getNativePath(const QString &path);
    Q_INVOKABLE QString getPathFromUrl(const QUrl &url);
    Q_INVOKABLE QString getUrlFromPath(const QString &path);
    Q_INVOKABLE QString getDir(const QString &path);
    Q_INVOKABLE bool fileExists(const QString& path);

    Q_INVOKABLE void copyToClipboard(const QString &text);
    Q_INVOKABLE bool saveToFile(const QVariant &value, const QString &path);
    Q_INVOKABLE void addNewValueToDynamicChart(QtCharts::QXYSeries* series, qreal value);
    Q_INVOKABLE QObject* wrapLargeText(const QByteArray &text);
    Q_INVOKABLE void deleteTextWrapper(QObject* w);
    Q_INVOKABLE QString escapeHtmlEntities(const QString& t);
    Q_INVOKABLE QString standardKeyToString(QKeySequence::StandardKey key);
    Q_INVOKABLE double getScreenScaleFactor();    
    Q_INVOKABLE bool isAppStoreBuild();
};
