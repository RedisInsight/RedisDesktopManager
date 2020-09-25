#include "externalformattersmanager.h"
#include "app/models/configmanager.h"

#include <QCoreApplication>
#include <QDebug>
#include <QDir>
#include <QDirIterator>
#include <QJsonDocument>
#include <QJsonObject>

ValueEditor::ExternalFormattersManager::ExternalFormattersManager() {}

QPair<QByteArray, QByteArray>
ValueEditor::ExternalFormattersManager::readOutputFromExternalProcess(
    const QStringList &cmd, const QByteArray &processInput, const QString &wd) {
  auto formatterProcess = createProcess();
  formatterProcess->setWorkingDirectory(wd);
  formatterProcess->start(cmd[0], cmd.mid(1));

  if (processInput.size()) {
    formatterProcess->write(processInput.constData(), processInput.size());
    formatterProcess->waitForBytesWritten();
    formatterProcess->closeWriteChannel();
  }

  if (!formatterProcess->waitForStarted(3000)) {
    emit error(QString("Cannot start process %1: %2")
                   .arg(cmd.join(" "))
                   .arg(formatterProcess->errorString()));
    return {QByteArray(), QByteArray()};
  }

  if (!formatterProcess->waitForFinished(3000)) {
    formatterProcess->kill();
    emit error(QString("Process %1 was killed by timeout: %2")
                   .arg(cmd.join(" "))
                   .arg(formatterProcess->errorString()));
    return {QByteArray(), QByteArray()};
  }

  return {formatterProcess->readAllStandardOutput(),
          formatterProcess->readAllStandardError()};
}

QJsonObject ValueEditor::ExternalFormattersManager::readJsonFromExternalProcess(
    const QStringList &cmd, const QByteArray &processInput, const QString &wd) {
  qDebug() << cmd;

  auto result = readOutputFromExternalProcess(cmd, processInput, wd);

  QJsonObject obj;

  if (result.second.size() > 0) {
    auto err =
        QString("%2: %1").arg(QString::fromLocal8Bit(result.second)).arg(wd);
    obj["error"] = err;
    emit error(err);
  }

  if (result.first.isEmpty()) return obj;

  QJsonParseError err;
  QJsonDocument output = QJsonDocument::fromJson(result.first, &err);

  if (err.error != QJsonParseError::NoError || !output.isObject()) {
    auto err = QString("Formatter returned invalid json: %1")
                   .arg(QString::fromLocal8Bit(result.first));
    emit error(err);
    obj["error"] = err;
    return obj;
  }

  return output.object();
}

void ValueEditor::ExternalFormattersManager::loadFormatters() {
  // Load formatters from file system
  QDir fd(formattersPath());
  fd.setFilter(QDir::NoDotAndDotDot | QDir::Dirs);

  if (!fd.exists() && fd.mkpath(formattersPath())) {
    qDebug() << "Formatters Dir created";
  }

  QDirIterator it(fd);

  while (it.hasNext()) {
    it.next();

    QFile usageFile(QString("%1/%2").arg(it.filePath()).arg("usage.json"));

    if (usageFile.exists() && usageFile.open(QIODevice::ReadOnly)) {
      QString usageRawJson = QString(usageFile.readAll()).simplified();

      QJsonParseError err;
      QJsonDocument cmd = QJsonDocument::fromJson(usageRawJson.toUtf8(), &err);

      if (err.error != QJsonParseError::NoError || !cmd.isArray()) {
        emit error(QString("Formatter %1 has invalid usage.json file")
                       .arg(it.filePath()));
        continue;
      }

      QStringList fullCmd = cmd.toVariant().toStringList();
      QStringList versionCmd(fullCmd);
      versionCmd.append("info");

      QJsonObject outputObj =
          readJsonFromExternalProcess(versionCmd, QByteArray(), it.filePath());

      if (outputObj.isEmpty()) {
        emit error(
            QString("Formatter %1 returned empty output for info command")
                .arg(it.filePath()));
        continue;
      }

      QVariantMap data;
      data["name"] = it.fileName();
      data["version"] = outputObj["version"].toString();
      data["description"] = outputObj["description"].toString();
      data["cmd"] = fullCmd.join(" ");
      data["cmd_list"] = fullCmd;
      data["cwd"] = it.filePath();

      if (outputObj.contains("read-only")) {
        data["read_only"] = outputObj["read-only"].toBool();
      } else {
        data["read_only"] = true;
      }

      m_formattersData.append(data);
      usageFile.close();
    }
  }

  fillMapping();
}

int ValueEditor::ExternalFormattersManager::rowCount(
    const QModelIndex &) const {
  return m_formattersData.size();
}

QVariant ValueEditor::ExternalFormattersManager::data(const QModelIndex &index,
                                                      int role) const {
  if (!(0 <= index.row() && index.row() < rowCount())) {
    return QVariant();
  }

  QVariantMap data = m_formattersData[index.row()];

  if (role == name) {
    return data["name"];
  } else if (role == version) {
    return data["version"];
  } else if (role == cmd) {
    return data["cmd"];
  } else if (role == description) {
    return data["description"];
  }

  return QVariant();
}

