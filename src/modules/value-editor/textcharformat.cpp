#include "textcharformat.h"

TextCharFormat::TextCharFormat(QObject* parent) :
    QObject(parent)
{
}

void TextCharFormat::setFont( const QFont& font )
{
    if ( font == QTextCharFormat::font() )
    {
        return;
    }

    QTextCharFormat::setFont(font);
    emit fontChanged();
}

QFont TextCharFormat::font() const
{
    return QTextCharFormat::font();
}

QVariant TextCharFormat::foreground() const
{
    return QTextCharFormat::foreground().color();
}

void TextCharFormat::setForeground( const QVariant& foreground )
{
    if ( foreground.canConvert<QColor>() )
    {
        QTextCharFormat::setForeground( QBrush( foreground.value< QColor >() ) );
        emit foregroundChanged();
    }
}
