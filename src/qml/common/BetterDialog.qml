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
        border.color: root.palette.dark
    }

    footer: BetterDialogButtonBox {
        standardButtons: Dialog.Save | Dialog.Cancel
    }
}
