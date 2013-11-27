/***************************************************************************
													qconsole.cpp  -  description
														 -------------------
		begin                : mar mar 15 2005
		copyright            : (C) 2005 by Houssem BDIOUI
		email                : houssem.bdioui@gmail.com
 ***************************************************************************/

// migrated to Qt4 by YoungTaek Oh. date: Nov 29, 2010

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "qconsole.h"
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QVBoxLayout>
#include <QApplication>
#include <QScrollBar>
#include <QDesktopWidget>

//#define USE_POPUP_COMPLETER
#define WRITE_ONLY QIODevice::WriteOnly

QSize PopupListWidget::sizeHint() const
{
		QAbstractItemModel *model = this->model();
		QAbstractItemDelegate *delegate = this->itemDelegate();
		const QStyleOptionViewItem sovi;
		int left, top, right, bottom = 0;

		QMargins margin = this->contentsMargins();

		top = margin.top();
		bottom = margin.bottom();
		left = margin.left();
		right = margin.right();

		const int vOffset = top + bottom;
		const int hOffset = left + right;



		bool vScrollOn = false;
		int height = 0;
		int width = 0;
		for (int i=0; i<this->count(); ++i) {
				QModelIndex index = model->index(i, 0);
				QSize itemSizeHint = delegate->sizeHint(sovi, index);
				if (itemSizeHint.width() > width)
						width = itemSizeHint.width();

				// height
				const int nextHeight = height + itemSizeHint.height();
				if (nextHeight + vOffset < this->maximumHeight())
						height = nextHeight;
				else {
						// early termination
						vScrollOn = true;
						break;
				}
		}

		QSize sizeHint(width + hOffset, 0);
		sizeHint.setHeight(height + vOffset);
		if (vScrollOn) {
				int scrollWidth = this->verticalScrollBar()->sizeHint().width();
				sizeHint.setWidth(sizeHint.width() + scrollWidth);
		}
		return sizeHint;
}

PopupCompleter::PopupCompleter(const QStringList& sl, QWidget *parent)
		: QDialog(parent, Qt::Popup)
{
		setModal(true);

		listWidget_ = new PopupListWidget();
		listWidget_->setMaximumHeight(200);
		qDebug() << "sizeHint(): " << listWidget_->sizeHint();
		Q_FOREACH(QString str, sl) {
				QListWidgetItem *item = new QListWidgetItem;
				item->setText(str);
				listWidget_->addItem(item);
		}
		qDebug() << "sizeHint(): " << listWidget_->sizeHint();
		listWidget_->setFixedSize(listWidget_->sizeHint());


		QLayout *layout = new QVBoxLayout();
		layout->setSizeConstraint(QLayout::SetFixedSize);
		layout->setContentsMargins(0, 0, 0, 0);
		layout->addWidget(listWidget_);

		setLayout(layout);

		// connect signal
		connect(listWidget_, SIGNAL(itemActivated(QListWidgetItem *)),
						SLOT(onItemActivated(QListWidgetItem*)));
}

PopupCompleter::~PopupCompleter()
{
}

void PopupCompleter::showEvent(QShowEvent *event)
{
		listWidget_->setFocus();
}

void PopupCompleter::onItemActivated(QListWidgetItem *event)
{
		selected_ = event->text();
		done(QDialog::Accepted);
}

/**
 * @brief execute PopupCompleter at appropriate position.
 *
 * @param parent Parent of this popup completer. usually QConsole.
 * @return see QDialog::exec
 * @see QDialog::exec
 */
int PopupCompleter::exec(QTextEdit *parent)
{
		QSize popupSizeHint = this->sizeHint();
		QRect cursorRect = parent->cursorRect();
		QPoint globalPt = parent->mapToGlobal(cursorRect.bottomRight());
		QDesktopWidget *dsk = QApplication::desktop();
		QRect screenGeom = dsk->screenGeometry(dsk->screenNumber(this));
		if (globalPt.y() + popupSizeHint.height() > screenGeom.height()) {
				globalPt = parent->mapToGlobal(cursorRect.topRight());
				globalPt.setY(globalPt.y() - popupSizeHint.height());
		}
		this->move(globalPt);
		this->setFocus();
		return QDialog::exec();
}

