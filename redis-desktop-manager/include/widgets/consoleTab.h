#pragma once

/**
	Based on Console Qt widget from RedisConsole 
	https://github.com/ptrofimov/RedisConsole
	author Petr Trofimov (petrofimov@yandex.ru)
**/

#include <QPlainTextEdit>

class RedisConnectionConfig;
class ConsoleConnectionWrapper;

class consoleTab: public QPlainTextEdit
{
	Q_OBJECT

public:
	consoleTab(RedisConnectionConfig&);
	~consoleTab(void);
	void output(QString);
	void scrollDown();

protected:
	void keyPressEvent(QKeyEvent *);
	void mousePressEvent(QMouseEvent *);
	void mouseDoubleClickEvent(QMouseEvent *);
	void contextMenuEvent(QContextMenuEvent *);
private:
	QString prompt;
	bool isLocked;
	QStringList *history;
	int historyPos;
	ConsoleConnectionWrapper * connection;
	//QThread connectionThread;

	void onEnter();
	void insertPrompt(bool insertNewBlock = true);
	void historyAdd(QString);
	void historyBack();
	void historyForward();
signals:
	void onCommand(QString);
	void onChange(QString);
};

