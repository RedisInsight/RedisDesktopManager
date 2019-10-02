import QtQuick 2.3
import QtQuick.Layouts 1.1
import QtQuick.Controls 1.2
import QtQuick.Dialogs 1.2
import "./platformutils.js" as PlatformUtils


RowLayout {
    id: root

    property alias placeholderText: textField.placeholderText
    property alias path: textField.text
    property alias nameFilters: fileDialog.nameFilters
    property alias title: fileDialog.title
    property alias style: textField.style

    TextField {
        id: textField
        readOnly: PlatformUtils.isOSX()
        Layout.fillWidth: true
    }

    Button {
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
