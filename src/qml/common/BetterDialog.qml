import QtQuick 2.13
import QtQuick.Controls 2.13

Dialog {
    id: root

    x: (approot.width - width) / 2
    y: (approot.height - height) / 3
    parent: Overlay.overlay

    modal: true

    onRejected: {
        root.close()
    }

    footer: DialogButtonBox {
      standardButtons: Dialog.Save | Dialog.Cancel
    }
}
