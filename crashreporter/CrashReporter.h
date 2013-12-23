/* 
 * RedisDesktopManager Crash Reporter
 * based on source from http://tomahawk-player.org
 */

#pragma once

#include <QDialog>
#include <QFile>
#include <QNetworkReply>
#include <QNetworkRequest>

#include "ui_CrashReporter.h"

class CrashReporter : public QDialog
{
    Q_OBJECT

public:
    CrashReporter( const QUrl& url, const QStringList& argv );
    virtual ~CrashReporter( );

private:
    Ui::CrashReporter ui;

    QString m_minidump;
    QNetworkRequest* m_request;
    QNetworkReply* m_reply;
    QUrl m_url;

    QString getPlatformInformation();

public slots:
    void send();

private slots:
    void onDone();
    void onProgress( qint64 done, qint64 total );
    void onFail( int error, const QString& errorString );
};
