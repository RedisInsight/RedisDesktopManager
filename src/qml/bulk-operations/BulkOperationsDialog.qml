import QtQuick 2.3
import QtQuick.Layouts 1.1
import QtQuick.Controls 1.2
import QtQuick.Dialogs 1.2

Dialog {
    id: root
    title: qsTr("Bulk Operations Manager")
    modality: Qt.ApplicationModal

    property string operationName: "delete_keys"

    standardButtons: StandardButton.NoButton

    function loadKeys() {
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
                PropertyChanges { target: operationLabel; text: qsTr("Delete keys") }
                PropertyChanges { target: targetConnectionSettings; visible: false }
            },
            State {
                name: "copy_keys"
                PropertyChanges { target: operationLabel; text: qsTr("Copy keys") }
                PropertyChanges { target: targetConnectionSettings; visible: true }
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

                    Label {
                        text: bulkOperations.keyPattern
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

                    ComboBox {

                    }

                }
            }

            Item {
                Layout.preferredHeight: 10
            }

            ColumnLayout {
                Layout.fillWidth: true
                Layout.fillHeight: true

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
                            bulkErrorNotification.text = e
                            bulkErrorNotification.open()
                        }
                    }
                }
            }

            Item {
                Layout.fillHeight: true
            }

            RowLayout {
                Layout.fillWidth: true

                Item { Layout.fillWidth: true; }

                Button {
                    text: qsTr("Delete Keys")
                    onClicked: bulkConfirmation.open()
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
                BusyIndicator { anchors.centerIn: parent; running: true }
            }

            MouseArea {
                anchors.fill: parent
            }
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
            title: qsTr("Confirmation")
            text: qsTr("Do you really want to perform bulk operation?")
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