/**
 * returns a common word of the given list
 *
 * @param list String list
 *
 * @return common word in the given string.
 */
static
QString getCommonWord(QStringList& list)
{
		QChar ch;
		QVector<QString> strarray = list.toVector();
		QString common;
		int col = 0,  min_len;
		bool cont = true;

		// get minimum length
		min_len = strarray.at(0).size();
		for (int i=1; i<strarray.size(); ++i) {
				const int len = strarray.at(i).size();
				if (len < min_len)
						min_len = len;
		}

		while(col < min_len) {
				ch = strarray.at(0)[col];
				for (int i=1; i<strarray.size(); ++i) {
						const QString& current_string = strarray.at(i);
						if (ch != current_string[col])
						{
								cont = false;
								break;
						}
				}
				if (!cont)
						break;

				common.push_back(ch);
				++col;
		}
		return common;
}

////////////////////////////////////////////////////////////////////////////////

//Clear the console
void QConsole::clear()
{
	 QTextEdit::clear();
}

//Reset the console
void QConsole::reset(const QString &welcomeText)
{
		clear();

		append(welcomeText);
		append("");

		//init attributes
		historyIndex = 0;
		history.clear();
		recordedScript.clear();
}

//QConsole constructor (init the QTextEdit & the attributes)
QConsole::QConsole(QWidget *parent, const QString &welcomeText)
		: QTextEdit(parent), errColor_(Qt::red),
		outColor_(Qt::white), completionColor(Qt::darkGreen),
		promptLength(0), promptParagraph(0)
{

	//Disable undo/redo
	setUndoRedoEnabled(false);


	//Disable context menu
	//This menu is useful for undo/redo, cut/copy/paste, del, select all,
	// see function QConsole::contextMenuEvent(...)
	//setContextMenuPolicy(Qt::NoContextMenu);


	//resets the console
	reset(welcomeText);
		const int tabwidth = QFontMetrics(currentFont()).width('a') * 4;
		setTabStopWidth(tabwidth);
}

//Sets the prompt and cache the prompt length to optimize the processing speed
void QConsole::setPrompt(const QString &newPrompt, bool display)
{
		prompt = newPrompt;
		promptLength = prompt.length();
		//display the new prompt
		if (display)
			displayPrompt();
}


//Displays the prompt and move the cursor to the end of the line.
void QConsole::displayPrompt()
{
		//Prevent previous text displayed to be undone
		setUndoRedoEnabled(false);
		//displays the prompt
		setTextColor(cmdColor_);
		QTextCursor cur = textCursor();
		cur.insertText(prompt);
		cur.movePosition(QTextCursor::EndOfLine);
		setTextCursor(cur);
		//Saves the paragraph number of the prompt
		promptParagraph = cur.blockNumber();

		//Enable undo/redo for the actual command
		setUndoRedoEnabled(true);
}

void QConsole::setFont(const QFont& f) {
		QTextCharFormat format;
		QTextCursor oldCursor = textCursor();
		format.setFont(f);
		selectAll();
		textCursor().setBlockCharFormat(format);
		setCurrentFont(f);
		setTextCursor(oldCursor);
}



//Give suggestions to autocomplete a command (should be reimplemented)
//the return value of the function is the string list of all suggestions
QStringList QConsole::suggestCommand(const QString&, QString& prefix)
{
		prefix = "";
		return QStringList();
}

//Treat the tab key & autocomplete the current command
void QConsole::handleTabKeyPress()
{
		QString command = getCurrentCommand();
		QString commandPrefix;
		QStringList sl = suggestCommand(command, commandPrefix);
		if (sl.count() == 0)
			textCursor().insertText("\t");
		else {
			if (sl.count() == 1)
				replaceCurrentCommand(commandPrefix + sl[0]);
			else
			{
				// common word completion
				QString commonWord = getCommonWord(sl);
				command = commonWord;

				PopupCompleter *popup = new PopupCompleter(sl);
				if (popup->exec(this) == QDialog::Accepted)
						replaceCurrentCommand(commandPrefix + popup->selected());
				delete popup;

			}
		}
}

