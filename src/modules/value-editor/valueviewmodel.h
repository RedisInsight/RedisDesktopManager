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

  Q_PROPERTY(bool isLoaded READ isModelLoaded NOTIFY modelLoaded)
  Q_PROPERTY(bool singlePageMode READ singlePageMode WRITE setSinglePageMode NOTIFY singlePageModeChanged)
  Q_PROPERTY(int totalRowCount READ totalRowCount NOTIFY totalRowCountChanged)
  Q_PROPERTY(int pageSize READ pageSize NOTIFY pageSizeChanged)
  Q_PROPERTY(
      QVariantList columnNames READ columnNames NOTIFY columnNamesChanged)

 public:
  ValueViewModel(const QString& loadingTitle);
  ~ValueViewModel() override {}

  int rowCount(const QModelIndex& parent = QModelIndex()) const override;

  int columnCount(const QModelIndex& parent = QModelIndex()) const override;

  QVariant data(const QModelIndex& index, int role) const;
  QHash<int, QByteArray> roleNames() const;

  QSharedPointer<Model> model();
  void setModel(QSharedPointer<Model> model);

  QString tabLoadingTitle() const;

  void close();

 public:
  // general key operations
  Q_INVOKABLE void renameKey(const QString& newKeyName);
  Q_INVOKABLE void setTTL(const QString& newTTL);
  Q_INVOKABLE void persistKey();
  Q_INVOKABLE void removeKey();

  // single row operations
  Q_INVOKABLE bool isRowLoaded(int i);
  Q_INVOKABLE void addRow(const QVariantMap& row);
  Q_INVOKABLE void updateRow(int i, const QVariantMap& row);
  Q_INVOKABLE void deleteRow(int i);
  Q_INVOKABLE QVariantMap getRow(int i);

  // multi row operations
  Q_INVOKABLE void loadRowsCount();
  Q_INVOKABLE void loadRows(int start, int limit);
  Q_INVOKABLE void reload();

  // filters
  Q_INVOKABLE QVariant filter(const QString& key) const;
  Q_INVOKABLE void setFilter(const QString&, QVariant);

  void setSinglePageMode(bool v);
  bool singlePageMode() const;

  bool isModelLoaded() const;

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
  void singlePageModeChanged();
  void modelLoaded();
  void tabClosed();
  void valueUpdated();

 private:
  QSharedPointer<Model> m_model;
  int m_startFramePosition;
  int m_lastLoadedRowFrameSize;
  bool m_singlePageMode;
  QString m_tabTitle;
};

}  // namespace ValueEditor
