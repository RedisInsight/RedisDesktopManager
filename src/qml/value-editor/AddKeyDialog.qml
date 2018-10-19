import QtQuick 2.3
import QtQuick.Layouts 1.1
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.1
import QtQuick.Dialogs 1.2
import QtQuick.Window 2.2
import "./editors/editor.js" as Editor

Dialog {
    id: root
    title: qsTranslate("RDM","Add New Key")
    visible: false

    standardButtons: StandardButton.NoButton

    Item {
        anchors.fill: parent
        implicitHeight: 500
        implicitWidth: 600

        ColumnLayout {
            anchors.fill: parent
            anchors.margins: 5

            Text {
                text: qsTranslate("RDM","Key:")
            }
            TextField {
                id: newKeyName
                Layout.fillWidth: true
                objectName: "rdm_add_key_name_field"
            }

            Text {
                text: qsTranslate("RDM","Type:")
            }
            ComboBox {
                id: typeSelector
                model: Editor.getSupportedKeyTypes()
                Layout.fillWidth: true
                objectName: "rdm_add_key_type_field"
            }

            Loader {
                id: valueAddEditor
                Layout.fillWidth: true
                Layout.fillHeight: true
                Layout.preferredHeight: 300

                source: Editor.getEditorByTypeString(
                            typeSelector.model[typeSelector.currentIndex])

                onLoaded: {
                    item.state = "new"
                    item.initEmpty()
                }
            }

            RowLayout {
                Layout.fillWidth: true
                Layout.minimumHeight: 40
                Item {
                    Layout.fillWidth: true
                }
                Button {
                    text: qsTranslate("RDM","Save")

                    onClicked: {
                        if (!valueAddEditor.item)
                            return

                        valueAddEditor.item.validateValue(function (result) {
                            if (!result)
                                return;

                            var row = valueAddEditor.item.getValue()
                            valuesModel.addKey(
                                newKeyName.text,
                                typeSelector.model[typeSelector.currentIndex],
                                row, afterSave
                            );
                        })
                    }

                    function afterSave(err) {
                        if (!err) {
                            newKeyName.text = ''
                            valueAddEditor.item.reset()
                            valueAddEditor.item.initEmpty()
                            root.close()
                        } else {
                            addError.text = err
                            addError.open()
                        }
                    }
                }

                Button {
                    text: qsTranslate("RDM","Cancel")
                    onClicked: root.close()
                }
            }
            Item {
                Layout.fillWidth: true
            }
        }

        MessageDialog {
            id: addError
            title: qsTranslate("RDM","Error")
            text: ""
            visible: false
            modality: Qt.ApplicationModal
            icon: StandardIcon.Warning
            standardButtons: StandardButton.Ok
        }
    }
}
