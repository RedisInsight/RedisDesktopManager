#pragma once
#include <QObject>
#include <QTextDocument>
#include <QSyntaxHighlighter>
#include <QQuickTextDocument>

/***
 * Based on https://github.com/stephenquan/QtSyntaxHighlighterApp
 *
 * Original code is licensed under the Apache License, Version 2.0
 * (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

   http://www.apache.org/licenses/LICENSE-2.0
 * */

class SyntaxHighlighter : public QSyntaxHighlighter
{
    Q_OBJECT
    Q_PROPERTY(QQuickTextDocument* textDocument READ textDocument WRITE setTextDocument NOTIFY textDocumentChanged)

public:
    SyntaxHighlighter(QObject* parent = nullptr);

    Q_INVOKABLE void setFormat(int start, int count, const QVariant& format);

signals:
    void textDocumentChanged();
    void highlightBlock(const QVariant& text);

protected:
    QQuickTextDocument* m_TextDocument;

    QQuickTextDocument* textDocument() const;
    void setTextDocument(QQuickTextDocument* textDocument);

    virtual void highlightBlock(const QString &text);
};
