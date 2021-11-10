import QtQuick 2.13
import QtQuick.Controls 2.13
import "."

BetterDialog {
    id: root

    header: BetterLabel {
        text: root.title
        visible: root.title
        elide: Label.ElideRight
        font.bold: true
        padding: 12
        background: Rectangle {
            x: 1; y: 1
            width: parent.width - 2
            height: parent.height - 1
            color: sysPalette.window
        }
    }

    implicitWidth: 300

    property alias text: label.text

    BetterLabel {
        id: label
        anchors.fill: parent
        anchors.margins: 10
    }

    signal yesClicked

    footer: BetterDialogButtonBox {
        BetterButton {
            text: qsTranslate("RDM","Yes")
            DialogButtonBox.buttonRole: DialogButtonBox.AcceptRole
            onClicked: {
                root.yesClicked()
            }
        }

        BetterButton {
            text: qsTranslate("RDM","No")
            onClicked: root.close()
        }
    }
}
