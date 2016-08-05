import QtQuick 2.3
import QtQuick.Layouts 1.1
import QtQuick.Controls 1.2
import QtQuick.Dialogs 1.2

import "./../common/"

Dialog {
    id: root
    title: "Bulk Operations Manager"
    modality: Qt.ApplicationModal

    property string operationName: bulkOperations.operationName

    standardButtons: StandardButton.NoButton

    function loadKeys() {
        uiBlocker.visible = true
        bulkOperations.getAffectedKeys()        
    }

    onVisibleChanged: {
        if (visible == false) {
            bulkOperations.clearOperation();
        } else {
            if (!uiBlocker.visible) {
                loadKeys();
            }
        }
    }

//    Timer {
//        id: ignoreCloseTimer
//        repeat: false
//        interval: 100
//        onTriggered: {
//            open()
//        }
//    }

//    onVisibleChanged: {
//        if (visible === false) {
//            console.log("ignore", visible)
//            ignoreCloseTimer.start()
//        }
//    }

    contentItem: Item {
        implicitWidth: 800
        implicitHeight: 600

        state: root.operationName

        states: [
            State {
                name: "delete_keys"
                PropertyChanges { target: operationLabel; text: "Delete keys" }
                PropertyChanges { target: actionButton; text: "Delete keys" }
                PropertyChanges { target: targetConnectionSettings; visible: false }
                PropertyChanges { target: exportToFile; visible: false }
            },
            State {
                name: "text_export"
                PropertyChanges { target: operationLabel; text: "Export Keys as Text Commands" }
                PropertyChanges { target: actionButton; text: "Export keys" }
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

                    Label { text: "Redis Server:" }
                    Label { text: bulkOperations.connectionName }

                    Label { text: "Database number:" }
                    Label { text: bulkOperations.dbIndex }

                    Label { text: "Key pattern:" }
                    Label { text: bulkOperations.keyPattern }
                }

                GridLayout {
                    id: targetConnectionSettings
                    columns: 2

                    // TODO: Implement UI for target connection
                }
            }

            Item { Layout.preferredHeight: 10 }

            ColumnLayout {
                Layout.fillWidth: true
                Layout.fillHeight: true

                Text {
                    text: "Affected keys:"
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
                            bulkSuccessNotification.text = "Bulk Operation finished."
                            bulkSuccessNotification.open()
                        }

                        onError: {
                            uiBlocker.visible = false
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
                    selectExisting: false
                }
            }

            Item { Layout.fillHeight: true }

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
                    text: "Cancel"
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
                BusyIndicator { anchors.centerIn: parent; running: true }
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
                bulkOperations.notifyAboutOperationSuccess();
                bulkOperations.clearOperation();
                root.close()
            }
        }

        MessageDialog {
            id: bulkConfirmation
            title: "Confirmation"
            text: "Do you really want to perform bulk operation?"
            onYes: {
                bulkOperations.runOperation()
            }
            visible: false
            modality: Qt.ApplicationModal
            icon: StandardIcon.Warning
            standardButtons: StandardButton.Yes | StandardButton.No
        }
    }
}
