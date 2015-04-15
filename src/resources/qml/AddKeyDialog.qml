import QtQuick 2.3
import QtQuick.Layouts 1.1
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.1
import QtQuick.Dialogs 1.2
import QtQuick.Window 2.2
import "./editors/editor.js" as Editor

Item {
    function open() {
        root.visible = true
        root.show()
    }

    Window {
        id: root
        title: "Add New Key"

        width: 520
        height: 450
        minimumWidth: 520
        minimumHeight: 450
        modality: Qt.ApplicationModal
        flags: Qt.Dialog
        x: Screen.desktopAvailableWidth / 2.0 - 250
        y: Screen.desktopAvailableHeight / 2.0 - 200
        visible: false

        Item {
            anchors.fill: parent

            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 5

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

                RowLayout {
                    Layout.fillWidth: true
                    Layout.minimumHeight: 40
                    Item { Layout.fillWidth: true}
                    Button {
                        text: "Save"

                        onClicked: {
                            if (!valueAddEditor.item)
                                return

                            if (!valueAddEditor.item.isValueValid()
                                    || newKeyName.text.length == 0) {
                                valueAddEditor.item.markInvalidFields()
                                return
                            }

                            var row = valueAddEditor.item.getValue()
                            viewModel.addKey(newKeyName.text,
                                             typeSelector.model[typeSelector.currentIndex],
                                             row)

                            newKeyName.text = ''
                            valueAddEditor.item.reset()
                            root.close()
                            addConfirmation.text = "Key was added!"
                            addConfirmation.open()
                        }
                    }

                    Button {
                        text: "Cancel"
                        onClicked: root.close()
                    }
                }
                Item { Layout.fillWidth: true}
            }
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
}
