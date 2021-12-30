import QtQuick 2.13
import QtQuick.Controls 2.13
import "."

BetterDialog {
    id: root

    implicitWidth: label.contentWidth + 50

    property alias text: label.text

    BetterLabel {
        id: label
        anchors.fill: parent
        anchors.margins: 10
    }

    signal yesClicked

    footer: BetterDialogButtonBox {
        spacing: 3

        BetterButton {
            text: qsTranslate("RESP","Yes")
            DialogButtonBox.buttonRole: DialogButtonBox.AcceptRole
            onClicked: {
                root.yesClicked()
            }            
        }

        BetterButton {
            text: qsTranslate("RESP","No")
            onClicked: root.close()
        }
    }
}
