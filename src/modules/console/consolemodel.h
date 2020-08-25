#pragma once
#include "common/tabviewmodel.h"
#include "exception.h"

namespace Console {

class Model : public TabModel {
  Q_OBJECT
  ADD_EXCEPTION
 public:
  Model(QSharedPointer<RedisClient::Connection> connection, int dbIndex, QList<QByteArray> initCmd);

  QString getName() const override;

 public slots:
  void executeCommand(const QString &);

 signals:
  void changePrompt(const QString &text, bool showPrompt);
  void addOutput(const QString &text, QString resultType);

 private:
  int m_current_db;

 private:
  void updatePrompt(bool showPrompt);
  void execCmd(QList<QByteArray> cmd);
};
}  // namespace Console