// If return pressed, do the evaluation and append the result
void QConsole::handleReturnKeyPress()
{
		//Get the command to validate
	QString command = getCurrentCommand();
	//execute the command and get back its text result and its return value
	if (isCommandComplete(command))
		pExecCommand(command);
	else
	{
		append("");
		moveCursor(QTextCursor::EndOfLine);
	}
}

bool QConsole::handleBackspaceKeyPress()
{
		QTextCursor cur = textCursor();
		const int col = cur.columnNumber();
		const int blk = cur.blockNumber();
		if (blk == promptParagraph && col == promptLength)
			return true;
		return false;
}

void QConsole::handleUpKeyPress()
{
		if (history.count())
		{
				QString command = getCurrentCommand();
				do
				{
						if (historyIndex)
								historyIndex--;
						else
						{
								break;
						}
				} while(history[historyIndex] == command);
				replaceCurrentCommand(history[historyIndex]);
		}
}

void QConsole::handleDownKeyPress()
{
		if (history.count())
		{
				QString command = getCurrentCommand();
				do
				{
						if (++historyIndex >= history.size())
						{
								historyIndex = history.size() - 1;
								break;
						}
				} while(history[historyIndex] == command);
				replaceCurrentCommand(history[historyIndex]);
		}
}


void QConsole::setHome(bool select)
{
		QTextCursor cursor = textCursor();
		cursor.movePosition(QTextCursor::StartOfBlock, select ? QTextCursor::KeepAnchor :
																														QTextCursor::MoveAnchor);
		if(textCursor().blockNumber() == promptParagraph)
		{
			cursor.movePosition(QTextCursor::Right, select ? QTextCursor::KeepAnchor :
																											 QTextCursor::MoveAnchor,
													promptLength);
		}
		setTextCursor(cursor);
}

//Reimplemented key press event
void QConsole::keyPressEvent( QKeyEvent *e )
{
	//If the user wants to copy or cut outside
	//the editing area we perform a copy
	if(textCursor().hasSelection())
	{
		if(e->modifiers() == Qt::CTRL)
		{
			if( e->matches(QKeySequence::Cut) )
			{
				e->accept();
				if(!isInEditionZone())
				{
						copy();
				}
				else
				{
						cut();
				}
				return;
			}
			else if(e->matches(QKeySequence::Copy) )
			{
				e->accept();
				copy();
			}
			else
			{
				QTextEdit::keyPressEvent( e );
				return;
			}
		}
	}
/*
	// if the cursor out of editing zone put it back first
	if(!isInEditionZone())
	{
		 QTextCursor editCursor = textCursor();
		 editCursor.setPosition(oldEditPosition);
		 setTextCursor(editCursor);
	}
*/
	// control is pressed
	if ( (e->modifiers() & Qt::ControlModifier) && (e->key() == Qt::Key_C) )
	{
		if ( isSelectionInEditionZone())
		{
			//If Ctrl + C pressed, then undo the current commant
			//append("");
			//displayPrompt();

			//(Thierry Belair:)I humbly suggest that ctrl+C copies the text, as is expected,
			//and indicated in the contextual menu
			copy();
			return;
		}

	}
	else {
		switch (e->key()) {
		case Qt::Key_Tab:
			if(isSelectionInEditionZone())
			{
				handleTabKeyPress();
			}
			return;

		case Qt::Key_Enter:
		case Qt::Key_Return:
			if (isSelectionInEditionZone())
			{
					handleReturnKeyPress();
			}
			// ignore return key
			return;

		case Qt::Key_Backspace:
			if (handleBackspaceKeyPress() || !isSelectionInEditionZone())
				return;
			break;

		case Qt::Key_Home:
			setHome(e->modifiers() & Qt::ShiftModifier);
		case Qt::Key_Down:
			if (isInEditionZone())
			{
					handleDownKeyPress();
			}
			return;
		case Qt::Key_Up:
			if (isInEditionZone())
			{
					 handleUpKeyPress();
			}
			return;

		//Default behaviour
		case Qt::Key_End:
		case Qt::Key_Left:
		case Qt::Key_Right:
			break;

		default:
			if (textCursor().hasSelection() ) {
				if (!isSelectionInEditionZone())
				{
					moveCursor(QTextCursor::End, QTextCursor::MoveAnchor);
				}
				break;
			}
			else 
			{ //no selection
				//when typing normal characters,
				//make sure the cursor is positionned in the
				//edition zone
				if ( !isInEditionZone() )
				{
					moveCursor(QTextCursor::End, QTextCursor::MoveAnchor);
				}
			}
		} //end of switch
	} //end of else : no control pressed

	QTextEdit::keyPressEvent( e );
}

