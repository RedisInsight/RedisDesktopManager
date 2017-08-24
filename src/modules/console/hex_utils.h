#pragma once

#include <QVariant>
#include <QByteArray>
#include <QString>

QString binaryStringToEscapedString(const QByteArray&);

QByteArray escapedStringToBinaryString(const QString&);

