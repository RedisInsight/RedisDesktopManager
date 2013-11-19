#pragma once

#include <QObject>
#include <QStandardItem>

#include "RedisServerDbItem.h"

class KeysTreeRenderer : public QObject
{
	Q_OBJECT

public:
	 static QList<QStandardItem*> KeysTreeRenderer::renderKeys(RedisServerDbItem *, QStringList, const QRegExp &, const RedisServerDbItem::Icons &);

	 static void KeysTreeRenderer::renderNamaspacedKey(QList<QStandardItem*> &, QStandardItem *, 
				QString, QString, const QIcon &, const QIcon &, RedisServerDbItem *);
};

