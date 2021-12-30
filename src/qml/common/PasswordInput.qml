import QtQuick 2.3
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.3

RowLayout {
    id: root

    property alias placeholderText: textField.placeholderText
    property alias text: textField.text
    property alias validationError: textField.validationError
    property alias checkboxWidth: passwordMask.width

    signal accepted

    function forceFocus() {
        textField.forceActiveFocus()
    }

    BetterTextField {
        id: textField
        Layout.fillWidth: true
        echoMode: passwordMask.checked ? TextInput.Normal : TextInput.Password

        onAccepted: root.accepted()
    }

    BetterCheckbox {
        id: passwordMask
        text: qsTranslate("RESP","Show password")
    }
}
