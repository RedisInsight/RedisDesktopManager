import QtQuick 2.3
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.3

RowLayout {
    id: root

    property alias placeholderText: textField.placeholderText
    property alias text: textField.text
    property alias validationError: textField.validationError

    BetterTextField {
        id: textField
        Layout.fillWidth: true
        echoMode: passwordMask.checked ? TextInput.Normal : TextInput.Password
    }

    BetterCheckbox {
        id: passwordMask
        text: qsTranslate("RDM","Show password")
    }
}
