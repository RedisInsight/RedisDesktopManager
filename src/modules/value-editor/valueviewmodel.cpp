#include "valueviewmodel.h"
#include <qredisclient/utils/text.h>
#include <QCoreApplication>
#include <QDebug>
#include <QQmlEngine>
#include <QSettings>

ValueEditor::ValueViewModel::ValueViewModel(const QString& loadingTitle)
    : BaseListModel(),
      m_startFramePosition(0),
      m_lastLoadedRowFrameSize(0),
      m_singlePageMode(false),
      m_tabTitle(loadingTitle)
{}

int ValueEditor::ValueViewModel::rowCount(const QModelIndex& parent) const {
  Q_UNUSED(parent);

  if (!m_model) return 0;

  if (m_singlePageMode) {
    return m_model->rowsCount();
  } else {
    return m_lastLoadedRowFrameSize;
  }
}

int ValueEditor::ValueViewModel::columnCount(const QModelIndex& parent) const {
    Q_UNUSED(parent);

    if (!m_model) return 0;

    return m_model->getColumnNames().size();
}

QString ValueEditor::ValueViewModel::tabLoadingTitle() const {
    return m_tabTitle;
}

bool ValueEditor::ValueViewModel::isModelLoaded() const {
    return !m_model.isNull();
}

QVariant ValueEditor::ValueViewModel::data(const QModelIndex& index,
                                           int role) const {
  if (!isIndexValid(index)) return QVariant();

  int mappedRole = role;

  if (role == Qt::DisplayRole && index.column() > 0) {
      mappedRole = m_model->getRoles().key(m_model->getColumnNames().at(index.column()).toLatin1());
  }

  return m_model->getData(m_startFramePosition + index.row(), mappedRole);
}

QHash<int, QByteArray> ValueEditor::ValueViewModel::roleNames() const {
  auto roles = m_model->getRoles();
  roles.insert(Qt::DisplayRole, "display");
  return roles;
}

QSharedPointer<ValueEditor::Model> ValueEditor::ValueViewModel::model() {
  return m_model;
}

void ValueEditor::ValueViewModel::setModel(QSharedPointer<Model> model) {
  m_model = model;
  emit modelLoaded();
}

void ValueEditor::ValueViewModel::renameKey(const QString& newKeyName) {
  if (!m_model) {
    qWarning() << "Model is not loaded";
    return;
  }

  m_model->setKeyName(printableStringToBinary(newKeyName),
                      [this](const QString& err) {
                        if (err.size() > 0) {
                          emit error(err);
                          return;
                        }

                        emit keyRenamed();
                      });
}

void ValueEditor::ValueViewModel::setTTL(const QString& newTTL) {
  if (!m_model) {
    qWarning() << "Model is not loaded";
    return;
  }

  m_model->setTTL(newTTL.toLong(), [this](const QString& err) {
    if (err.size() > 0) {
      emit error(err);
      return;
    }

    emit keyTTLChanged();
  });
}

void ValueEditor::ValueViewModel::persistKey() {
    if (!m_model) {
      qWarning() << "Model is not loaded";
      return;
    }

    m_model->persistKey([this](const QString& err) {
      if (err.size() > 0) {
        emit error(err);
        return;
      }

      emit keyTTLChanged();
    });
}

void ValueEditor::ValueViewModel::removeKey() {
  if (!m_model) {
    qWarning() << "Model is not loaded";
    return;
  }

  m_model->removeKey([this](const QString& err) {
    if (err.size() > 0) {
      emit error(err);
      return;
    }

    emit keyRemoved();
  });
}

void ValueEditor::ValueViewModel::close()
{
    emit tabClosed();
}

QVariantList ValueEditor::ValueViewModel::columnNames() {
  QVariantList result;

  if (!m_model) return result;

  foreach (QString str, m_model->getColumnNames()) {
    result.append(QVariant(str));
  }

  return result;
}

void ValueEditor::ValueViewModel::reload() {
  if (!m_model) {
    qWarning() << "Model is not loaded";
    return;
  }

  m_model->clearRowCache();
  m_model->loadRowsCount([this](const QString& err) {
    if (err.size() > 0 || m_model->rowsCount() <= 0) {
      emit error(
          QCoreApplication::translate("RDM", "Cannot reload key value: %1")
              .arg(err));
      return;
    }

    emit totalRowCountChanged();
    emit pageSizeChanged();

    loadRows(m_startFramePosition, m_model->rowsCount() < pageSize()
                                       ? m_model->rowsCount()
                                       : pageSize());
  });
}

void ValueEditor::ValueViewModel::setSinglePageMode(bool v) {
  m_singlePageMode = v;
  emit singlePageModeChanged();
}

bool ValueEditor::ValueViewModel::singlePageMode() const {
  return m_singlePageMode;
}

