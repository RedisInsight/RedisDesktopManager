#include "autocompletemodel.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDebug>
#include <QFile>

Console::AutocompleteModel::AutocompleteModel()
{
    QFile commandsResource("://commands.json");
    if (!commandsResource.open(QIODevice::ReadOnly)) {
        qWarning() << "Cannot load list of redis commands. Autocomplete in redis console wont work.";
        return;
    }

    QByteArray commandsJsonRaw = commandsResource.readAll();

    QJsonDocument commandsJson = QJsonDocument::fromJson(commandsJsonRaw);

    if (commandsJson.isEmpty() || !commandsJson.isArray()) {
        qWarning() << "Invalid commands.json" << commandsJson;
        return;
    }

    auto commands = commandsJson.array();

    for (auto command : commands) {
        auto cmd = command.toObject();

        m_commands.append(
            CommandInfo {
                cmd["cmd"].toString(),
                cmd["arguments"].toString(),
                cmd["summary"].toString(),
                cmd["since"].toString()
            }
        );
    }
}

QModelIndex Console::AutocompleteModel::index(int row, int column, const QModelIndex &parent) const
{
    Q_UNUSED(parent);

    if (row < 0 || column < 0)
        return QModelIndex();

    return createIndex(row, 0);
}

int Console::AutocompleteModel::rowCount(const QModelIndex &parent) const
{
    return m_commands.size();
}

QVariant Console::AutocompleteModel::data(const QModelIndex &index, int role) const
{
    if (!isIndexValid(index))
        return QVariant();

    auto command = m_commands[index.row()];

    switch (role) {
        case name: return command.name;
        case arguments: return command.arguments;
        case summary: return command.summary;
        case since: return command.since;
    }

    return QVariant();
}

QHash<int, QByteArray> Console::AutocompleteModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[name] = "name";
    roles[arguments] = "arguments";
    roles[summary] = "summary";
    roles[since] = "since";
    return roles;
}

QVariantMap Console::AutocompleteModel::getRow(int i)
{
    if (!isRowIndexValid(i))
        return QVariantMap();

    return getRowRaw(i);
}
