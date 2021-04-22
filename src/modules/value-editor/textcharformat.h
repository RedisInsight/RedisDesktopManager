#pragma once
#include <QObject>
#include <QTextCharFormat>

/***
 * Based on https://github.com/stephenquan/QtSyntaxHighlighterApp
 *
 * Original code is licensed under the Apache License, Version 2.0
 * (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

   http://www.apache.org/licenses/LICENSE-2.0
 * */

class TextCharFormat : public QObject, public QTextCharFormat
{
    Q_OBJECT
    Q_PROPERTY (QFont font READ font WRITE setFont NOTIFY fontChanged)
    Q_PROPERTY (QVariant foreground READ foreground WRITE setForeground NOTIFY foregroundChanged)

public:
    TextCharFormat(QObject* parent = nullptr);

signals:
    void fontChanged();
    void foregroundChanged();

protected:
    void setFont(const QFont& font);
    QFont font() const;

    QVariant foreground() const;
    void setForeground(const QVariant& foreground);
};
