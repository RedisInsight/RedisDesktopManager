#include "hex_utils.h"


QString binaryStringToEscapedString(const QByteArray &value)
{
    QString val = QString::fromStdString(value.toStdString());

    for(int index=0; index < val.length(); ++index) {

        QChar c(val.at(index));

        if (c.category() == QChar::Other_Control) {
            QString num = QString::number(c.toLatin1(), 16);

            QString replace = QString("\\x%1%2").arg(num.size() == 1 ? "0": "").arg(num);
            val.replace(index, 1, replace);
            index += replace.size();
        }
    }
    return val;
}


QByteArray escapedStringToBinaryString(const QString &value)
{
    QString result = value;
    QRegExp rx("\\" "\\" "x([0-9]{2,2})");
    int pos = 0;

    while ((pos = rx.indexIn(result, pos)) != -1) {
        QString found = rx.cap(1);
        QString after(QChar(found.toInt(nullptr, 16)));

        result.replace(pos, rx.matchedLength(), after);
        pos += 1;
    }

    return result.toUtf8();
}
