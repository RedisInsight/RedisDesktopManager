import QtQuick 2.13
import QtQuick.Controls 2.13
import "."

Dialog {
    id: root

    x: (approot.width - width) / 2
    y: (approot.height - height) / 3
    parent: Overlay.overlay

    modal: true

    onRejected: {
        root.close()
    }

    background: Rectangle {
        color: sysPalette.base
        border.color: sysPalette.mid
    }

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

    footer: BetterDialogButtonBox {
        BetterButton {
            text: qsTranslate("RDM","Save")
            DialogButtonBox.buttonRole: DialogButtonBox.AcceptRole
        }

        BetterButton {
            text: qsTranslate("RDM","Cancel")
            onClicked: root.close()
        }
    }
}
