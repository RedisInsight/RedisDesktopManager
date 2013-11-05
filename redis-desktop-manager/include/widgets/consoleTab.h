#pragma once


#include <QPlainTextEdit>
#include <QThread>

class RedisConnectionConfig;
class ConsoleConnectionWrapper;

class consoleTab: public QPlainTextEdit
{
	Q_OBJECT

public:
	consoleTab(RedisConnectionConfig&);
	~consoleTab(void);
	void scrollDown();

public slots:
	void output(QString);
	void setPrompt(QString);	

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
	QThread connectionThread;

	void onEnter();
	void insertPrompt(bool insertNewBlock = true);
	void historyAdd(QString);
	void historyBack();
	void historyForward();
signals:
	void onCommand(QString);
	void onChange(QString);
};

