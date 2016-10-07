import QtQuick 2.3
import QtQuick.Layouts 1.1
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.1
import QtQuick.Dialogs 1.2
import QtQuick.Window 2.2
import "./editors/editor.js" as Editor

Dialog {
    id: root
    title: "Add New Key"
    width: 550
    height: 500
    modality: Qt.ApplicationModal
    visible: false

    standardButtons: StandardButton.NoButton

    Item {
        anchors.fill: parent

        ColumnLayout {
            anchors.fill: parent
            anchors.margins: 5

            Text { text: qsTr("Key:") }
            TextField {
                id: newKeyName
                Layout.fillWidth: true
            }

            Text { text: qsTr("Type:") }
            ComboBox {
                id: typeSelector
                model: Editor.getSupportedKeyTypes()
                Layout.fillWidth: true
            }

            Text { text: qsTr("Value:") }
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
                    text: qsTr("Save")

                    onClicked: {
                        if (!valueAddEditor.item)
                            return

                        if (!valueAddEditor.item.isValueValid()
                                || newKeyName.text.length == 0) {
                            valueAddEditor.item.markInvalidFields()
                            return
                        }

                        var row = valueAddEditor.item.getValue()
                        viewModel.addKey(
                                    newKeyName.text,
                                    typeSelector.model[typeSelector.currentIndex],
                                    row,
                                    function callback(err) {
                                        if (!err) {
                                            newKeyName.text = ''
                                            valueAddEditor.item.reset()
                                            root.close()
                                        } else {
                                            addError.text = err
                                            addError.open()
                                        }
                                    }
                                    )
                    }
                }

                Button {
                    text: qsTr("Cancel")
                    onClicked: root.close()
                }
            }
            Item { Layout.fillWidth: true}
        }
    }

    MessageDialog {
        id: addError
        title: qsTr("Error")
        text: ""
        visible: false
        modality: Qt.ApplicationModal
        icon: StandardIcon.Warning
        standardButtons: StandardButton.Ok
    }
}

