/* 
 * RedisDesktopManager Crash Reporter
 * based on source from http://tomahawk-player.org
 */

#include "CrashReporter.h"
#include "version.h"
#include <QIcon>
#include <QDebug>
#include <QTimer>
#include <QDir>
#include <QDateTime>
#include <QMessageBox>

CrashReporter::CrashReporter( const QUrl& url, const QStringList& args )
    : m_reply( 0 )
    , m_url( url )
{
    ui.setupUi( this );    
    ui.progressBar->setRange( 0, 100 );
    ui.progressBar->setValue( 0 );    

    ui.vboxLayout->setSpacing( 16 );
    ui.hboxLayout1->setSpacing( 16 );
    ui.progressBar->setTextVisible( false );
    ui.progressLabel->setIndent( 1 );

    m_request = new QNetworkRequest( m_url );

    m_minidump = args.value( 1 ) + ".dmp";

    setFixedSize( size() );

    QTimer::singleShot( 1, this, SLOT( send() ) );
}

CrashReporter::~CrashReporter()
{
    delete m_request;
    delete m_reply;
}

static QByteArray
contents( const QString& path )
{
    QFile f( path );
    f.open( QFile::ReadOnly );
    return f.readAll();
}


void
CrashReporter::send()
{
    QByteArray body;

    // add parameters
    typedef QPair<QByteArray, QByteArray> Pair;
    QList<Pair> pairs;

    pairs << Pair( "product", "RDM" )
          << Pair( "platform",  getPlatformInformation().toUtf8() )
          << Pair( "version",  RDM_VERSION );

    foreach ( Pair const pair, pairs )
    {
        body += "--rboundary\r\n";
        body += "Content-Disposition: form-data; name=\"" +
                           pair.first  + "\"\r\n\r\n" +
                           pair.second + "\r\n";
    }

    // add minidump file
    body += "--rboundary\r\n";
    body += "Content-Disposition: form-data; name=\"upload_file_minidump\"; filename=\""
          + QFileInfo( m_minidump ).fileName() + "\"\r\n";
    body += "Content-Type: application/octet-stream\r\n";
    body += "\r\n";
    body += contents( m_minidump );
    body += "\r\n";
    body += "--rboundary\r\n";

    QNetworkAccessManager* nam = new QNetworkAccessManager( this );
    m_request->setHeader( QNetworkRequest::ContentTypeHeader, "multipart/form-data; boundary=rboundary" );
    m_reply = nam->post( *m_request, body );

    connect( m_reply, &QNetworkReply::finished, this, &CrashReporter::onDone, Qt::QueuedConnection );
    connect( m_reply, &QNetworkReply::uploadProgress, this, &CrashReporter::onProgress );
}

QString 
CrashReporter::getPlatformInformation()
{
    //todo: collect more info

    QString platform;

#ifdef Q_OS_LINUX
    platform = "Linux";
#endif

#ifdef Q_OS_MAC    
    platform = "MacOS";
#endif

#ifdef Q_OS_WIN    
    platform = "Windows";
#endif

    return platform;
}


void
CrashReporter::onProgress( qint64 done, qint64 total )
{
    if ( total )
    {
        QString const msg = tr( "Uploaded %L1 of %L2 KB." ).arg( done / 1024 ).arg( total / 1024 );

        ui.progressBar->setMaximum( total );
        ui.progressBar->setValue( done );
        ui.progressLabel->setText( msg );
    }
}


void
CrashReporter::onDone()
{
    QByteArray data = m_reply->readAll();
    ui.progressBar->setValue( ui.progressBar->maximum() );
    ui.button->setText( tr( "Close" ) );

    QString const response = QString::fromUtf8( data );

    if ( ( m_reply->error() != QNetworkReply::NoError ) || !response.startsWith( "http" ) )
    {
        QMessageBox::warning(this, "Server Error", response);
        onFail( m_reply->error(), m_reply->errorString() );
    }
    else {
        QMessageBox::warning(
            this, "Issue Created!", 
            QString(
                "Issue created, please add more info about crash in comments "
                "<br /><a href='%1'>%1</a>").arg(response)
            );
        ui.progressLabel->setText( tr( "Sent! <b>Many thanks</b>." ) );
        QFile::remove(m_minidump);
    }
}


void
CrashReporter::onFail( int error, const QString& errorString )
{
    ui.button->setText( tr( "Close" ) );
    ui.progressLabel->setText( tr( "Failed to send crash info." ) );
    qDebug() << "Error:" << error << errorString;
}
