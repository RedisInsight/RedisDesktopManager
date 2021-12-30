#include "consolemodel.h"
#include <qredisclient/redisclient.h>
#include <QCoreApplication>

using namespace Console;

Model::Model(QSharedPointer<RedisClient::Connection> connection, int dbIndex,
             QList<QByteArray> initCmd)
    : TabModel(connection, dbIndex), m_current_db(dbIndex) {
  QObject::connect(this, &TabModel::initialized, [this, initCmd]() {
    if (m_connection->mode() == RedisClient::Connection::Mode::Cluster) {
      emit addOutput(
          QCoreApplication::translate("RESP", "Connected to cluster.\n"),
          "complete");
    } else {
      emit addOutput(QCoreApplication::translate("RESP", "Connected.\n"),
                     "complete");
    }

    updatePrompt(true);

    if (initCmd.size() > 0)
        execCmd(initCmd);
  });

  QObject::connect(this, &TabModel::error, [this](const QString& msg) {
    emit addOutput(msg, "error");
  });
}

QString Model::getName() const { return m_connection->getConfig().name(); }

void Model::executeCommand(const QString& cmd) {
  return execCmd(RedisClient::Command::splitCommandString(cmd));
}

void Model::updatePrompt(bool showPrompt) {
  if (m_connection->mode() == RedisClient::Connection::Mode::Cluster) {
    emit changePrompt(QString("%1(%2:%3)>")
                          .arg(m_connection->getConfig().name())
                          .arg(m_connection->getConfig().host())
                          .arg(m_connection->getConfig().port()),
                      showPrompt);
  } else {
    emit changePrompt(QString("%1:%2>")
                          .arg(m_connection->getConfig().name())
                          .arg(m_current_db),
                      showPrompt);
  }
}

void Model::execCmd(QList<QByteArray> cmd)
{
    using namespace RedisClient;

    Command command(cmd, m_current_db);

    if (command.isSubscriptionCommand() || command.isMonitorCommand()) {
      emit addOutput(
          QCoreApplication::translate(
              "RESP",
              "Switch to %1 mode. Close console tab to stop listen for "
              "messages.").arg(command.isSubscriptionCommand()? "Pub/Sub": "Monitor"),
          "part");

      command.setCallBack(this, [this](Response result, QString err) {
        if (!err.isEmpty()) {
          emit addOutput(
              QCoreApplication::translate("RESP", "Subscribe error: %1").arg(err),
              "error");
          return;
        }

        QVariant value = result.value();
        emit addOutput(RedisClient::Response::valueToHumanReadString(value).replace("\r\n", "\n"),
                       "part");
      });

      m_connection->command(command);
    } else {
        bool isSelect = command.isSelectCommand();
        command.setCallBack(this, [this, isSelect](Response result, QString err) {
          if (!err.isEmpty()) {
              emit addOutput(QCoreApplication::translate("RESP", "Connection error: ") +
                                 QString(err),
                             "error");
            return;
          }

          if (isSelect || m_connection->mode() == RedisClient::Connection::Mode::Cluster) {
            m_current_db = m_connection->dbIndex();
            updatePrompt(false);
          }

          QVariant value = result.value();
          emit addOutput(RedisClient::Response::valueToHumanReadString(value).replace("\r\n", "\n"),
                         "complete");
        });

        m_connection->command(command);
    }
}
