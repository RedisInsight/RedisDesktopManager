#pragma once

#include <QObject>
#include <QStandardItem>

#include "RedisServerDbItem.h"

class KeysTreeRenderer : public QObject
{
    Q_OBJECT

public:
     static QList<QStandardItem*> renderKeys(RedisServerDbItem *, QStringList, const QRegExp &, const RedisServerDbItem::Icons &);

     static void renderNamaspacedKey(QList<QStandardItem*> &, QStandardItem *,
                QString, QString, const QIcon &, const QIcon &, RedisServerDbItem *);
};

