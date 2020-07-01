import QtQuick.Controls 2.3

GroupBox {
    id: root
    property string labelText
    property bool checked: false

    palette.windowText: sysPalette.windowText
    palette.mid: sysPalette.mid

    label: BetterCheckbox {
            id: checkBox
            objectName: "checkbox"
            checked: root.checked
            text: root.labelText           
    }
}
