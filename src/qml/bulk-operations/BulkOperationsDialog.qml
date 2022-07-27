import QtQuick 2.3
import QtQuick.Layouts 1.1
import QtQuick.Controls 2.13
import QtQuick.Dialogs 1.3
import "./../common/"
import "../common/platformutils.js" as PlatformUtils

BetterDialog {
    id: root
    title: qsTranslate("RESP","Bulk Operations Manager")

    footer: null

    property string operationName: bulkOperations.operationName

    property int firstColSize: PlatformUtils.isScalingDisabled()? 300 : 250

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
        } else {
            bulkErrorNotification.detailedText = ""
        }

        bulkErrorNotification.open()
    }

    function validate() {
        if (root.operationName == "rdb_import" && !qmlUtils.fileExists(rdbPath.path)) {
            rdbPath.validationError = true
            showError(qsTranslate("RESP","Invalid RDB path"), qsTranslate("RESP","Please specify valid path to RDB file"), "")
            return false;
        }
        rdbPath.validationError = false
        return true;
    }

    contentItem: Rectangle {
        id: contentWrapper
        implicitWidth: PlatformUtils.isScalingDisabled() ? 1100 : 900
        implicitHeight: PlatformUtils.isScalingDisabled() ? 650 : 600
        color: sysPalette.base

        Control {
            palette: approot.palette
            anchors.fill: parent

            state: root.operationName

            states: [
                State {
                    name: "delete_keys"
                    PropertyChanges { target: operationLabel; text: qsTranslate("RESP","Delete keys") }
                    PropertyChanges { target: actionButton; text:  qsTranslate("RESP","Delete keys") }
                    PropertyChanges { target: ttlField; visible: false }
                    PropertyChanges { target: replaceKeysField; visible: false }
                    PropertyChanges { target: targetConnectionSettings; visible: false }
                    PropertyChanges { target: rdbImportFields; visible: false; }
                },
                State {
                    name: "ttl"
                    PropertyChanges { target: operationLabel; text: qsTranslate("RESP","Set TTL for multiple keys") }
                    PropertyChanges { target: actionButton; text: qsTranslate("RESP","Set TTL") }
                    PropertyChanges { target: ttlField; visible: true }
                    PropertyChanges { target: replaceKeysField; visible: false }
                    PropertyChanges { target: targetConnectionSettings; visible: false }
                    PropertyChanges { target: rdbImportFields; visible: false; }
                },
                State {
                    name: "copy_keys"
                    PropertyChanges { target: operationLabel; text: qsTranslate("RESP","Copy keys to another database") }
                    PropertyChanges { target: actionButton; text:  qsTranslate("RESP","Copy keys") }
                    PropertyChanges { target: ttlField; visible: true }
                    PropertyChanges { target: replaceKeysField; visible: true }
                    PropertyChanges { target: targetConnectionSettings; visible: true }
                    PropertyChanges { target: rdbImportFields; visible: false; }
                },

                State {
                    name: "rdb_import"
                    PropertyChanges { target: operationLabel; text: qsTranslate("RESP","Import data from rdb file") }
                    PropertyChanges { target: actionButton; text:  qsTranslate("RESP","Import") }
                    PropertyChanges { target: ttlField; visible: false }
                    PropertyChanges { target: replaceKeysField; visible: false }
                    PropertyChanges { target: targetConnectionSettings; visible: false }
                    PropertyChanges { target: rdbImportFields; visible: true; }
                }
            ]

            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 20

                BetterLabel {
                    id: operationLabel
                    font.pixelSize: 20
                }

                Rectangle {
                    color: sysPalette.mid
                    Layout.preferredHeight: 1
                    Layout.fillWidth: true
                }

                Item {
                    Layout.preferredHeight: 5
                }

                GridLayout {
                    id: sourceConnectionSettings
                    columns: 2

                    Layout.fillWidth: true

                    BetterLabel {
                        text: qsTranslate("RESP","Redis Server:")
                        Layout.preferredWidth: root.firstColSize
                        Layout.preferredHeight: 25
                    }

                    BetterLabel {
                        Layout.fillWidth: true
                        Layout.preferredHeight: 25
                        text: bulkOperations.connectionName
                    }

                    BetterLabel {
                        text: qsTranslate("RESP","Database number:")
                        Layout.preferredWidth: root.firstColSize
                        Layout.preferredHeight: 25
                    }

                    BetterLabel {
                        Layout.fillWidth: true
                        Layout.preferredHeight: 25
                        text: bulkOperations.dbIndex
                    }

                    GridLayout {
                        id: rdbImportFields
                        columns: 2
                        Layout.rowSpan: 2
                        Layout.columnSpan: 2
                        Layout.fillWidth: true

                        BetterLabel {
                            id: rdbPathLabel
                            text: qsTranslate("RESP","Path to RDB file:")
                            Layout.preferredWidth: root.firstColSize
                        }

                        FilePathInput {
                            id: rdbPath

                            Layout.fillWidth: true

                            objectName: "rdm_bulk_operations_dialog_rdb_path"

                            placeholderText: qsTranslate("RESP","Path to dump.rdb file")
                            nameFilters: [ "RDB (*.rdb)" ]
                            title: qsTranslate("RESP","Select dump.rdb")
                            path: ""
                            onPathChanged: {
                                console.log(rdbPath.path)
                            }
                        }

                        BetterLabel {
                            id: rdbDbLabel
                            text: qsTranslate("RESP","Select DB in RDB file:")
                            Layout.preferredWidth: root.firstColSize
                        }

                        BetterSpinBox {
                            id: rdbDb

                            Layout.fillWidth: true

                            from: 0
                            to: 10000000
                            value: 0
                            onValueChanged: {
                                setMetadata()
                                root.resetKeysPreview()
                            }
                        }
                    }

                    BetterLabel {
                        text: root.operationName == "rdb_import"? qsTranslate("RESP","Import keys that match <b>regex</b>:") : qsTranslate("RESP","Key pattern:")
                        Layout.preferredWidth: root.firstColSize
                    }

                    BetterTextField {
                        objectName: "rdm_bulk_operations_dialog_key_pattern"
                        Layout.fillWidth: true
                        text: bulkOperations.keyPattern
                        onTextChanged: {
                            bulkOperations.keyPattern = text
                            root.resetKeysPreview()
                        }
                    }

                    RowLayout {
                        id: ttlField

                        Layout.fillWidth: true
                        Layout.columnSpan: 2

                        BetterLabel {
                            text: "New TTL value (seconds):"
                            Layout.preferredWidth: root.firstColSize
                        }

                        BetterSpinBox {
                            id: ttlValue
                            objectName: "rdm_bulk_operations_dialog_ttl_value"

                            Layout.fillWidth: true


                            from: -1
                            to: 10000000
                            value: 0
                        }
                    }
                }

                GridLayout {
                    id: targetConnectionSettings
                    columns: 2
                    Layout.fillWidth: true
                    visible: bulkOperations.multiConnectionOperation()

                    BetterLabel {
                        Layout.preferredWidth: root.firstColSize
                        text: qsTranslate("RESP","Destination Redis Server:")
                    }

                    BetterComboBox {
                        id: targetConnection
                        objectName: "rdm_bulk_operations_dialog_connection_combobox"
                        Layout.fillWidth: true
                    }

                    BetterLabel {
                        Layout.preferredWidth: root.firstColSize
                        text: qsTranslate("RESP","Destination Redis Server Database Index:")
                    }

                    BetterSpinBox {
                        id: targetDatabaseIndex

                        Layout.fillWidth: true

                        objectName: "rdm_bulk_operations_dialog_target_db_index"

                        from: 0
                        to: 10000000
                        value: 0
                    }

                    RowLayout{
                        id: replaceKeysField

                        Layout.columnSpan: 2

                        BetterLabel {
                            text: "Replace existing keys in target db:"
                            Layout.preferredWidth: root.firstColSize
                        }

                        BetterCheckbox {
                            id: replaceKeys
                            objectName: "rdm_bulk_operations_dialog_replace_keys"
                            Layout.fillWidth: true
                        }
                    }
                }

                Item { Layout.preferredHeight: 10 }

                BetterButton {
                    id: btnShowAffectedKeys
                    text: root.operationName == "rdb_import"? qsTranslate("RESP","Show matched keys") : qsTranslate("RESP","Show Affected keys")
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

                    BetterLabel {
                        text: root.operationName == "rdb_import"? qsTranslate("RESP","Matched keys:")  : qsTranslate("RESP","Affected keys:")
                    }

                    FastTextView {
                        id: affectedKeysListView
                        color: sysPalette.base

                        border.color: sysPalette.shadow
                        border.width: 1

                        Layout.fillWidth: true
                        Layout.fillHeight: true

                        Connections {
                            target: bulkOperations

                            function onAffectedKeys(r) {
                                console.log("Affected keys loaded")
                                affectedKeysListView.model = r
                                uiBlocker.visible = false
                            }

                            function onOperationFinished() {
                                affectedKeysListView.model = []
                                uiBlocker.visible = false
                                bulkSuccessNotification.text = qsTranslate("RESP","Bulk Operation finished.")
                                bulkSuccessNotification.open()
                            }

                            function onError(e, details) {
                                showError(qsTranslate("RESP","Bulk Operation finished with errors"), e, details)
                            }
                        }
                    }
                }

                Item { id: spacer; Layout.fillHeight: true }

                RowLayout {
                    Layout.fillWidth: true

                    Item { Layout.fillWidth: true; }

                    BetterButton {
                        id: actionButton
                        objectName: "rdm_bulk_operations_dialog_action_button"


                        onClicked: {
                            if (!validate()) {
                                return;
                            }

                            setMetadata()
                            bulkConfirmation.open()
                        }
                    }

                    BetterButton {
                        text: qsTranslate("RESP","Cancel")
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
                        BetterLabel {
                            text: {
                                if (bulkOperations.operationProgress > 0)
                                    return qsTranslate("RESP","Processed: ") + bulkOperations.operationProgress
                                else {
                                    return qsTranslate("RESP", "Getting list of affected keys...")
                                }
                            }
                        }
                    }
                }

                MouseArea { anchors.fill: parent }
            }

            Loader {
                id: bulkErrorNotification

                property var icon: StandardIcon.Warning
                property string title
                property string text
                property string detailedText

                Component {
                    id: bulkNotificationTemplate

                    OkDialog {
                        modality: Qt.NonModal
                        title: bulkErrorNotification.title
                        icon: bulkErrorNotification.icon
                        text: bulkErrorNotification.text
                        detailedText: bulkErrorNotification.detailedText

                        onVisibleChanged: {
                            if (!visible) {
                                bulkErrorNotification.sourceComponent = undefined
                            }
                        }
                    }
                }

                onLoaded: {
                    item.open()
                }

                function open() {
                    sourceComponent = bulkNotificationTemplate
                }
            }

            OkDialogOverlay {
                id: bulkSuccessNotification

                title: qsTranslate("RESP","Success")

                x: (root.width - width) / 2
                y: (root.height - height) / 3

                visible: false
                onAccepted: cleanUp()

                onVisibleChanged: {
                    if (visible == false)
                        cleanUp()
                }

                function cleanUp() {
                    bulkOperations.clearOperation();
                    uiBlocker.visible = false
                    root.close()
                }
            }

            BetterMessageDialog {
                id: bulkConfirmation

                x: (root.width - width) / 2
                y: (root.height - height) / 3

                title: qsTranslate("RESP", "Confirmation")
                text: qsTranslate("RESP", "Do you really want to perform bulk operation?")
                onYesClicked: {
                    uiBlocker.visible = true
                    bulkOperations.runOperation(targetConnection.currentIndex, targetDatabaseIndex.value)
                }
                visible: false
            }
        }
    }
}

