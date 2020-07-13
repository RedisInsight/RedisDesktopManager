import QtQuick 2.7
import "./platformutils.js" as PlatformUtils

TextEdit {
    color: PlatformUtils.isOSX() ? sysPalette.text : "#3b3a3a"
    wrapMode: TextEdit.WrapAnywhere
    font {
        family: appSettings.valueEditorFont
        pointSize: appSettings.valueEditorFontSize
    }
    selectByMouse: true
}