QHash<int, QByteArray> ValueEditor::ExternalFormattersManager::roleNames()
    const {
  QHash<int, QByteArray> roles;
  roles[name] = "name";
  roles[version] = "version";
  roles[description] = "description";
  roles[cmd] = "cmd";
  return roles;
}

void ValueEditor::ExternalFormattersManager::setPath(const QString &path) {
  m_formattersPath = path;
}

void ValueEditor::ExternalFormattersManager::decode(
    const QString &formatterName, const QByteArray &data, QJSValue jsCallback) {
  if (!m_mapping.contains(formatterName)) {
    emit error(
        QCoreApplication::translate("RDM", "Can't find formatter with name: %1")
            .arg(formatterName));
    return;
  }

  if (!jsCallback.isCallable()) {
    emit error(QCoreApplication::translate("RDM", "Invalid callback"));
    return;
  }

  QVariantMap formatter = m_formattersData[m_mapping[formatterName]];

  QStringList cmd = formatter["cmd_list"].toStringList();
  cmd.append("decode");

  QJsonObject outputObj = readJsonFromExternalProcess(
      cmd, data.toBase64(), formatter["cwd"].toString());

  if (outputObj.isEmpty()) {
    jsCallback.call(
        QJSValueList{QCoreApplication::translate(
                         "RDM", "Cannot decode value using %1 formatter. ")
                         .arg(formatterName)});
    return;
  }

  jsCallback.call(QJSValueList{
      outputObj["error"].toString(), outputObj["output"].toString(),
      outputObj["read-only"].toBool(), outputObj["format"].toString()});
}

void ValueEditor::ExternalFormattersManager::isValid(
    const QString &formatterName, const QByteArray &data, QJSValue jsCallback) {
  if (!m_mapping.contains(formatterName)) {
    emit error(
        QCoreApplication::translate("RDM", "Can't find formatter with name: %1")
            .arg(formatterName));
    return;
  }

  QVariantMap formatter = m_formattersData[m_mapping[formatterName]];

  QStringList cmd = formatter["cmd_list"].toStringList();
  cmd.append("validate");

  QJsonObject outputObj = readJsonFromExternalProcess(
      cmd, data.toBase64(), formatter["cwd"].toString());

  if (outputObj.isEmpty()) {
    emit error(QCoreApplication::translate(
                   "RDM", "Cannot validate value using %1 formatter.")
                   .arg(formatterName));
    return;
  }

  if (jsCallback.isCallable()) {
    jsCallback.call(QJSValueList{outputObj["valid"].toBool()});
  }
}

void ValueEditor::ExternalFormattersManager::encode(
    const QString &formatterName, const QByteArray &data, QJSValue jsCallback) {
  if (!m_mapping.contains(formatterName)) {
    emit error(
        QCoreApplication::translate("RDM", "Can't find formatter with name: %1")
            .arg(formatterName));
    return;
  }

  QVariantMap formatter = m_formattersData[m_mapping[formatterName]];

  QStringList cmd = formatter["cmd_list"].toStringList();
  cmd.append("encode");

  auto result = readJsonFromExternalProcess(cmd, data.toBase64(),
                                            formatter["cwd"].toString());

  if (result.isEmpty()) {
    emit error(QCoreApplication::translate(
                   "RDM", "Cannot encode value using %1 formatter. ")
                   .arg(formatterName));
    return;
  }

  if (jsCallback.isCallable()) {
    jsCallback.call(QJSValueList{result["error"].toString(), result["output"].toString()});
  }
}

QVariantList ValueEditor::ExternalFormattersManager::getPlainList() {
    QList<QVariant> r;
    foreach (QVariantMap v, m_formattersData) {
        r.append(v);
    }
    return r;
}

QString ValueEditor::ExternalFormattersManager::formattersPath() {
  if (m_formattersPath.isEmpty()) {
    return QDir::toNativeSeparators(
        QString("%1/%2").arg(ConfigManager::getConfigPath()).arg("formatters"));
  } else {
    return m_formattersPath;
  }
}

bool ValueEditor::ExternalFormattersManager::isInstalled(const QString &name) {
  return m_mapping.contains(name);
}

void ValueEditor::ExternalFormattersManager::fillMapping() {
  int index = 0;

  for (QVariantMap f : m_formattersData) {
    m_mapping[f["name"].toString()] = index;
    index++;
  }
}

QSharedPointer<QProcess>
ValueEditor::ExternalFormattersManager::createProcess() {
  auto process = QSharedPointer<QProcess>(new QProcess());

  QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
#ifdef Q_OS_WIN
  env.insert("PATH", QString("%1/python;%2")
                         .arg(QCoreApplication::applicationDirPath())
                         .arg(env.value("PATH", "")));
#elif defined Q_OS_MACOS
  env.insert("PATH", QString("/usr/local/bin:/usr/bin:/bin:/usr/sbin:/sbin:%1")
                         .arg(env.value("PATH", "")));
#endif
  process->setProcessEnvironment(env);
  return process;
}
