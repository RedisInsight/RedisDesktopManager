import QtQuick 2.7
import "./platformutils.js" as PlatformUtils

TextEdit {
    color: sysPalette.text
    wrapMode: TextEdit.WrapAnywhere
    font {
        family: appSettings.valueEditorFont
        pointSize: appSettings.valueEditorFontSize
    }
    selectByMouse: true
}
