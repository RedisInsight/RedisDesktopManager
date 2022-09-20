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
                icon.source: PlatformUtils.getThemeIcon("key.svg")

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
                    title: qsTranslate("RESP","Changes are not saved")
                    text: qsTranslate("RESP","Do you want to close key tab without saving changes?")

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
        property var addRowDialog

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
                        horizontalAlignment: Text.AlignHCenter
                    }

                    BetterTextField {
                        id: keyNameField
                        Layout.fillWidth: true
                        text: keyModel? keyName : ""
                        readOnly: true
                        objectName: "rdm_key_name_field"                    

                        ImageButton {
                            anchors.right: parent.right
                            anchors.rightMargin: 5
                            anchors.verticalCenter: parent.verticalCenter

                            iconSource: PlatformUtils.getThemeIcon("cleanup.svg")
                            tooltip: qsTranslate("RESP","Rename key")
                            objectName: "rdm_key_rename_btn"

                            onClicked: renameConfirmation.open()

                            BetterDialog {
                                id: renameConfirmation
                                title: qsTranslate("RESP","Rename key")

                                width: 520

                                RowLayout {
                                    implicitWidth: 500
                                    implicitHeight: 100
                                    width: 500

                                    BetterLabel { text: qsTranslate("RESP","New name:") }
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
                    }

                    BetterLabel {
                        visible: keyType === "hyperloglog";
                        text:  qsTranslate("RESP","Size: ") + keyRowsCount
                    }

                    BetterButton {
                        Layout.preferredWidth: isMultiRow? 92 : 98

                        text: qsTranslate("RESP","TTL:") + keyTtl
                        objectName: "rdm_key_ttl_value"
                        tooltip: keyTtl

                        BetterDialog {
                            id: setTTLConfirmation
                            title: qsTranslate("RESP","Set key TTL")

                            width: 520

                            RowLayout {
                                implicitWidth: 500
                                implicitHeight: 100
                                width: 500

                                BetterLabel { text: qsTranslate("RESP","New TTL:") }
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
                                    text: qsTranslate("RESP","Save")
                                    DialogButtonBox.buttonRole: DialogButtonBox.AcceptRole
                                }

                                BetterButton {
                                    objectName: "rdm_persist_key_btn"
                                    text: qsTranslate("RESP","Persist key")
                                    onClicked: {
                                        keyTab.keyModel.persistKey()
                                        setTTLConfirmation.close()
                                    }
                                }

                                BetterButton {
                                    text: qsTranslate("RESP","Cancel")
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
                        text: qsTranslate("RESP","Delete")
                        iconSource: PlatformUtils.getThemeIcon("delete.svg")

                        BetterMessageDialog {
                            id: deleteConfirmation
                            title: qsTranslate("RESP","Delete key")
                            text: qsTranslate("RESP","Do you really want to delete this key?")
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
                        text: qsTranslate("RESP","Reload Value")
                        onClicked: reloadValue()
                        visible: !isMultiRow
                        iconSource: PlatformUtils.getThemeIcon("refresh.svg")
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
                    Item {
                        id: editorWrapper

                        SplitView.fillWidth: true
                        SplitView.fillHeight: !isMultiRow
                        Layout.topMargin: 20
                        SplitView.minimumHeight: 220

                        BetterDialog {
                            id: fullScreenEditorDialog
                            title: tabName

                            width: approot.width * 0.9
                            height: approot.height * 0.9
                            footer: null

                            Rectangle {
                                id: fullscreenEditorParent

                                anchors.fill: parent
                                implicitHeight: 500
                                implicitWidth: 800
                            }

                            onClosed: {
                                editor.state = "default"
                            }
                        }

                        Rectangle {
                            id: editor
                            anchors.fill: parent

                            color: sysPalette.base

                            state: "default"

                            states: [
                                State {
                                        name: "full_screen"
                                        ParentChange { target: editor; parent: fullscreenEditorParent;}
                                        PropertyChanges {
                                            target: fullScreenEditorDialog
                                            visible: true
                                        }
                                        PropertyChanges {
                                            target: fullScreenModeBtn
                                            visible: false
                                        }
                                    },
                                State {
                                    name: "default"
                                    ParentChange { target: editor; parent: editorWrapper; }
                                    PropertyChanges {
                                        target: fullScreenEditorDialog
                                        visible: false
                                    }
                                    PropertyChanges {
                                        target: fullScreenModeBtn
                                        visible: true
                                    }
                                }
                            ]                            

                            ColumnLayout {
                                id: editorLayout
                                anchors.fill: parent
                                anchors.margins: 5                                
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

                                    source: keyTab.keyModel? Editor.getEditorByTypeString(keyType, false) : ""

                                    function loadRowValue(row) {
                                        console.log("loading row value", row)
                                        if (valueEditor.item) {
                                            var rowValue = keyTab.keyModel.getRow(row)
                                            valueEditor.currentRow = row
                                            valueEditor.item.reset()
                                            valueEditor.item.defaultFormatter = defaultFormatter
                                            valueEditor.item.setValue(rowValue)
                                        } else {
                                            console.log("cannot load row value - item is missing")
                                        }
                                    }

                                    function clear() {
                                        if (valueEditor.item) {
                                            currentRow = -1
                                            valueEditor.item.keyType = Qt.binding(function() { return keyType });
                                            valueEditor.item.reset()
                                        }
                                    }

                                    onLoaded: clear()
                                }
                            }
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
