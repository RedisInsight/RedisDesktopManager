import QtQuick.Controls 2.3

GroupBox {
    id: root
    property string labelText
    property alias checked: checkBox.checked

    palette.windowText: sysPalette.windowText
    palette.mid: sysPalette.mid

    spacing: 1
    padding: 1

    label: BetterCheckbox {
            id: checkBox
            objectName: "checkbox"
            text: root.labelText
    }
}
