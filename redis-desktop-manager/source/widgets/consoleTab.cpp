#include "consoleTab.h"

#include <QScrollBar>
#include <QTextBlock>

#include "RedisConnectionConfig.h"

consoleTab::consoleTab(RedisConnectionConfig& config)
{
	prompt += config.name + ">";

	QPalette p = palette();
	p.setColor(QPalette::Base, QColor(57, 57, 57));
	p.setColor(QPalette::Text, QColor(238, 238, 238));
	setPalette(p);

	history = new QStringList;
	historyPos = 0;
	insertPrompt(false);
	isLocked = false;
}


consoleTab::~consoleTab(void)
{
}

void consoleTab::keyPressEvent(QKeyEvent *event)
{
	if(isLocked)
		return;
	if(event->key() >= 0x20 && event->key() <= 0x7e
		&& (event->modifiers() == Qt::NoModifier || event->modifiers() == Qt::ShiftModifier))
		QPlainTextEdit::keyPressEvent(event);
	if(event->key() == Qt::Key_Backspace
		&& event->modifiers() == Qt::NoModifier
		&& textCursor().positionInBlock() > prompt.length())
		QPlainTextEdit::keyPressEvent(event);
	if(event->key() == Qt::Key_Return && event->modifiers() == Qt::NoModifier)
		onEnter();
	if(event->key() == Qt::Key_Up && event->modifiers() == Qt::NoModifier)
		historyBack();
	if(event->key() == Qt::Key_Down && event->modifiers() == Qt::NoModifier)
		historyForward();
	QString cmd = textCursor().block().text().mid(prompt.length());
	emit onChange(cmd);
}

void consoleTab::mousePressEvent(QMouseEvent *)
{
	setFocus();
}

void consoleTab::mouseDoubleClickEvent(QMouseEvent *){}

void consoleTab::contextMenuEvent(QContextMenuEvent *){}

void consoleTab::onEnter()
{
	if(textCursor().positionInBlock() == prompt.length())
	{
		insertPrompt();
		return;
	}
	QString cmd = textCursor().block().text().mid(prompt.length());
	isLocked = true;
	historyAdd(cmd);
	emit onCommand(cmd);
}

void consoleTab::output(QString s)
{
	textCursor().insertBlock();
	QTextCharFormat format;
	format.setForeground(Qt::white);
	textCursor().setBlockCharFormat(format);
	textCursor().insertText(s);
	insertPrompt();
	isLocked = false;
}

void consoleTab::insertPrompt(bool insertNewBlock)
{
	if(insertNewBlock)
		textCursor().insertBlock();
	QTextCharFormat format;
	format.setForeground(Qt::green);
	textCursor().setBlockCharFormat(format);
	textCursor().insertText(prompt);
	scrollDown();
}

void consoleTab::scrollDown()
{
	QScrollBar *vbar = verticalScrollBar();
	vbar->setValue(vbar->maximum());
}

void consoleTab::historyAdd(QString cmd)
{
	history->append(cmd);
	historyPos = history->length();
}

void consoleTab::historyBack()
{
	if(!historyPos)
		return;
	QTextCursor cursor = textCursor();
	cursor.movePosition(QTextCursor::StartOfBlock);
	cursor.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);
	cursor.removeSelectedText();
	cursor.insertText(prompt + history->at(historyPos-1));
	setTextCursor(cursor);
	historyPos--;
}

void consoleTab::historyForward()
{
	if(historyPos == history->length())
		return;
	QTextCursor cursor = textCursor();
	cursor.movePosition(QTextCursor::StartOfBlock);
	cursor.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);
	cursor.removeSelectedText();
	if(historyPos == history->length() - 1)
		cursor.insertText(prompt);
	else
		cursor.insertText(prompt + history->at(historyPos + 1));
	setTextCursor(cursor);
	historyPos++;
}