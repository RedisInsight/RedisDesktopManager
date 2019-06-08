#include "embeddedformattersmanager.h"
#include "app/models/configmanager.h"

#include <qpython.h>
#include <QCoreApplication>
#include <QDebug>
#include <QDir>
#include <QDirIterator>
#include <QJsonDocument>
#include <QJsonObject>

ValueEditor::EmbeddedFormattersManager::EmbeddedFormattersManager(
    QSharedPointer<QPython> p)
    : m_python(p) {
  QObject::connect(m_python.data(), &QPython::error, this,
                   &EmbeddedFormattersManager::error);
}

void ValueEditor::EmbeddedFormattersManager::loadFormatters(QJSValue callback) {
  m_python->importModule_sync("formatters");
  m_python->call("formatters.get_formatters_list", QVariantList(), callback);
}

void ValueEditor::EmbeddedFormattersManager::decode(
    const QString &formatterName, const QByteArray &data, QJSValue jsCallback) {
  m_python->call("formatters.decode", QVariantList{formatterName, data},
                 jsCallback);
}

void ValueEditor::EmbeddedFormattersManager::isValid(
    const QString &formatterName, const QByteArray &data, QJSValue jsCallback) {
  m_python->call("formatters.validate", QVariantList{formatterName, data},
                 jsCallback);
}

void ValueEditor::EmbeddedFormattersManager::encode(
    const QString &formatterName, const QByteArray &data, QJSValue jsCallback) {
  m_python->call("formatters.encode", QVariantList{formatterName, data},
                 jsCallback);
}
