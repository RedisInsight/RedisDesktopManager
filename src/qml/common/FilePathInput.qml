import QtQuick 2.3
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.3
import QtQuick.Dialogs 1.2
import "./platformutils.js" as PlatformUtils


RowLayout {
    id: root

    property alias placeholderText: textField.placeholderText
    property alias path: textField.text
    property alias nameFilters: fileDialog.nameFilters
    property alias title: fileDialog.title
    property alias validationError: textField.validationError

    BetterTextField {
        id: textField
        objectName: root.objectName? root.objectName + "_text" : ""
        readOnly: PlatformUtils.isOSX()
        Layout.fillWidth: true
    }

    BetterButton {
        implicitHeight: 30
        objectName: root.objectName? root.objectName + "_button" : ""
        text: qsTranslate("RDM","Select File")
        onClicked: fileDialog.open()
    }

    FileDialog {
        id: fileDialog
        selectExisting: true
        onAccepted: textField.text = qmlUtils.getPathFromUrl(fileDialog.fileUrl)
    }
}
