import QtQuick 2.0
import rdm.models 1.0

Item {
    property alias textDocument: syntaxHighlighter.textDocument

    property bool _darkPalette: sysPalette.base.hslLightness < 0.4

    TextCharFormat {
        id: keyFormat;
        foreground: _darkPalette? '#fcfcfc': '#000000'
        font.family: appSettings.valueEditorFont
        font.pointSize: appSettings.valueEditorFontSize
    }

    TextCharFormat {
        id: numberFormat;
        foreground: _darkPalette? '#008cff': '#0000ff'
        font.family: appSettings.valueEditorFont
        font.pointSize: appSettings.valueEditorFontSize
    }

    TextCharFormat {
        id: boolFormat;
        foreground: _darkPalette? '#d62929': '#b22222'
        font.family: appSettings.valueEditorFont
        font.pointSize: appSettings.valueEditorFontSize
    }

    TextCharFormat {
        id: stringFormat;
        foreground: _darkPalette? '#05a605': '#008000'
        font.family: appSettings.valueEditorFont
        font.pointSize: appSettings.valueEditorFontSize
    }

    TextCharFormat {
        id: nullFormat;
        foreground: _darkPalette? '#a8a8a8' : '#808080'
        font.family: appSettings.valueEditorFont
        font.pointSize: appSettings.valueEditorFontSize
    }

    SyntaxHighlighter {
        id: syntaxHighlighter

        onHighlightBlock: {
            let rx = /("(\\u[a-zA-Z0-9]{4}|\\[^u]|[^\\"])*"(\s*:)?|\b(true|false|null)\b|-?\d+(?:\.\d*)?(?:[eE][+\-]?\d+)?)/g
            let m
            while ( ( m = rx.exec(text) ) !== null ) {
                var type = 'number';
                if (/^"/.test(m[0])) {
                    if (/:$/.test(m[0])) {
                        setFormat(m.index, m[0].length, keyFormat);
                        continue;
                    } else {
                        setFormat(m.index, m[0].length, stringFormat);
                        continue;
                    }
                } else if (/true|false/.test(m[0])) {
                    setFormat(m.index, m[0].length, boolFormat);
                    continue;
                } else if (/null/.test(m[0])) {
                    setFormat(m.index, m[0].length, nullFormat);
                    continue;
                }

                setFormat(m.index, m[0].length, numberFormat);
                continue;
            }
        }

    }
}
