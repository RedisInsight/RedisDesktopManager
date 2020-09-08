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
          QCoreApplication::translate("RDM", "Connected to cluster.\n"),
          "complete");
    } else {
      emit addOutput(QCoreApplication::translate("RDM", "Connected.\n"),
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
  if (cmd == "segfault") {  // crash
    delete reinterpret_cast<QString*>(0xFEE1DEAD);
    return;
  }   

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

    if (command.isSubscriptionCommand()) {
      emit addOutput(
          QCoreApplication::translate(
              "RDM",
              "Switch to Pub/Sub mode. Close console tab to stop listen for "
              "messages."),
          "part");

      command.setCallBack(this, [this](Response result, QString err) {
        if (!err.isEmpty()) {
          emit addOutput(
              QCoreApplication::translate("RDM", "Subscribe error: %1").arg(err),
              "error");
          return;
        }

        QVariant value = result.value();
        emit addOutput(RedisClient::Response::valueToHumanReadString(value),
                       "part");
      });

      m_connection->command(command);
    } else {
        bool isSelect = command.isSelectCommand();
        command.setCallBack(this, [this, isSelect](Response result, QString err) {
          if (!err.isEmpty()) {
              emit addOutput(QCoreApplication::translate("RDM", "Connection error: ") +
                                 QString(err),
                             "error");
            return;
          }

          if (isSelect || m_connection->mode() == RedisClient::Connection::Mode::Cluster) {
            m_current_db = m_connection->dbIndex();
            updatePrompt(false);
          }

          QVariant value = result.value();
          emit addOutput(RedisClient::Response::valueToHumanReadString(value),
                         "complete");
        });

        m_connection->command(command);
    }
}
