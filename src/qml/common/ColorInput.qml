import QtQuick 2.3
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.3
import QtQuick.Dialogs 1.3
import "./platformutils.js" as PlatformUtils


RowLayout {
    id: root

    property alias placeholderText: textField.placeholderText
    property alias color: textField.text
    property alias title: dialog.title
    property alias validationError: textField.validationError

    function reset() {
        color = ""
        dialog.color = ""
    }

    Rectangle {
        implicitWidth: 30
        implicitHeight: 30
        color: textField.text? textField.text: "transparent"
        border.color:  sysPalette.highlight
        border.width: 1

        MouseArea {
            anchors.fill: parent
            onClicked: dialog.open()
        }
    }

    BetterTextField {
        id: textField
        objectName: root.objectName? root.objectName + "_text" : ""
        Layout.fillWidth: true
    }

    BetterButton {
        implicitHeight: 30
        objectName: root.objectName? root.objectName + "_button" : ""
        text: qsTranslate("RESP","Select")
        onClicked: dialog.open()
    }

    ColorDialog {
        id: dialog
        onAccepted: textField.text = dialog.color
    }
}
