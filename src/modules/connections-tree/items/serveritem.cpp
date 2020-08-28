#include "serveritem.h"

#include <asyncfuture.h>

#include <QAction>
#include <QDebug>
#include <QMenu>
#include <QMessageBox>
#include <algorithm>
#include <functional>

#include "connections-tree/model.h"
#include "connections-tree/utils.h"
#include "databaseitem.h"

using namespace ConnectionsTree;

ServerItem::ServerItem(QSharedPointer<Operations> operations, Model& model,
                       QWeakPointer<TreeItem> parent)
    : SortableTreeItem(model),
      m_operations(operations),
      m_parent(parent) {}

ServerItem::~ServerItem() {}

QString ServerItem::getDisplayName() const { return m_operations->connectionName(); }

QList<QSharedPointer<TreeItem> > ServerItem::getAllChilds() const {
  return m_databases;
}

uint ServerItem::childCount(bool) const {
  return static_cast<uint>(m_databases.size());
}

QSharedPointer<TreeItem> ServerItem::child(uint row) const {
  if (row < m_databases.size()) {
    return m_databases.at(row);
  }

  return QSharedPointer<TreeItem>();
}

QWeakPointer<TreeItem> ServerItem::parent() const {
    return m_parent;
}

void ServerItem::setParent(QWeakPointer<TreeItem> p)
{
    m_parent = p;
}

bool ServerItem::isDatabaseListLoaded() const {
  return isLocked() == false && m_databases.size() > 0;
}

QSharedPointer<Operations> ServerItem::getOperations() { return m_operations; }

int ServerItem::row() const
{
    if (!parent()) {
        return m_row;
    }

    return TreeItem::row();
}

void ServerItem::load() {
  lock();

  std::function<void(RedisClient::DatabaseList)> callback =
      [this](RedisClient::DatabaseList databases) {
        if (databases.size() == 0) {
          unlock();
          return;
        }

        RedisClient::DatabaseList::const_iterator db = databases.constBegin();
        while (db != databases.constEnd()) {
          QSharedPointer<TreeItem> database((new DatabaseItem(
              db.key(), db.value(), m_operations, m_self, m_model)));

          m_databases.push_back(database);
          ++db;
        }

        unlock();
        emit m_model.itemChildsLoaded(m_self);
      };

  m_currentOperation = m_operations->getDatabases(callback);

  if (m_currentOperation.isRunning()) {
    auto wPtr = m_self;

    AsyncFuture::observe(m_currentOperation)
        .subscribe(
            [this, wPtr]() {
              if (!wPtr) return;
              unlock();
            },
            [this, wPtr]() {
              if (!wPtr) return;
              m_operations->resetConnection();
              unlock();
            });
  } else {
    unlock();
  }
}

void ServerItem::unload() {
  if (!isDatabaseListLoaded()) return;

  lock();

  emit m_model.itemChildsUnloaded(m_self);

  m_operations->disconnect();
  m_databases.clear();

  unlock();
}

void ServerItem::reload() {
  unload();
  load();
}

void ServerItem::edit() {
  unload();
  emit editActionRequested();
}

void ServerItem::remove() {
  unload();
  emit deleteActionRequested();
}

void ServerItem::openConsole() { m_operations->openConsoleTab(); }

QHash<QString, std::function<void()> > ServerItem::eventHandlers() {
  auto events = TreeItem::eventHandlers();

  events.insert("click", [this]() {
    if (isDatabaseListLoaded()) return;

    load();
  });

  events.insert("console", [this]() { m_operations->openConsoleTab(); });

  events.insert("server_info", [this]() { m_operations->openServerStats(); });

  events.insert("duplicate", [this]() { m_operations->duplicateConnection(); });

  events.insert("reload", [this]() {
    reload();
    emit m_model.itemChanged(getSelf());
  });

  events.insert("unload", [this]() { unload(); });

  events.insert("edit", [this]() {
    auto unloadAction = [this]() {
      unload();
      emit editActionRequested();
    };

    if (m_operations->isConnected()) {
      confirmAction(nullptr,
                    QCoreApplication::translate(
                        "RDM",
                        "Value and Console tabs related to this "
                        "connection will be closed. Do you want to continue?"),
                    unloadAction);
    } else {
      unloadAction();
    }
  });

  events.insert("delete", [this]() {
    confirmAction(nullptr,
                  QCoreApplication::translate(
                      "RDM", "Do you really want to delete connection?"),
                  [this]() {
                    unload();
                    emit deleteActionRequested();
                  });
  });

  return events;
}

void ServerItem::setWeakPointer(QWeakPointer<ServerItem> self) {
  m_self = self;
  m_selfPtr = self;
}

QVariantMap ConnectionsTree::ServerItem::metadata() const {
  QVariantMap meta = TreeItem::metadata();

  if (isDatabaseListLoaded()) {
    meta["server_type"] = m_operations->mode();
  } else {
    meta["server_type"] = "unknown";
  }

  return meta;
}
