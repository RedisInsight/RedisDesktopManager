#pragma once
#include <QAbstractListModel>
#include <QJSValue>
#include <QSharedPointer>
#include <QVariantMap>
#include "common/baselistmodel.h"
#include "keymodel.h"

namespace ValueEditor {

class ValueViewModel : public BaseListModel {
  Q_OBJECT

  Q_PROPERTY(int totalRowCount READ totalRowCount NOTIFY totalRowCountChanged)
  Q_PROPERTY(int pageSize READ pageSize NOTIFY pageSizeChanged)
  Q_PROPERTY(
      QVariantList columnNames READ columnNames NOTIFY columnNamesChanged)

 public:
  ValueViewModel(QSharedPointer<Model> model);
  ~ValueViewModel() {}

  int rowCount(const QModelIndex& parent = QModelIndex()) const override;
  QVariant data(const QModelIndex& index, int role) const;
  QHash<int, QByteArray> roleNames() const;

  QSharedPointer<Model> model();

 public:
  // general key operations
  Q_INVOKABLE void renameKey(const QString& newKeyName, QJSValue jsCallback);
  Q_INVOKABLE void setTTL(const QString& newTTL, QJSValue jsCallback);
  Q_INVOKABLE void removeKey();

  // single row operations
  Q_INVOKABLE bool isRowLoaded(int i);
  Q_INVOKABLE void addRow(const QVariantMap& row);
  Q_INVOKABLE void updateRow(int i, const QVariantMap& row);
  Q_INVOKABLE void deleteRow(int i);
  Q_INVOKABLE QVariantMap getRow(int i);

  // multi row operations
  Q_INVOKABLE void loadRowsCount(QJSValue jsCallback);
  Q_INVOKABLE void loadRows(int start, int limit);
  Q_INVOKABLE void reload();

  int totalRowCount();
  int pageSize();
  QVariantList columnNames();

 signals:
  void rowsLoaded(int start, int count);
  void error(QString error);
  void totalRowCountChanged();
  void pageSizeChanged();
  void columnNamesChanged();
  void keyRenamed();
  void keyRemoved();
  void keyTTLChanged();

 protected:
  int mapRowIndex(int i);

 private:
  QSharedPointer<Model> m_model;
  int m_startFramePosition;
  int m_lastLoadedRowFrameSize;
};

}  // namespace ValueEditor