//Get the current command
QString QConsole::getCurrentCommand()
{
		QTextCursor cursor = textCursor();    //Get the current command: we just remove the prompt
		cursor.movePosition(QTextCursor::StartOfLine);
		cursor.movePosition(QTextCursor::Right, QTextCursor::MoveAnchor, promptLength);
		cursor.movePosition(QTextCursor::EndOfLine, QTextCursor::KeepAnchor);
		QString command = cursor.selectedText();
		cursor.clearSelection();
		return command;
}

//Replace current command with a new one
void QConsole::replaceCurrentCommand(const QString &newCommand)
{
		QTextCursor cursor = textCursor();
		cursor.movePosition(QTextCursor::StartOfLine);
		cursor.movePosition(QTextCursor::Right, QTextCursor::MoveAnchor, promptLength);
		cursor.movePosition(QTextCursor::EndOfLine, QTextCursor::KeepAnchor);
		cursor.insertText(newCommand);
}

//default implementation: command always complete
bool QConsole::isCommandComplete(const QString &)
{
		return true;
}

//Tests whether the cursor is in th edition zone or not (after the prompt
//or in the next lines (in case of multi-line mode)
bool QConsole::isInEditionZone()
{
		const int para = textCursor().blockNumber();
		const int index = textCursor().columnNumber();
		return (para > promptParagraph) || ( (para == promptParagraph) && (index >= promptLength) );
}


//Tests whether position (in parameter) is in the edition zone or not (after the prompt
//or in the next lines (in case of multi-line mode)
bool QConsole::isInEditionZone(const int& pos)
{
		QTextCursor cur = textCursor();
		cur.setPosition(pos);
		const int para = cur.blockNumber();
		const int index = cur.columnNumber();
		return (para > promptParagraph) || ( (para == promptParagraph) && (index >= promptLength) );
}


//Tests whether the current selection is in th edition zone or not
bool QConsole::isSelectionInEditionZone()
{
		QTextCursor cursor(document());
		int range[2];

		range[0] = textCursor().selectionStart();
		range[1] = textCursor().selectionEnd();
		for (int i = 0; i < 2; i++)
		{
				cursor.setPosition(range[i]);
				int para = cursor.blockNumber();
				int index = cursor.columnNumber();
				if ((para <= promptParagraph) && ( (para != promptParagraph) || (index < promptLength) ))
				{
						return false;
				}
		}
		return true;
}


//Basically, puts the command into the history list
//And emits a signal (should be called by reimplementations)
QString QConsole::addCommandToHistory(const QString &command)
{
	//Add the command to the recordedScript list
	recordedScript.append(command);
	//update the history and its index
	QString modifiedCommand = command;
	modifiedCommand.replace("\n", "\\n");
	history.append(modifiedCommand);
	historyIndex = history.size();
	//emit the commandExecuted signal
	Q_EMIT commandAddedToHistory(modifiedCommand);
	return "";
}

//pExecCommand(QString) executes the command and displays back its result
void QConsole::pExecCommand(const QString &command)
{
	addCommandToHistory(command);		

	emit execCommand(command); 
}

void QConsole::printCommandExecutionResults(const QString &result, ResultType type)
{
	//According to the return value, display the result either in red or in blue
	if (type == ResultType::Error)
		setTextColor(errColor_);
	else
		setTextColor(outColor_);	

	append(result);

	if (!result.endsWith("\n"))
		append("\n");

	moveCursor(QTextCursor::End);

	//Display the prompt again
	if (type == ResultType::Complete)
		displayPrompt();
}	



