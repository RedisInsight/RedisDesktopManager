#pragma once
#include <QObject>

class TestDatabaseItem : public QObject {
  Q_OBJECT
 public:
  explicit TestDatabaseItem(QObject *parent = 0);

 private slots:
  void testLoadKeys();
  // void testUnloadKeys();
};
