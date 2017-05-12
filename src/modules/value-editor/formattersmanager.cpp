#include "formattersmanager.h"
#include "app/models/configmanager.h"

#include <QDebug>
#include <QDir>
#include <QDirIterator>
#include <QProcess>
#include <QJsonDocument>
#include <QJsonObject>
#include <easylogging++.h>

ValueEditor::FormattersManager::FormattersManager()
{

}

QByteArray readStdoutFromExternalProcess(const QStringList& cmd, const QString& wd)
{
    QProcess formatterProcess;
    formatterProcess.setWorkingDirectory(wd);
    formatterProcess.start(cmd[0], cmd.mid(1));

    if (!formatterProcess.waitForStarted(3000)) {
        LOG(ERROR) << QString("Cannot start process %1: ").arg(cmd.join(" "))
                   << formatterProcess.errorString();
        return QByteArray();
    }

    if (!formatterProcess.waitForFinished(3000)) {
        formatterProcess.kill();
        LOG(ERROR) << QString("Process %1 was killed by timeout: ").arg(cmd.join(" "))
                   << formatterProcess.errorString();
        return QByteArray();
    }

    return formatterProcess.readAllStandardOutput();
}


QJsonObject readJsonFromExternalProcess(const QStringList& cmd, const QString& wd)
{
    qDebug() << cmd;

    QByteArray result = readStdoutFromExternalProcess(cmd, wd);

    if (result.isEmpty())
        return QJsonObject();

    QJsonParseError err;
    QJsonDocument output = QJsonDocument::fromJson(result, &err);

    if (err.error != QJsonParseError::NoError
            || !output.isObject()) {
        LOG(ERROR) << QString("Formatter returned invalid json");
        return QJsonObject();
    }

    return output.object();
}


void ValueEditor::FormattersManager::loadFormatters(bool ignoreCache)
{
    if (!ignoreCache) {
        // Try to load data from cache
        //fillMapping();
        // if still empty - continue loading
    }

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

            if (err.error != QJsonParseError::NoError
                    || !cmd.isArray()) {
                LOG(ERROR) << QString("Formatter %1 has invalid usage.json file");
                continue;
            }

            QStringList fullCmd = cmd.toVariant().toStringList();
            QStringList versionCmd(fullCmd);
            versionCmd.append("--version");

            QByteArray result = readStdoutFromExternalProcess(versionCmd, it.filePath());

            if (result.isEmpty())
                continue;

            QVariantMap data;
            data["name"] = it.fileName();
            data["version"] = result.simplified();
            data["cmd"] = fullCmd.join(" ");
            data["cmd_list"] = fullCmd;
            data["cwd"] = it.filePath();

            m_formattersData.append(data);
            usageFile.close();
        }
    }

    fillMapping();
}

int ValueEditor::FormattersManager::rowCount(const QModelIndex &) const
{
    return m_formattersData.size();
}

QVariant ValueEditor::FormattersManager::data(const QModelIndex &index, int role) const
{
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
    }

    return QVariant();
}

QHash<int, QByteArray> ValueEditor::FormattersManager::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[name] = "name";
    roles[version] = "version";
    roles[cmd] = "cmd";
    return roles;
}

void ValueEditor::FormattersManager::decode(const QString &formatterName, const QByteArray &data,
                                            QJSValue jsCallback)
{
    if (!m_mapping.contains(formatterName)) {
        emit error(QObject::tr("Can't find formatter with name: %1").arg(formatterName));
        return;
    }

    QVariantMap formatter = m_formattersData[m_mapping[formatterName]];

    QStringList cmd = formatter["cmd_list"].toStringList();
    cmd.append("decode");
    cmd.append(data.toBase64());

    QJsonObject outputObj = readJsonFromExternalProcess(cmd, formatter["cwd"].toString());

    if (outputObj.isEmpty()) {
        emit error(QObject::tr("Cannot decode value using %1 formatter. See log for more details.").arg(formatterName));
        return;
    }

    if (jsCallback.isCallable()) {
        jsCallback.call(QJSValueList { outputObj["error"].toString(),
                                       outputObj["output"].toString(),
                                       outputObj["read-only"].toBool(),
                                       outputObj["format"].toString() });
    }
}

void ValueEditor::FormattersManager::isValid(const QString &formatterName, const QByteArray &data,
                                             QJSValue jsCallback)
{
    if (!m_mapping.contains(formatterName)) {
        emit error(QObject::tr("Can't find formatter with name: %1").arg(formatterName));
        return;
    }

    QVariantMap formatter = m_formattersData[m_mapping[formatterName]];

    QStringList cmd = formatter["cmd_list"].toStringList();
    cmd.append("is_valid");
    cmd.append(data.toBase64());

    QJsonObject outputObj = readJsonFromExternalProcess(cmd, formatter["cwd"].toString());

    if (outputObj.isEmpty()) {
        emit error(QObject::tr("Cannot validate value using %1 formatter. See log for more details.").arg(formatterName));
        return;
    }

    if (jsCallback.isCallable()) {
        jsCallback.call(QJSValueList { outputObj["valid"].toBool(),
                                       outputObj["message"].toString() });
    }
}

void ValueEditor::FormattersManager::encode(const QString &formatterName, const QByteArray &data,
                                            QJSValue jsCallback)
{
    if (!m_mapping.contains(formatterName)) {
        emit error(QObject::tr("Can't find formatter with name: %1").arg(formatterName));
        return;
    }

    QVariantMap formatter = m_formattersData[m_mapping[formatterName]];

    QStringList cmd = formatter["cmd_list"].toStringList();
    cmd.append("encode");
    cmd.append(data.toBase64());

    QByteArray output = readStdoutFromExternalProcess(cmd, formatter["cwd"].toString());

    if (output.isEmpty()) {
        emit error(QObject::tr("Cannot encode value using %1 formatter. See log for more details.").arg(formatterName));
        return;
    }

    if (jsCallback.isCallable()) {
        jsCallback.call(QJSValueList { QString::fromUtf8(output) });
    }
}

QStringList ValueEditor::FormattersManager::getPlainList()
{
    return m_mapping.keys();
}

QString ValueEditor::FormattersManager::formattersPath()
{
    return QDir::toNativeSeparators(
                QString("%1/%2").arg(ConfigManager::getConfigPath()).arg("formatters")
                );
}

void ValueEditor::FormattersManager::fillMapping()
{
    int index = 0;

    for (QVariantMap f : m_formattersData) {
        m_mapping[f["name"].toString()] = index;
        index++;
    }
}
