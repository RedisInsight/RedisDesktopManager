#pragma once
#include <QAbstractListModel>
#include <QHash>
#include <QList>
#include <QSharedPointer>

namespace ValueEditor {

class LargeTextWrappingModel : public QAbstractListModel {
  // TODO(u_glide): Process out of memory exceptions

  Q_OBJECT
 public:
  LargeTextWrappingModel(const QString &text = QString(),
                         uint chunkSize = 10000);

  ~LargeTextWrappingModel();

  QHash<int, QByteArray> roleNames() const;

  int rowCount(const QModelIndex &parent = QModelIndex()) const override;

  QVariant data(const QModelIndex &index, int role) const;

  void setText(const QString &text);

 public slots:
  void cleanUp();

  QString getText();

  void setTextChunk(uint row, QString text);

 private:
  bool isIndexValid(const QModelIndex &index) const;

 private:
  uint m_chunkSize;
  QList<QString> m_textRows;
};

}  // namespace ValueEditor
