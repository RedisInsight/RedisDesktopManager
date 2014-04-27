#ifndef ABSTRACTTRANSPORTER_H
#define ABSTRACTTRANSPORTER_H

#include <QObject>

class AbstractTransporter : public QObject
{
    Q_OBJECT
public:
    explicit AbstractTransporter(QObject *parent = 0);

    void init();

signals:

public slots:

};

#endif // ABSTRACTTRANSPORTER_H
