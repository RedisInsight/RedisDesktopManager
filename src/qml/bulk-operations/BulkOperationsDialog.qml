import QtQuick 2.3
import QtQuick.Layouts 1.1
import QtQuick.Controls 1.2
import QtQuick.Dialogs 1.2

import "./../common/"

Dialog {
    id: root
    title: qsTr("Bulk Operations Manager")
    modality: Qt.ApplicationModal

    property string operationName: bulkOperations.operationName

    standardButtons: StandardButton.NoButton

    function loadKeys() {
        bulkOperations.getAffectedKeys()
    }

    onVisibleChanged: {
        if (visible == false) {
            bulkOperations.clearOperation();
            resetKeysPreview()
        }
    }

    function resetKeysPreview() {
        keysPreview.visible = false
        btnShowAffectedKeys.visible = true
        spacer.visible = true
    }

    contentItem: Item {
        implicitWidth: 800
        implicitHeight: 600

        state: root.operationName

        states: [
            State {
                name: "delete_keys"
                PropertyChanges { target: operationLabel; text: qsTr("Delete keys") }
                PropertyChanges { target: actionButton; text:  qsTr("Delete keys") }
                PropertyChanges { target: targetConnectionSettings; visible: false }
                PropertyChanges { target: exportToFile; visible: false }
            },
            State {
                name: "text_export"
                PropertyChanges { target: operationLabel; text: qsTr("Export Keys as Text Commands") }
                PropertyChanges { target: actionButton; text: qsTr("Export keys") }
                PropertyChanges { target: targetConnectionSettings; visible: false }
                PropertyChanges { target: exportToFile; visible: true }
            }
        ]

        ColumnLayout {
            anchors.fill: parent
            anchors.margins: 20

            Text {
                id: operationLabel
                font.pixelSize: 20
            }

            Rectangle {
                color: "#e2e2e2"
                Layout.preferredHeight: 1
                Layout.fillWidth: true
            }

            RowLayout {
                Layout.fillWidth: true

                GridLayout {
                    id: sourceConnectionSettings
                    columns: 2

                    Label {
                        text: qsTr("Redis Server:")
                    }

                    Label {
                        text: bulkOperations.connectionName
                    }

                    Label {
                        text: qsTr("Database number:")
                    }

                    Label {
                        text: bulkOperations.dbIndex
                    }

                    Label {
                        text: qsTr("Key pattern:")
                    }

                    BetterTextField {
                        text: bulkOperations.keyPattern
                        onTextChanged: {
                            bulkOperations.keyPattern = text
                            root.resetKeysPreview()
                        }
                    }
                }

                GridLayout {
                    id: targetConnectionSettings
                    columns: 2

                    Label {
                        text: qsTr("Destination Redis Server:")
                    }

                    ComboBox {

                    }

                    Label {
                        text: qsTr("Destination Redis Server Database Index:")
                    }
                }
            }
            Item { Layout.preferredHeight: 10 }

            Button {
                id: btnShowAffectedKeys
                text: qsTr("Show Affected keys")
                onClicked: {
                    uiBlocker.visible = true
                    root.loadKeys()
                    btnShowAffectedKeys.visible = false
                    keysPreview.visible = true
                    spacer.visible = false                    
                }
            }

            Item { id: spacer; Layout.fillHeight: true }

            ColumnLayout {
                id: keysPreview
                Layout.fillWidth: true
                Layout.fillHeight: true

                visible: false

                Text {
                    text: qsTr("Affected keys:")
                }

                Rectangle {
                    id: listContainer
                    color: "#eee"

                    border.color: "#ccc"
                    border.width: 1

                    Layout.fillWidth: true
                    Layout.fillHeight: true

                    ScrollView {
                        anchors.fill: parent
                        anchors.margins: 10

                        verticalScrollBarPolicy: Qt.ScrollBarAlwaysOn

                        ListView {
                            id: affectedKeysListView
                            width: listContainer.width * 0.9

                            delegate: Text { text: (index+1) + ". " + modelData}
                        }
                    }

                    Connections {
                        target: bulkOperations

                        onAffectedKeys: {
                            console.log("Affected keys loaded")
                            affectedKeysListView.model = r
                            uiBlocker.visible = false
                        }

                        onOperationFinished: {
                            affectedKeysListView.model = []
                            uiBlocker.visible = false
                            bulkSuccessNotification.text = qsTr("Bulk Operation finished.")
                            bulkSuccessNotification.open()
                        }

                        onError: {
                            uiBlocker.visible = false
                            bulkErrorNotification.title = qsTr("Bulk Operation finished with errors")
                            bulkErrorNotification.text = e
                            bulkErrorNotification.open()
                        }
                    }
                }
            }

            RowLayout {
                id: exportToFile

                Layout.fillWidth: true

                Label { text: "Export to file:" }

                FilePathInput {
                    id: exportFilePathField
                }
            }            

            RowLayout {
                Layout.fillWidth: true

                Item { Layout.fillWidth: true; }

                Button {
                    id: actionButton
                    onClicked: {

                        if (root.operationName == "text_export") {
                            if (!exportFilePathField.path) {
                                bulkErrorNotification.title = "Validation Error"
                                bulkErrorNotification.text = "Please select file for exported keys."
                                bulkErrorNotification.open()
                                return
                            } else {
                                bulkOperations.setOperationMetadata({"path": exportFilePathField.path})
                            }
                        }

                        bulkConfirmation.open()
                    }
                }

                Button {
                    text: qsTr("Cancel")
                    onClicked: root.close()
                }
            }
        }


        Rectangle {
            id: uiBlocker
            visible: false
            anchors.fill: parent
            color: Qt.rgba(0, 0, 0, 0.1)

            Item {
                anchors.fill: parent

                ColumnLayout {
                    anchors.centerIn: parent;

                    BusyIndicator { running: true }
                    Label {
                        text: {
                            if (bulkOperations.operationProgress > 0)
                                return "Processed: " + bulkOperations.operationProgress
                            else {
                                return "Getting list of affected keys..."
                            }
                        }
                    }
                }
            }

            MouseArea { anchors.fill: parent }
        }

        MessageDialog {
            id: bulkErrorNotification
            visible: false
            modality: Qt.WindowModal
            icon: StandardIcon.Warning
            standardButtons: StandardButton.Ok
        }

        MessageDialog {
            id: bulkSuccessNotification
            visible: false
            modality: Qt.WindowModal
            icon: StandardIcon.Information
            standardButtons: StandardButton.Ok

            onAccepted: cleanUp()

            onVisibilityChanged: {
                if (visible == false)
                    cleanUp()
            }

            function cleanUp() {                
                bulkOperations.clearOperation();
                uiBlocker.visible = false
                root.close()
            }
        }

        MessageDialog {
            id: bulkConfirmation
            title: qsTr("Confirmation")
            text: qsTr("Do you really want to perform bulk operation?")
            onYes: {
                uiBlocker.visible = true
                bulkOperations.runOperation()
            }
            visible: false
            modality: Qt.ApplicationModal
            icon: StandardIcon.Warning
            standardButtons: StandardButton.Yes | StandardButton.No
        }
    }
}

