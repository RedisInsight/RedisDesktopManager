import QtQuick 2.13
import QtQuick.Controls 2.13
import "."

BetterDialog {
    id: root

    implicitWidth: 300

    property alias text: label.text

    BetterLabel {
        id: label
        anchors.fill: parent
        anchors.margins: 10
    }

    footer: BetterDialogButtonBox {
        BetterButton {
            text: qsTranslate("RDM","OK")
            DialogButtonBox.buttonRole: DialogButtonBox.AcceptRole
        }
    }
}