bool ValueEditor::ValueViewModel::isRowLoaded(int i) {
  if (!m_model) {
    qWarning() << "Model is not loaded";
    return false;
  }

  return m_model->isRowLoaded(i);
}

void ValueEditor::ValueViewModel::loadRows(int start, int limit) {
  if (!m_model) {
    qWarning() << "Model is not loaded";
    return;
  }

  int rowsLeft = totalRowCount() - start;
  int loaded = (rowsLeft > limit) ? limit : rowsLeft;

  // frame already loaded
  if (m_model->isRowLoaded(start) && m_model->isRowLoaded(start + loaded - 1)) {
    m_startFramePosition = start;
    m_lastLoadedRowFrameSize = loaded;

    emit layoutAboutToBeChanged();
    emit rowsLoaded(start, loaded);
    emit layoutChanged();
    return;
  }

  QString msg = QCoreApplication::translate("RDM", "Cannot load key value: %1");

  m_model->loadRows(
      start, limit,
      [this, start, limit, msg](const QString& err, unsigned long rowsCount) {
        if (!err.isEmpty()) {
          emit error(msg.arg(err));
          return;
        }

        m_lastLoadedRowFrameSize = rowsCount > limit ? limit : rowsCount;
        m_startFramePosition = start;

        emit layoutAboutToBeChanged();
        emit rowsLoaded(start, m_lastLoadedRowFrameSize);
        emit layoutChanged();
      });
}

void ValueEditor::ValueViewModel::addRow(const QVariantMap& row) {
  if (!m_model) {
    qWarning() << "Model is not loaded";
    return;
  }

  m_model->addRow(row, [this](const QString& err) {
    if (err.size() > 0) {
      emit error(err);
      return;
    }
    emit layoutChanged();
  });
}

void ValueEditor::ValueViewModel::updateRow(int rowIndex, const QVariantMap& row) {
  if (!m_model) {
    qWarning() << "Model is not loaded";
    return;
  }

  if (rowIndex < 0 || !m_model->isRowLoaded(rowIndex)) return;

  m_model->updateRow(rowIndex, row, [this, rowIndex](const QString& err) {
    if (err.size() > 0) {
      emit error(err);
      return;
    }
    emit dataChanged(index(rowIndex, 0), index(rowIndex, m_model->getColumnNames().size() - 1));
    emit valueUpdated();
  });
}

void ValueEditor::ValueViewModel::deleteRow(int rowIndex) {
  if (!m_model) {
    qWarning() << "Model is not loaded";
    return;
  }

  if (rowIndex < 0 || !m_model->isRowLoaded(rowIndex)) return;

  m_model->removeRow(rowIndex, [this, rowIndex](const QString& err) {
    if (err.size() > 0) {
      emit error(err);
      return;
    }

    emit beginRemoveRows(QModelIndex(), rowIndex, rowIndex);
    emit endRemoveRows();

    if (m_lastLoadedRowFrameSize > 0)
        m_lastLoadedRowFrameSize -= 1;

    if (m_model->rowsCount() == 0) emit keyRemoved();
  });
}

int ValueEditor::ValueViewModel::totalRowCount() {
  if (!m_model) {
    qWarning() << "Model is not loaded";
    return 0;
  }

  return m_model->rowsCount();
}

int ValueEditor::ValueViewModel::pageSize() {
  QSettings settings;

  return settings.value("app/valueEditorPageSize", 100).toInt();
}

QVariantMap ValueEditor::ValueViewModel::getRow(int rowIndex) {
  if (!m_model) {
    qWarning() << "Model is not loaded";
    return QVariantMap();
  }

  if (rowIndex < 0 || !m_model->isRowLoaded(rowIndex)) return QVariantMap();

  QHash<int,QByteArray> names = roleNames();
  QHashIterator<int, QByteArray> i(names);
  QVariantMap res;

  while (i.hasNext()) {
      i.next();

      if (i.value() == "display")
          continue;

      QVariant d = m_model->getData(rowIndex, i.key());
      res[i.value()] = d;
  }

  return res;
}

void ValueEditor::ValueViewModel::loadRowsCount() {
  if (!m_model) {
    qWarning() << "Model is not loaded";
    return;
  }

  m_model->loadRowsCount([this](const QString& err) {
    if (err.size() > 0) {
      emit error(err);
      return;
    }

    emit totalRowCountChanged();
  });
}

QVariant ValueEditor::ValueViewModel::filter(const QString& key) const
{
    if (!m_model) {
      qWarning() << "Model is not loaded";
      return QVariant();
    }

    return m_model->filter(key);
}

void ValueEditor::ValueViewModel::setFilter(const QString& key, QVariant v)
{
    if (!m_model) {
      qWarning() << "Model is not loaded";
      return;
    }

    return m_model->setFilter(key, v);
}
