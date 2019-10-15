#include "largetextmodel.h"
#include <QDebug>

ValueEditor::LargeTextWrappingModel::LargeTextWrappingModel(const QString &text,
                                                            uint chunkSize)
    : m_chunkSize(chunkSize) {
  setText(text);
}

ValueEditor::LargeTextWrappingModel::~LargeTextWrappingModel() {}

QHash<int, QByteArray> ValueEditor::LargeTextWrappingModel::roleNames() const {
  QHash<int, QByteArray> roles;
  roles[Qt::UserRole + 1] = "value";
  return roles;
}

int ValueEditor::LargeTextWrappingModel::rowCount(const QModelIndex &) const {
  return m_text.size() / m_chunkSize +
         (m_text.size() % m_chunkSize == 0 ? 0 : 1);
}

QVariant ValueEditor::LargeTextWrappingModel::data(const QModelIndex &index,
                                                   int role) const {
  if (!isIndexValid(index)) return QVariant();

  if (role == Qt::UserRole + 1) {
    return m_text.mid(index.row() * m_chunkSize, m_chunkSize);
  }

  return QVariant();
}

void ValueEditor::LargeTextWrappingModel::setText(const QString &text) {
  m_text = text;
}

void ValueEditor::LargeTextWrappingModel::cleanUp() {
  emit beginRemoveRows(QModelIndex(), 0, rowCount() - 1);
  m_text.clear();
  emit endRemoveRows();
}

QString ValueEditor::LargeTextWrappingModel::getText() { return m_text; }

void ValueEditor::LargeTextWrappingModel::setTextChunk(uint row, QString text) {
  m_text.replace(row * m_chunkSize, m_chunkSize, text);
}

bool ValueEditor::LargeTextWrappingModel::isIndexValid(
    const QModelIndex &index) const {
  return 0 <= index.row() && index.row() < rowCount();
}
