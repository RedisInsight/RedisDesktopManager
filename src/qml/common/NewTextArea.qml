import QtQuick 2.7
import "./platformutils.js" as PlatformUtils
import "."

TextEdit {
    id: root
    color: sysPalette.text
    wrapMode: TextEdit.WrapAnywhere
    font {
        family: appSettings.valueEditorFont
        pointSize: appSettings.valueEditorFontSize
    }
    selectByMouse: true

    property bool highlightJSON: false

    Loader {
        source: root.highlightJSON? "./JsonHighlighter.qml" : ""
        onLoaded: {
            if (item) {
                item.textDocument = root.textDocument
            }
        }
    }
}
