import QtQuick 2.3
import QtQuick.Layouts 1.1
import QtQuick.Controls 1.2

RowLayout {
    id: root

    property alias placeholderText: textField.placeholderText
    property alias text: textField.text
    property alias style: textField.style

    TextField {
        id: textField
        Layout.fillWidth: true
        echoMode: passwordMask.checked ? TextInput.Normal : TextInput.Password
    }

    BetterCheckbox {
        id: passwordMask
        text: qsTr("Show password")
    }
}
