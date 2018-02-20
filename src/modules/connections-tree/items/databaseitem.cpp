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
    : AbstractNamespaceItem(model, parent, operations, index),
      m_keysCount(keysCount)
{           

    m_eventHandlers.insert("click", [this]() {
        if (m_childItems.size() != 0)
            return;

         loadKeys();         
    });

    m_eventHandlers.insert("add_key", [this]() {
        m_operations->openNewKeyDialog(m_dbIndex, [this]()
        {
            confirmAction(nullptr,
                          tr("Key was added. Do you want to reload keys in selected database?"),
                          [this]() { reload(); m_keysCount++; }, tr("Key was added"));
        });
    });

    m_eventHandlers.insert("reload", [this]() {
        if (isLocked()) {
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
             m_operations->flushDb(m_dbIndex, [this](const QString&)
             {
                 unload();                 
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
    if (m_operations) m_operations->notifyDbWasUnloaded(m_dbIndex);
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
    QString filter =  m_filter.isEmpty()? "" : QString("[filter: %1]").arg(m_filter.pattern());
    QString liveUpdate = m_liveUpdateTimer.isActive()? "[live update]" : "";

    return QString("db%1 %2 (%3) %4")
            .arg(m_dbIndex)
            .arg(filter)
            .arg(m_keysCount)
            .arg(liveUpdate);
}

bool DatabaseItem::isEnabled() const {return true;}

void DatabaseItem::notifyModel()
{
    unlock();
    AbstractNamespaceItem::notifyModel();
}

void DatabaseItem::loadKeys(std::function<void ()> callback)
{
    lock();
    emit m_model.itemChanged(getSelf());

    QString filter = (m_filter.isEmpty())? "" : m_filter.pattern();

    auto self = getSelf().toStrongRef();

    if (!self) {
        unlock();
        return;
    }

    m_operations->getDatabases([this](QMap<int, int> dbMapping){
        if (dbMapping.contains(m_dbIndex)) {
            m_keysCount = dbMapping[m_dbIndex];
            emit m_model.itemChanged(getSelf());
        }
    });

    m_operations->loadNamespaceItems(qSharedPointerDynamicCast<AbstractNamespaceItem>(self),
                                     filter, [this, callback](const QString& err) {       
        unlock();
        if (!err.isEmpty())
            return showLoadingError(err);

        if (callback) {
            callback();
        }
    });
}

QVariantMap DatabaseItem::metadata() const
{
    QVariantMap metadata = TreeItem::metadata();
    metadata["filter"] = m_filter.pattern();
    metadata["live_update"] = m_liveUpdateTimer.isActive();
    return metadata;
}

void DatabaseItem::setMetadata(const QString &key, QVariant value)
{
    bool isResetValue = (value.isNull() || !value.canConvert<QString>() || value.toString().isEmpty());

    if (key == "filter") {
        if (!m_filter.isEmpty() && isResetValue)
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

void DatabaseItem::unload()
{
    if (m_childItems.size() == 0)
        return;

    lock();
    clear();

    m_keysCount = 0;

    m_operations->notifyDbWasUnloaded(m_dbIndex);    

    unlock();    
}

void DatabaseItem::reload()
{
    unload();
    loadKeys();
}

void DatabaseItem::liveUpdate()
{
    if (isLocked()) {
        qDebug() << "Another loading operation is in progress. Skip this live update...";
        m_liveUpdateTimer.start();
        return;
    }

    loadKeys([this]() {
        QSettings settings;
        if (m_childItems.size() >= settings.value("app/liveUpdateKeysLimit", 1000).toInt()) {
            m_liveUpdateTimer.stop();
            emit m_model.itemChanged(getSelf());
            QMessageBox::warning(nullptr, tr("Live update was disabled"),
                                 tr("Live update was disabled due to exceeded keys limit. "
                                    "Please specify filter more carrfully or change limit in settings."));
        } else {
            m_liveUpdateTimer.start();
        }
    });
}

void DatabaseItem::filterKeys(const QRegExp &filter)
{
    m_filter = filter;
    emit m_model.itemChanged(getSelf());
    reload();
}

void DatabaseItem::resetFilter()
{
    m_filter = QRegExp();
    emit m_model.itemChanged(getSelf());
    reload();
}
