#include "syntaxhighlighter.h"
#include "textcharformat.h"


SyntaxHighlighter::SyntaxHighlighter( QObject* parent ) :
    QSyntaxHighlighter(parent),
    m_TextDocument(nullptr)
{
}

void SyntaxHighlighter::highlightBlock( const QString &text )
{
    emit highlightBlock( QVariant(text) );
}

QQuickTextDocument* SyntaxHighlighter::textDocument() const
{
    return m_TextDocument;
}

void SyntaxHighlighter::setTextDocument( QQuickTextDocument* textDocument )
{
    if (textDocument == m_TextDocument)
    {
        return;
    }

    m_TextDocument = textDocument;

    QTextDocument* doc = m_TextDocument->textDocument();
    setDocument(doc);

    emit textDocumentChanged();
}

void SyntaxHighlighter::setFormat( int start, int count, const QVariant& format )
{
    TextCharFormat* charFormat = qvariant_cast<TextCharFormat*>( format );
    if ( charFormat )
    {
        QSyntaxHighlighter::setFormat( start, count, *charFormat );
        return;
    }

    if ( format.canConvert(QVariant::Color) )
    {
        QSyntaxHighlighter::setFormat( start, count, format.value<QColor>() );
        return;
    }

    if ( format.canConvert(QVariant::Font) )
    {
        QSyntaxHighlighter::setFormat( start, count, format.value<QFont>() );
        return;
    }
}
