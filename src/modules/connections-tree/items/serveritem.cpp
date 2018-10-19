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

ServerItem::ServerItem(const QString& name,
                       QSharedPointer<Operations> operations, Model& model)
    : TreeItem(model), m_name(name), m_row(0), m_operations(operations) {
  m_eventHandlers.insert("click", [this]() {
    if (isDatabaseListLoaded()) return;

    load();
  });

  m_eventHandlers.insert("console",
                         [this]() { m_operations->openConsoleTab(); });

  m_eventHandlers.insert("server_info",
                         [this]() { m_operations->openServerStats(); });

  m_eventHandlers.insert("duplicate",
                         [this]() { m_operations->duplicateConnection(); });

  m_eventHandlers.insert("reload", [this]() {
    reload();
    emit m_model.itemChanged(getSelf());
  });

  m_eventHandlers.insert("unload", [this]() { unload(); });

  m_eventHandlers.insert("edit", [this]() {
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

  m_eventHandlers.insert("delete", [this]() {
    confirmAction(nullptr,
                  QCoreApplication::translate(
                      "RDM", "Do you really want to delete connection?"),
                  [this]() {
                    unload();
                    emit deleteActionRequested();
                  });
  });
}

ServerItem::~ServerItem() {}

QString ServerItem::getDisplayName() const { return m_name; }

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
  return QWeakPointer<TreeItem>();
}

int ServerItem::row() const { return m_row; }

void ServerItem::setRow(int r) { m_row = r; }

bool ServerItem::isEnabled() const { return true; }

bool ServerItem::isDatabaseListLoaded() const {
  return isLocked() == false && m_databases.size() > 0;
}

QSharedPointer<Operations> ServerItem::getOperations() { return m_operations; }

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
    AsyncFuture::observe(m_currentOperation)
        .subscribe([this]() { unlock(); }, [this]() { unlock(); });
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

void ServerItem::setName(const QString& name) { m_name = name; }

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
