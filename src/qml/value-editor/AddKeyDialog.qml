import QtQuick 2.3
import QtQuick.Layouts 1.1
import QtQuick.Controls 2.12
import "./../common"
import "./editors/editor.js" as Editor
import "../common/platformutils.js" as PlatformUtils

BetterDialog {
    id: root
    title: qsTranslate("RESP","Add New Key to ") + (request? request.dbIdString: "")
    visible: false
    property var request
    property bool loadingKeyTypes: false
    property var supportedKeyTypes

    footer: null

    onRequestChanged: {
        if (!request)
            return;

        root.loadingKeyTypes = true;
        request.loadAdditionalKeyTypesInfo(processAdditionalKeyTypes);

        root.supportedKeyTypes = Editor.getSupportedKeyTypes();
    }


    function processAdditionalKeyTypes() {
        console.log(root.supportedKeyTypes)

        if (arguments && arguments.length > 0) {
            for (var indx in arguments) {
                console.log("module:", arguments[indx])
                root.supportedKeyTypes.push(arguments[indx])
            }
        }

        console.log(root.supportedKeyTypes)

        root.supportedKeyTypes = supportedKeyTypes;
        root.loadingKeyTypes = false;
    }


    Item {
        anchors.fill: parent
        implicitHeight: PlatformUtils.isOSX() ? 400 : 600
        implicitWidth: PlatformUtils.isOSX() ? 600 : 800

        ColumnLayout {
            anchors.fill: parent
            anchors.margins: 5

            BetterLabel {
                text: qsTranslate("RESP","Key:")
            }
            BetterTextField {
                id: newKeyName
                Layout.fillWidth: true
                objectName: "rdm_add_key_name_field"
                text: request? request.keyName : ''
            }

            BetterLabel {
                text: qsTranslate("RESP","Type:")
            }

            BetterComboBox {
                id: typeSelector
                model: root.supportedKeyTypes
                Layout.fillWidth: true
                objectName: "rdm_add_key_type_field"

                onCurrentIndexChanged: {
                    if (valueAddEditor.item.keyType !== undefined) {
                        valueAddEditor.item.keyType = typeSelector.model[typeSelector.currentIndex]                        
                    }
                }

                BusyIndicator {
                    anchors.centerIn: parent
                    running: root.loadingKeyTypes === true
                    width: typeSelector.height
                }

            }

            Loader {
                id: valueAddEditor
                Layout.fillWidth: true
                Layout.fillHeight: true
                Layout.preferredHeight: 300

                asynchronous: true
                source: Editor.getEditorByTypeString(
                            typeSelector.model[typeSelector.currentIndex], true)

                onLoaded: {
                    item.state = "new"
                    if (item.keyType !== undefined)
                        item.keyType = typeSelector.model[typeSelector.currentIndex]
                    item.initEmpty()
                }
            }

            BetterLabel { text: qsTranslate("RESP", "Or Import Value from the file") + ":" }

            FilePathInput {
                id: valueFilePath
                objectName: "rdm_add_key_value_file"
                Layout.fillWidth: true
                placeholderText: qsTranslate("RESP","(Optional) Any file")
                nameFilters: [ "Any file (*)" ]
                title: qsTranslate("RESP","Select file with value")
                path: ""
            }

            RowLayout {
                Layout.fillWidth: true
                Layout.minimumHeight: 40
                Item {
                    Layout.fillWidth: true
                }
                BetterButton {
                    objectName: "rdm_add_key_save_btn"
                    text: qsTranslate("RESP","Save")

                    function submitNewKeyRequest(row) {
                        root.request.keyName = newKeyName.text
                        root.request.keyType = typeSelector.model[typeSelector.currentIndex]
                        root.request.value = row
                        root.request.valueFilePath = valueFilePath.path
                        keyFactory.submitNewKeyRequest(root.request)
                    }


                    onClicked: {
                        if (!valueAddEditor.item)
                            return

                        var validateVal = (valueFilePath.path === "")

                        valueAddEditor.item.getValue(validateVal, function (valid, row) {
                            if (!valid)
                                return;

                            submitNewKeyRequest(row);
                        })
                    }

                    Connections {
                        target: keyFactory

                        function onKeyAdded() {
                            root.request = null
                            valueAddEditor.item.reset()
                            valueAddEditor.item.initEmpty()
                            valueFilePath.path = ""
                            root.close()
                        }

                        function onError(err) {
                            addError.text = err
                            addError.open()
                        }
                    }

                }

                BetterButton {
                    text: qsTranslate("RESP","Cancel")
                    onClicked: root.close()
                }
            }
            Item {
                Layout.fillWidth: true
            }
        }

        OkDialogOverlay {
            id: addError
            title: qsTranslate("RESP","Error")
            text: ""
            visible: false
        }
    }
}
