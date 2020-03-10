import QtQuick.Controls 2.3

GroupBox {
    id: root
    property string labelText
    property bool checked: false

    label: BetterCheckbox {
            id: checkBox
            objectName: "checkbox"
            checked: root.checked
            text: root.labelText           
    }
}
