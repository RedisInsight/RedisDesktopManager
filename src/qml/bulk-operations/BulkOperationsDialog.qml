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
        } else {
            targetConnection.model = bulkOperations.getTargetConnections()
        }
    }

    function resetKeysPreview() {
        keysPreview.visible = false
        btnShowAffectedKeys.visible = true
        spacer.visible = true
    }

    function setMetadata() {
        bulkOperations.setOperationMetadata(
                    {
                        "ttl": ttlValue.value,
                        "replace": replaceKeys.checked ? "replace": "",
                        "path": rdbPath.path,
                        "db": rdbDb.value
                    }
                    )
    }

    function showError(title, text, details) {
        uiBlocker.visible = false
        bulkErrorNotification.title = title
        bulkErrorNotification.text = text
        if (details) {
            bulkErrorNotification.detailedText = details
        }
        bulkErrorNotification.open()
    }

    function validate() {
        if (root.operationName == "rdb_import" && !qmlUtils.fileExists(rdbPath.path)) {
            showError(qsTr("Invalid RDB path"), qsTr("Please specify valid path to RDB file"), "")
            return false;
        }
        return true;
    }

    contentItem: Item {
        id: contentWrapper
        implicitWidth: 900
        implicitHeight: 600

        state: root.operationName

        states: [
            State {
                name: "delete_keys"
                PropertyChanges { target: operationLabel; text: qsTr("Delete keys") }
                PropertyChanges { target: actionButton; text:  qsTr("Delete keys") }
                PropertyChanges { target: ttlField; visible: false }
                PropertyChanges { target: replaceKeysField; visible: false }
                PropertyChanges { target: targetConnectionSettings; visible: false }
                PropertyChanges { target: contentWrapper; width: 600 }
                PropertyChanges { target: rdbImportFields; visible: false; }
            },
            State {
                name: "ttl"
                PropertyChanges { target: operationLabel; text: qsTr("Set TTL for multiple keys") }
                PropertyChanges { target: actionButton; text: qsTr("Set TTL") }
                PropertyChanges { target: ttlField; visible: true }
                PropertyChanges { target: replaceKeysField; visible: false }
                PropertyChanges { target: targetConnectionSettings; visible: false }
                PropertyChanges { target: contentWrapper; width: 600 }
                PropertyChanges { target: rdbImportFields; visible: false; }
            },
            State {
                name: "copy_keys"
                PropertyChanges { target: operationLabel; text: qsTr("Copy keys to another database") }
                PropertyChanges { target: actionButton; text:  qsTr("Copy keys") }
                PropertyChanges { target: ttlField; visible: true }
                PropertyChanges { target: replaceKeysField; visible: true }
                PropertyChanges { target: targetConnectionSettings; visible: true }
                PropertyChanges { target: contentWrapper; width: 1000 }
                PropertyChanges { target: rdbImportFields; visible: false; }
            },

            State {
                name: "rdb_import"
                PropertyChanges { target: operationLabel; text: qsTr("Import data from rdb file") }
                PropertyChanges { target: actionButton; text:  qsTr("Import") }
                PropertyChanges { target: ttlField; visible: false }
                PropertyChanges { target: replaceKeysField; visible: false }
                PropertyChanges { target: targetConnectionSettings; visible: false }
                PropertyChanges { target: contentWrapper; width: 600 }
                PropertyChanges { target: rdbImportFields; visible: true; }
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
                        Layout.preferredWidth: 250
                    }

                    Label {
                        text: bulkOperations.connectionName
                    }

                    Label {
                        text: qsTr("Database number:")
                        Layout.preferredWidth: 250
                    }

                    Label {
                        text: bulkOperations.dbIndex
                    }

                    GridLayout {
                        id: rdbImportFields
                        columns: 2
                        Layout.rowSpan: 2
                        Layout.columnSpan: 2
                        Layout.fillWidth: true

                        Label {
                            id: rdbPathLabel
                            text: qsTr("Path to RDB file:")
                            Layout.preferredWidth: 250
                        }

                        FilePathInput {
                            id: rdbPath
                            placeholderText: qsTranslate("RDM","Path to dump.rdb file")
                            nameFilters: [ "RDB (*.rdb)" ]
                            title: qsTranslate("RDM","Select dump.rdb")
                            path: ""
                            onPathChanged: {
                                console.log(rdbPath.path)
                            }
                        }

                        Label {
                            id: rdbDbLabel
                            text: qsTr("Select DB in RDB file:")
                            Layout.preferredWidth: 250
                        }

                        SpinBox {
                            id: rdbDb
                            minimumValue: 0
                            maximumValue: 10000000000
                            value: 0
                            decimals: 0
                            onValueChanged: {
                                setMetadata()
                                root.resetKeysPreview()
                            }
                        }
                    }

                    Label {
                        text: root.operationName == "rdb_import"? qsTr("Import keys that match <b>regex</b>:") : qsTr("Key pattern:")
                        Layout.preferredWidth: 250
                    }

                    BetterTextField {
                        text: bulkOperations.keyPattern
                        onTextChanged: {
                            bulkOperations.keyPattern = text
                            root.resetKeysPreview()
                        }
                    }

                    RowLayout {
                        id: ttlField

                        Layout.columnSpan: 2

                        Label {
                            text: "New TTL value (seconds):"
                            Layout.preferredWidth: 250
                        }

                        SpinBox {
                            id: ttlValue
                            minimumValue: -1
                            maximumValue: 10000000000
                            value: 0
                            decimals: 0
                        }
                    }
                }

                GridLayout {
                    id: targetConnectionSettings
                    columns: 2
                    visible: bulkOperations.multiConnectionOperation()

                    Label {
                        text: qsTr("Destination Redis Server:")
                    }

                    ComboBox {
                        id: targetConnection
                    }

                    Label {
                        text: qsTr("Destination Redis Server Database Index:")
                    }

                    SpinBox {
                        id: targetDatabaseIndex
                        minimumValue: 0
                        maximumValue: 10000000000
                        value: 0
                        decimals: 0
                    }

                    RowLayout{
                        id: replaceKeysField

                        Layout.columnSpan: 2

                        Label {
                            text: "Replace existing keys in target db:"
                            Layout.preferredWidth: 250
                        }

                        BetterCheckbox {
                            id: replaceKeys
                        }
                    }
                }
            }


            Item { Layout.preferredHeight: 10 }

            Button {
                id: btnShowAffectedKeys
                text: root.operationName == "rdb_import"? qsTr("Show matched keys") : qsTr("Show Affected keys")
                onClicked: {
                    if (!validate()) {
                        return;
                    }

                    uiBlocker.visible = true
                    setMetadata()
                    root.loadKeys()
                    btnShowAffectedKeys.visible = false
                    spacer.visible = false
                    keysPreview.visible = true
                }
            }

            ColumnLayout {
                id: keysPreview
                Layout.fillWidth: true
                Layout.fillHeight: true

                visible: false

                Text {
                    text: root.operationName == "rdb_import"? qsTr("Matched keys:")  : qsTr("Affected keys:")
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
                            showError(qsTr("Bulk Operation finished with errors"), e, details)
                        }
                    }
                }
            }

            Item { id: spacer; Layout.fillHeight: true }

            RowLayout {
                Layout.fillWidth: true

                Item { Layout.fillWidth: true; }

                Button {
                    id: actionButton
                    onClicked: {
                        if (!validate()) {
                            return;
                        }

                        setMetadata()
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
                bulkOperations.runOperation(targetConnection.currentIndex, targetDatabaseIndex.value)
            }
            visible: false
            modality: Qt.ApplicationModal
            icon: StandardIcon.Warning
            standardButtons: StandardButton.Yes | StandardButton.No
        }
    }
}

