#include "keyitem.h"
#include <qredisclient/utils/text.h>
#include <QCoreApplication>
#include <QMenu>
#include <QMessageBox>

#include "connections-tree/model.h"
#include "connections-tree/utils.h"

using namespace ConnectionsTree;

KeyItem::KeyItem(const QByteArray& fullPath, unsigned short dbIndex,
                 QSharedPointer<Operations> operations,
                 QWeakPointer<TreeItem> parent, Model& model)
    : TreeItem(model),
      m_fullPath(fullPath),
      m_dbIndex(dbIndex),
      m_operations(operations),
      m_parent(parent),
      m_removed(false) {
  m_eventHandlers.insert("click", [this]() {
    if (isEnabled()) m_operations->openKeyTab(*this, false);
  });

  m_eventHandlers.insert("mid-click", [this]() {
    if (isEnabled()) m_operations->openKeyTab(*this, true);
  });

  m_eventHandlers.insert("delete", [this]() {
    confirmAction(nullptr,
                  QCoreApplication::translate(
                      "RDM", "Do you really want to delete this key?"),
                  [this]() {
                    m_operations->deleteDbKey(*this, [](const QString& error) {
                      QMessageBox::warning(
                          nullptr,
                          QCoreApplication::translate("RDM", "Key error"),
                          error);
                    });
                  });
  });
}

QString KeyItem::getDisplayName() const {
  return printableString(m_fullPath, true);
}

QByteArray KeyItem::getName() const { return m_fullPath; }

QList<QSharedPointer<TreeItem>> KeyItem::getAllChilds() const {
  return QList<QSharedPointer<TreeItem>>();
}

bool KeyItem::supportChildItems() const { return false; }

uint KeyItem::childCount(bool) const { return 0u; }

QSharedPointer<TreeItem> KeyItem::child(uint) const {
  return QSharedPointer<TreeItem>();
}

QWeakPointer<TreeItem> KeyItem::parent() const { return m_parent; }

bool KeyItem::isEnabled() const {
  if (!m_removed && m_parent) {
    return m_parent.toStrongRef()->isEnabled();
  } else {
    return m_removed == false;
  }
}

QByteArray KeyItem::getFullPath() const { return m_fullPath; }

int KeyItem::getDbIndex() const { return m_dbIndex; }

void KeyItem::setRemoved() {
  m_removed = true;

  emit m_model.itemChanged(getSelf());
}
