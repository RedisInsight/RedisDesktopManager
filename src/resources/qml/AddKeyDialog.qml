import QtQuick 2.3
import QtQuick.Layouts 1.1
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.1
import QtQuick.Dialogs 1.2
import "./editors/editor.js" as Editor

Item {
    function open() {
        root.open()
    }

    Dialog {
        id: root
        title: "Add New Key"

        width: 520
        height: 450
        modality: Qt.ApplicationModal

        ColumnLayout {
            implicitWidth: 500
            implicitHeight: 100
            width: 500

            Text { text: "Key:" }
            TextField {
                id: newKeyName
                Layout.fillWidth: true
            }

            Text { text: "Type:" }
            ComboBox {
                id: typeSelector
                model: Editor.getSupportedKeyTypes()
                Layout.fillWidth: true
            }

            Text { text: "Value:" }
            Loader {
                id: valueAddEditor
                Layout.fillWidth: true
                Layout.preferredHeight: 300

                source: Editor.getEditorByTypeString(typeSelector.model[typeSelector.currentIndex])

                onLoaded: {
                    item.state = "new"
                }
            }
        }

        onAccepted: {
            if (!valueAddEditor.item)
                return false

            if (!valueAddEditor.item.isValueValid()
                    || newKeyName.text.length == 0) {
                valueAddEditor.item.markInvalidFields()
                return open()
            }

            var row = valueAddEditor.item.getValue()
            viewModel.addKey(newKeyName.text,
                             typeSelector.model[typeSelector.currentIndex],
                             row)

            addConfirmation.text = "Key was added!"
            addConfirmation.open()
        }

        visible: false
        standardButtons: StandardButton.Save | StandardButton.Cancel

    }
    MessageDialog {
        id: addConfirmation
        title: "Save value"
        text: ""
        visible: false
        modality: Qt.ApplicationModal
        icon: StandardIcon.Information
        standardButtons: StandardButton.Ok
    }
}