//Change paste behaviour
void QConsole::insertFromMimeData(const QMimeData *source)
{
		if (isSelectionInEditionZone())
		{
				QTextEdit::insertFromMimeData(source);
		}
}

//Implement paste with middle mouse button
void QConsole::mousePressEvent(QMouseEvent* event)
{
	oldPosition = textCursor().position();
		if (event->button() == Qt::MidButton)
		{
				copy();
				QTextCursor cursor = cursorForPosition(event->pos());
				setTextCursor(cursor);
				paste();
				return;
		}
		
		QTextEdit::mousePressEvent(event);
}


//Redefinition of the dropEvent to have a copy paste
//instead of a cut paste when copying out of the
//editable zone
void QConsole::dropEvent ( QDropEvent * event)
{
		if(!isInEditionZone())
		{
				//Execute un drop a drop at the old position
				//if the drag started out of the editable zone
				QTextCursor cur = textCursor();
				cur.setPosition(oldPosition);
				setTextCursor(cur);
		}
		//Execute a normal drop
		QTextEdit::dropEvent(event);
}



void QConsole::dragMoveEvent( QDragMoveEvent * event)
{
		//Get a cursor for the actual mouse position
		QTextCursor cur = textCursor();
		cur.setPosition(cursorForPosition(event->pos()).position());

		if(!isInEditionZone(cursorForPosition(event->pos()).position()))
		{
				//Ignore the event if out of the editable zone
				event->ignore(cursorRect(cur));
		}
		else
		{
				//Accept the event if out of the editable zone
				event->accept(cursorRect(cur));
		}
}


void QConsole::contextMenuEvent ( QContextMenuEvent * event)
{
		QMenu *menu = new QMenu(this);

		QAction *undo = new QAction(tr("Undo"), this);
		undo->setShortcut(tr("Ctrl+Z"));
		QAction *redo = new QAction(tr("Redo"), this);
		redo->setShortcut(tr("Ctrl+Y"));
		QAction *cut = new QAction(tr("Cut"), this);
		cut->setShortcut(tr("Ctrl+X"));
		QAction *copy = new QAction(tr("Copy"), this);
		copy->setShortcut(tr("Ctrl+Ins"));
		QAction *paste = new QAction(tr("Paste"), this);
		paste->setShortcut(tr("Ctrl+V"));
		QAction *del = new QAction(tr("Delete"), this);
		del->setShortcut(tr("Del"));
		QAction *selectAll = new QAction(tr("Select All"), this);
		selectAll->setShortcut(tr("Ctrl+A"));

		menu->addAction(undo);
		menu->addAction(redo);
		menu->addSeparator();
		menu->addAction(cut);
		menu->addAction(copy);
		menu->addAction(paste);
		menu->addAction(del);
		menu->addSeparator();
		menu->addAction(selectAll);

		connect(undo, SIGNAL(triggered()), this, SLOT(undo()));
		connect(redo, SIGNAL(triggered()), this, SLOT(redo()));
		connect(cut, SIGNAL(triggered()), this, SLOT(cut()));
		connect(copy, SIGNAL(triggered()), this, SLOT(copy()));
		connect(paste, SIGNAL(triggered()), this, SLOT(paste()));
		connect(del, SIGNAL(triggered()), this, SLOT(del()));
		connect(selectAll, SIGNAL(triggered()), this, SLOT(selectAll()));


		menu->exec(event->globalPos());

		delete menu;
}



void QConsole::cut()
{
		//Cut only in the editing zone,
		//perfom a copy otherwise
		if(isInEditionZone())
		{
				QTextEdit::cut();
				return;
		}

		QTextEdit::copy();
}


/*
//Allows pasting with middle mouse button (x window)
//when clicking outside of the edition zone
void QConsole::paste()
{
		restoreOldPosition();
		QTextEdit::paste();
}
*/

void QConsole::del()
{
		//Delete only in the editing zone
		if(isInEditionZone())
		{
				textCursor().movePosition(QTextCursor::EndOfWord, QTextCursor::KeepAnchor);
				textCursor().deleteChar();
		}
}


void QConsole::correctPathName(QString& pathName)
{
		if(pathName.contains(tr(":\\")))
		{
				pathName.replace('\\', tr("/"));
		}
}
