#ifndef UPDATER_H
#define UPDATER_H

#include <QtCore/QObject>
#include <QtNetwork/QNetworkAccessManager>

class Updater : public QObject
{
    Q_OBJECT

public:
    Updater();
    ~Updater();

signals:
    void updateUrlRetrived(QString &);

private:
	//  TBD: use app wide QNetworkAccessManager
    QNetworkAccessManager *manager;

private slots:
    void requestFinished(QNetworkReply*);

};

#endif // UPDATER_H
