import QtQuick 2.0
import QtQuick.Dialogs 1.3

MessageDialog {
    id: root

    standardButtons: StandardButton.Yes | StandardButton.No

    signal yesClicked

    onYes: {
        yesClicked()
    }
}
