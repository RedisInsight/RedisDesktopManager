import QtQuick 2.13
import QtQuick.Layouts 1.1
import QtQuick.Controls 2.13
import QtQuick.Controls.Styles 1.1
import QtQuick.Window 2.2
import "./editors/editor.js" as Editor
import "./../common/platformutils.js" as PlatformUtils
import "./../common"
import rdm.models 1.0
import Qt.labs.qmlmodels 1.0

Repeater {

    BetterTab {
        id: keyTab
        objectName: "rdm_value_tab"

        Component {
            id: valueTabButton

            BetterTabButton {
                objectName: "rdm_value_tab_btn"
                icon.source: "qrc:/images/key.svg"

                text: tabName
                tooltip: keyModel && tabName <= keyName? keyName : ""

                onCloseClicked: {
                    if (valueEditor.item && valueEditor.item.isEdited() && keyType != "stream") {
                        closeConfirmation.open()
                    } else {
                        valuesModel.closeTab(keyIndex)
                    }
                }

                BetterMessageDialog {
                    id: closeConfirmation
                    title: qsTranslate("RDM","Changes are not saved")
                    text: qsTranslate("RDM","Do you want to close key tab without saving changes?")

                    modality: Qt.WindowModal
                    visible: false

                    onYesClicked: {
                        valuesModel.closeTab(keyIndex)
                    }
                }
            }
        }

        property int tabIndex: keyIndex
        property var table
        property var valueEditor
        property var searchModel
        property var tabButton
        property bool loadingModel: showLoader
        property variant keyModel: keyViewModel

        onKeyModelChanged: {
            console.log("keyModel changed")
            if (keyModel && keyModel.isLoaded) {
                table.forceLoading = false
                table.currentStart = 0
                table.searchField.text = ""

                if (valueEditor.item)
                    valueEditor.item.reset()

                table.loadValue()
            }
        }

        property Component searchModelComponent: Component {
            SortFilterProxyModel {
                source: keyViewModel
                sortOrder: Qt.AscendingOrder
                sortCaseSensitivity: Qt.CaseInsensitive
                sortRole: keyTab.keyModel && keyTab.keyModel.isLoaded ? "row" : ""

                filterString: table.searchField.text
                filterSyntax: SortFilterProxyModel.Wildcard
                filterCaseSensitivity: Qt.CaseInsensitive
                filterKeyColumn: -1

                onFilterStringChanged: {
                    table.resetCurrentRow()
                }

                Component.onCompleted:  {
                    if (keyTab.keyModel && keyTab.keyModel.isLoaded && keyTab.keyModel.singlePageMode) {
                        // NOTE(u_glide): disable live search in all values
                        filterString = table.searchField.text
                    }
                }
            }
        }

        Keys.onPressed: {
            var reloadKey = event.key == Qt.Key_F5
                    || (event.key == Qt.Key_R && (event.modifiers & Qt.ControlModifier))
                    || (event.key == Qt.Key_R && (event.modifiers & Qt.MetaModifier))

            if (reloadKey && keyModel.isLoaded) {
                console.log("Reload")
                keyModel.reload()
            }
        }

        Component.onCompleted: {
            keyTab.focus = true
            keyTab.forceActiveFocus()

            // Update tabBar
            if (!tabButton) {
                tabButton = valueTabButton.createObject(keyTab);
                tabButton.self = tabButton;
                tabButton.tabRef = keyTab;
                tabBar.addItem(tabButton)
                tabBar.activateTabButton(tabButton)
                tabs.activateTab(keyTab)
            }
        }

        onActivate: {
            valuesModel.setCurrentTab(keyIndex)
        }

        function reloadValue() {
            console.log("Reload value in tab")
            keyTab.keyModel.reload()

            if (isMultiRow) {
                valueEditor.clear()
                table.resetCurrentRow()

                if (table.currentPage > table.totalPages) {
                    table.goToPage(1)
                }
            }
        }

        Rectangle {
            id: wrapper
            color: sysPalette.base
            anchors.fill: parent
            anchors.margins: 5

            function showLoader() {
                uiBlocker.visible = true
            }

            function hideLoader() {
                uiBlocker.visible = false
            }

            ColumnLayout {
                visible: !loadingModel
                anchors.fill: parent
                spacing: 5

                RowLayout {
                    Layout.preferredHeight: 30
                    Layout.minimumHeight: 30
                    Layout.fillWidth: true
                    spacing: 5

                    BetterLabel {
                        Layout.preferredWidth: isMultiRow ? 70 : 90
                        text: keyModel? keyType.toUpperCase() + ":" : "";
                        font.bold: true
                    }

                    BetterTextField {
                        id: keyNameField
                        Layout.fillWidth: true
                        text: keyModel? keyName : ""
                        readOnly: true
                        objectName: "rdm_key_name_field"
                    }

                    BetterButton {
                        Layout.preferredWidth: 98

                        text: qsTranslate("RDM","Rename key")
                        objectName: "rdm_key_rename_btn"

                        onClicked: renameConfirmation.open()

                        BetterDialog {
                            id: renameConfirmation
                            title: qsTranslate("RDM","Rename key")

                            width: 520

                            RowLayout {
                                implicitWidth: 500
                                implicitHeight: 100
                                width: 500

                                BetterLabel { text: qsTranslate("RDM","New name:") }
                                BetterTextField {
                                    id: newKeyName;
                                    Layout.fillWidth: true;
                                    objectName: "rdm_rename_key_field"
                                    text: keyModel? keyName : ""
                                }
                            }

                            onAccepted: {
                                if (newKeyName.text.length == 0) {
                                    return open()
                                }

                                keyTab.keyModel.renameKey(newKeyName.text)
                            }

                            visible: false
                        }
                    }

                    BetterLabel {
                        visible: keyType === "hyperloglog";
                        text:  qsTranslate("RDM","Size: ") + keyRowsCount
                    }

                    BetterButton {
                        Layout.preferredWidth: 98

                        text: qsTranslate("RDM","TTL:") + keyTtl
                        objectName: "rdm_key_ttl_value"

                        BetterDialog {
                            id: setTTLConfirmation
                            title: qsTranslate("RDM","Set key TTL")

                            width: 520

                            RowLayout {
                                implicitWidth: 500
                                implicitHeight: 100
                                width: 500

                                BetterLabel { text: qsTranslate("RDM","New TTL:") }
                                BetterTextField {
                                    id: newTTL;
                                    Layout.fillWidth: true;
                                    objectName: "rdm_set_ttl_key_field"
                                    inputMethodHints: Qt.ImhDigitsOnly
                                    validator: IntValidator{bottom: 1}
                                }
                            }

                            footer: BetterDialogButtonBox {
                                BetterButton {
                                    text: qsTranslate("RDM","Save")
                                    DialogButtonBox.buttonRole: DialogButtonBox.AcceptRole
                                }

                                BetterButton {
                                    text: qsTranslate("RDM","Persist key")
                                    onClicked: {
                                        keyTab.keyModel.persistKey()
                                        setTTLConfirmation.close()
                                    }
                                }

                                BetterButton {
                                    text: qsTranslate("RDM","Cancel")
                                    onClicked: setTTLConfirmation.close()
                                }
                            }

                            onAccepted: {
                                if (newTTL.text.length == 0) {
                                    return open()
                                }

                                keyTab.keyModel.setTTL(newTTL.text)
                            }

                            visible: false
                        }

                        onClicked: {
                            if (keyTtl > 0) {
                                newTTL.text = ""+keyTtl
                            } else {
                                newTTL.text = ""
                            }

                            setTTLConfirmation.open()
                        }
                    }

                    BetterButton {
                        objectName: "rdm_value_tab_delete_btn"
                        Layout.preferredWidth: 98
                        text: qsTranslate("RDM","Delete")
                        iconSource: "qrc:/images/delete.svg"

                        BetterMessageDialog {
                            id: deleteConfirmation
                            title: qsTranslate("RDM","Delete key")
                            text: qsTranslate("RDM","Do you really want to delete this key?")
                            onYesClicked: {
                                keyTab.keyModel.removeKey()
                            }
                            visible: false
                        }

                        onClicked: {
                            deleteConfirmation.open()
                        }
                    }

                    BetterButton {
                        objectName: "rdm_value_editor_reload_value_btn"
                        text: qsTranslate("RDM","Reload Value")
                        onClicked: reloadValue()
                        visible: !isMultiRow
                        iconSource: "qrc:/images/refresh.svg"
                    }
                }

                BetterSplitView {
                    orientation: Qt.Vertical
                    Layout.fillHeight: true
                    Layout.fillWidth: true

                    // Table
                    ValueTable {
                        id: navigationTable

                        Layout.fillWidth: true
                        Layout.fillHeight: false
                        Layout.bottomMargin: 20
                        SplitView.minimumHeight: 300
                        visible: keyModel? isMultiRow : false
                    }

                    // Value editor
                    ColumnLayout {
                        id: editorWrapper
                        SplitView.fillWidth: true
                        SplitView.fillHeight: !isMultiRow
                        Layout.topMargin: 20
                        SplitView.minimumHeight: 220
                        spacing: 10

                        Loader {
                            id: valueEditor
                            objectName: "rdm_value_editor_loader"

                            Layout.topMargin: 5
                            Layout.fillWidth: true
                            Layout.fillHeight: true
                            Layout.minimumHeight: 180

                            Component.onCompleted: {
                                keyTab.valueEditor = valueEditor
                            }

                            property int currentRow: -1

                            source: keyTab.keyModel? Editor.getEditorByTypeString(keyType) : ""

                            function loadRowValue(row) {
                                console.log("loading row value", row)
                                if (valueEditor.item) {
                                    var rowValue = keyTab.keyModel.getRow(row)
                                    valueEditor.currentRow = row
                                    valueEditor.item.setValue(rowValue)
                                } else {
                                    console.log("cannot load row value - item is missing")
                                }
                            }

                            function clear() {
                                if (valueEditor.item) {
                                    currentRow = -1

                                    if (valueEditor.item.keyType !== undefined) {
                                        valueEditor.item.keyType = keyType
                                    }

                                    valueEditor.item.reset()
                                }
                            }

                            onLoaded: clear()
                        }
                    }
                    // Value editor end
                }
            }

            Rectangle {
                id: uiBlocker
                visible: loadingModel
                anchors.fill: parent
                color: loadingModel? Qt.rgba(0, 0, 0, 0) : Qt.rgba(0, 0, 0, 0.1)

                Item {
                    anchors.fill: parent

                    ColumnLayout {
                        anchors.centerIn: parent;

                        BusyIndicator { Layout.alignment: Qt.AlignHCenter;  running: true }

                        BetterLabel {
                            visible: loadingModel
                            text: tabName
                        }
                    }
                }

                MouseArea {
                    anchors.fill: parent
                }
            }
        }
    }
}
