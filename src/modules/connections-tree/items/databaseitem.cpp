#include "databaseitem.h"

#include <typeinfo>
#include <functional>
#include <algorithm>
#include <QDebug>
#include <QMenu>
#include <QInputDialog>
#include <QMessageBox>
#include <QFutureWatcher>

#include "namespaceitem.h"
#include "keyitem.h"
#include "serveritem.h"
#include "connections-tree/utils.h"
#include "connections-tree/model.h"

using namespace ConnectionsTree;

DatabaseItem::DatabaseItem(unsigned int index, int keysCount,
                           QSharedPointer<Operations> operations,
                           QWeakPointer<TreeItem> parent,
                           Model& model)
    : AbstractNamespaceItem(model, parent, operations),
      m_index(index),
      m_keysCount(keysCount),      
      m_locked(false)
{    
    m_renderingSettings.nsSeparator = operations->getNamespaceSeparator();
    m_renderingSettings.dbIndex = index;

    m_eventHandlers.insert("click", [this]() {
        if (m_childItems.size() != 0)
            return;

        loadKeys();
    });

    m_eventHandlers.insert("add_key", [this]() {
        m_operations->openNewKeyDialog(m_index, [this]()
        {
            confirmAction(nullptr,
                          tr("Key was added. Do you want to reload keys in selected database?"),
                          [this]() { reload(); m_keysCount++; }, tr("Key was added"));
        });
    });

    m_eventHandlers.insert("reload", [this]() {
        if (m_locked) {
            QMessageBox::warning(nullptr, tr("Another operation is currently in progress"),
                                 tr("Please wait until another operation will be finised."));
            return;
        }

        reload();
    });

    m_eventHandlers.insert("flush", [this]() {
    confirmAction(nullptr,
                      tr("Do you really want to remove all keys from this database?"),
                      [this]()
        {
             m_operations->flushDb(m_index, [this](const QString&)
             {
                 unload();
                 m_keysCount = 0;
             });
         });
    });

    QSettings settings;
    m_liveUpdateTimer.setInterval(settings.value("app/liveUpdateInterval", 10).toInt() * 1000);
    m_liveUpdateTimer.setSingleShot(true);
    connect(&m_liveUpdateTimer, &QTimer::timeout, this, [this]() {
        liveUpdate();
    });
}

DatabaseItem::~DatabaseItem()
{
    if (m_operations) m_operations->notifyDbWasUnloaded(m_index);
}

QByteArray DatabaseItem::getName() const
{
    return QByteArray();
}

QByteArray DatabaseItem::getFullPath() const
{
    return QByteArray();
}

QString DatabaseItem::getDisplayName() const
{
    if (m_childItems.isEmpty()) {
        return QString("db%1 (%2)").arg(m_index).arg(m_keysCount);
    } else {                
        QString filter =  m_renderingSettings.filter.isEmpty()? "" : QString("[filter: %1]").arg(m_renderingSettings.filter.pattern());
        QString liveUpdate = m_liveUpdateTimer.isActive()? "[live update]" : "";

        return QString("db%1 %2 (%3/%4) %5")
                .arg(m_index)
                .arg(filter)
                .arg(m_rawChilds.size())
                .arg(m_keysCount)
                .arg(liveUpdate);
    }
}

QString DatabaseItem::getIconUrl() const
{
    if (m_locked) return QString("qrc:/images/wait.svg");
    return QString("qrc:/images/db.svg");
}


bool DatabaseItem::isLocked() const {return m_locked;}

bool DatabaseItem::isEnabled() const {return true;}

void DatabaseItem::notifyModel()
{
    m_locked = false;
    AbstractNamespaceItem::notifyModel();
}

void DatabaseItem::loadKeys()
{
    if (m_rawChilds.size() > 0) {
        clear(false);
        renderChilds();
        return;
    }

    m_locked = true;
    emit m_model.itemChanged(getSelf());

    QString filter = (m_renderingSettings.filter.isEmpty())? "" : m_renderingSettings.filter.pattern();

    m_operations->getDatabaseKeys(m_index, filter, [this](const RedisClient::Connection::RawKeysList& rawKeys, const QString& err) {
        if (!err.isEmpty()) {
            m_locked = false;

            emit m_model.itemChanged(getSelf());
            emit m_model.error(err);

            QMessageBox::warning(nullptr, tr("Keys error"), err);

            return;
        }
        m_rawChilds = rawKeys;
        renderChilds();
    });
}

int DatabaseItem::getIndex() const
{
    return m_index;
}

QVariant DatabaseItem::metadata(const QString &key)
{
    if (key == "filter")
        return m_renderingSettings.filter.pattern();
    if (key == "live_update")
        return m_liveUpdateTimer.isActive();

    return QVariant();
}

void DatabaseItem::setMetadata(const QString &key, QVariant value)
{
    bool isResetValue = (value.isNull() || !value.canConvert<QString>() || value.toString().isEmpty());

    if (key == "filter") {
        if (!m_renderingSettings.filter.isEmpty() && isResetValue)
            return resetFilter();
        else if (isResetValue)
            return;

        QRegExp pattern(value.toString(), Qt::CaseSensitive, QRegExp::PatternSyntax::WildcardUnix);
        return filterKeys(pattern);
    } else if (key == "live_update") {
        if (m_liveUpdateTimer.isActive() && isResetValue) {
            qDebug() << "Stop live update";
            m_liveUpdateTimer.stop();            
        } else {
            qDebug() << "Start live update";
            m_liveUpdateTimer.start();
        }

        emit m_model.itemChanged(getSelf());
    }
}

void DatabaseItem::unload(bool removeRawKeys)
{
    if (m_childItems.size() == 0)
        return;

    m_locked = true;
    clear(removeRawKeys);

    m_operations->notifyDbWasUnloaded(m_index);

    m_locked = false;
    emit m_model.itemChanged(getSelf());
}

void DatabaseItem::reload()
{
    unload();
    loadKeys();
}

void DatabaseItem::liveUpdate()
{
    if (m_locked) {
        qDebug() << "Another loading operation is in progress. Skip this live update...";
        m_liveUpdateTimer.start();
        return;
    }

    m_locked = true;
    emit m_model.itemChanged(getSelf());

    QString filter = (m_renderingSettings.filter.isEmpty())? "" : m_renderingSettings.filter.pattern();

    m_operations->getDatabaseKeys(m_index, filter, [this](const RedisClient::Connection::RawKeysList& rawKeys, const QString& err) {
        if (!err.isEmpty()) {
            m_locked = false;

            emit m_model.itemChanged(getSelf());
            emit m_model.error(err);

            QMessageBox::warning(nullptr, tr("Keys error"), err);

            return;
        }

        m_rawChilds = rawKeys;

        QSettings settings;
        if (m_rawChilds.size() >= settings.value("app/liveUpdateKeysLimit", 1000).toInt()) {
            m_liveUpdateTimer.stop();
            emit m_model.itemChanged(getSelf());
            QMessageBox::warning(nullptr, tr("Live update was disabled"),
                                 tr("Live update was disabled due to exceeded keys limit. "
                                    "Please specify filter more carrfully or change limit in settings."));
        } else {
            clear(false);
            renderChilds();
            m_liveUpdateTimer.start();
        }
    });
}

void DatabaseItem::filterKeys(const QRegExp &filter)
{
    m_renderingSettings.filter = filter;    
    loadKeys();
}

void DatabaseItem::resetFilter()
{
    m_renderingSettings.filter = QRegExp();    
    loadKeys();
}
