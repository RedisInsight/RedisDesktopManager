import QtQuick 2.3
import QtQuick.Layouts 1.1
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.1
import QtQuick.Dialogs 1.2
import QtQuick.Window 2.2
import MeasurementProtocol 1.0
import "./editors/editor.js" as Editor
import "./parts"


Repeater {

    Tab {
        id: keyTab
        width: approot.width
        height: approot.height

        function close(index) {

            //            console.log(valueEditor!=undefined)

            //            if (valueEditor != undefined && valueEditor.item && valueEditor.item.isValueChanged()) {
            //                // TODO: show "Unsaved changes detected" warnings
            //                return
            //            }

            viewModel.closeTab(tabIndex)
        }

        title: {
            var limit = 30
            if (keyTitle.length <= limit)
                return keyTitle

            return keyTitle.substring(0, limit/2) + "..." + keyTitle.substring(keyTitle.length - limit/2)

        }
        property int tabIndex: keyIndex
        property var table

        Keys.onPressed: {
            if (!table)
                return

            var reloadKey = event.key == Qt.Key_F5
                    || (event.key == Qt.Key_R && (event.modifiers & Qt.ControlModifier))
                    || (event.key == Qt.Key_R && (event.modifiers & Qt.MetaModifier))

            if (reloadKey) {
                console.log("Reload")
                table.model.reload()
            }
        }

        Component.onCompleted: {
            keyTab.focus = true
            keyTab.forceActiveFocus()            
        }

        Item {
            id: wrapper
            anchors.fill: parent
            anchors.margins: 5

            function showLoader() {
                uiBlocker.visible = true
            }

            function hideLoader() {
                uiBlocker.visible = false
            }

            ColumnLayout {
                anchors.fill: parent
                spacing: 1

                RowLayout {
                    Layout.preferredHeight: 40
                    Layout.minimumHeight: 40
                    Layout.fillWidth: true
                    spacing: 5

                    Text { text: keyType.toUpperCase() + ":"; font.bold: true }

                    TextField {
                        id: keyNameField
                        Layout.fillWidth: true
                        text: keyName
                        readOnly: true
                    }

                    Item { visible: showValueNavigation; Layout.preferredWidth: 5}
                    Text { visible: showValueNavigation; text: "Size: "+ table.model.totalRowCount() }
                    Item { Layout.preferredWidth: 5}
                    Text { text: "TTL:"; font.bold: true }
                    Text { text: keyTtl}
                    Item { Layout.preferredWidth: 5}

                    Button {
                        text: "Rename"

                        Dialog {
                            id: renameConfirmation
                            title: "Rename key"

                            width: 520

                            RowLayout {
                                implicitWidth: 500
                                implicitHeight: 100
                                width: 500

                                Text { text: "New name:" }
                                TextField {
                                    id: newKeyName;
                                    Layout.fillWidth: true;
                                }
                            }

                            onAccepted: {
                                if (newKeyName.text.length == 0) {
                                    return open()
                                }

                                viewModel.renameKey(keyTab.keyIndex, newKeyName.text)
                            }

                            visible: false
                            modality: Qt.ApplicationModal
                            standardButtons: StandardButton.Ok | StandardButton.Cancel
                        }

                        onClicked: {
                            newKeyName.text = keyNameField.text
                            renameConfirmation.open()

                            Analytics.reportEvent("value-editor", "rename-key")
                        }
                    }

                    Button {
                        text: "Delete"
                        iconSource: "qrc:/images/delete.png"

                        MessageDialog {
                            id: deleteConfirmation
                            title: "Delete key"
                            text: "Do you really want to delete this key?"
                            onYes: {
                                console.log("remove key")
                                viewModel.removeKey(keyTab.keyIndex)
                            }
                            visible: false
                            modality: Qt.ApplicationModal
                            icon: StandardIcon.Warning
                            standardButtons: StandardButton.Yes | StandardButton.No
                        }

                        onClicked: {
                            deleteConfirmation.open()

                            Analytics.reportEvent("value-editor", "delete-key")
                        }
                    }

                    Button {
                        text: "Reload Value"
                        action: reLoadAction
                        visible: !showValueNavigation
                    }

                    Button {
                        text: "Set TTL"
                        Dialog {
                            id: setTTLConfirmation
                            title: "Set key TTL"

                            width: 520

                            RowLayout {
                                implicitWidth: 500
                                implicitHeight: 100
                                width: 500

                                Text { text: "New TTL:" }
                                TextField {
                                    id: newTTL;
                                    Layout.fillWidth: true;
                                }
                            }

                            onAccepted: {
                                if (newTTL.text.length == 0) {
                                    return open()
                                }

                                viewModel.setTTL(keyTab.keyIndex, newTTL.text)
                            }

                            visible: false
                            modality: Qt.ApplicationModal
                            standardButtons: StandardButton.Ok | StandardButton.Cancel
                        }

                        onClicked: {
                            newTTL.text = ""+keyTtl
                            setTTLConfirmation.open()

                            Analytics.reportEvent("value-editor", "set-key-ttl")
                        }
                    }
                }

                SplitView {
                    orientation: Qt.Vertical
                    Layout.fillHeight: true
                    Layout.fillWidth: true

                    handleDelegate: ColumnLayout {
                        Item { Layout.fillWidth: true; Layout.preferredHeight: 2; }
                        Rectangle { color: "#e2e2e2"; Layout.fillWidth: true; Layout.preferredHeight: 1; }
                        Item { Layout.fillWidth: true; Layout.preferredHeight: 2;}
                    }

                    onResizingChanged: {
                        if (resizing) {
                            valueEditor.maxHeight = 9999
                        }
                    }

                    // Table
                    RowLayout {
                        id: navigationTable
                        Layout.fillWidth: true
                        Layout.fillHeight: showValueNavigation
                        visible: showValueNavigation

                        TableView {
                            id: table
                            Layout.fillWidth: true
                            Layout.fillHeight: true
                            Layout.minimumHeight: 100

                            TableViewColumn{ width: 50 }
                            TableViewColumn{ width: 150 }
                            TableViewColumn{ width: table.width - 200}

                            model: viewModel.getValue(tabIndex)

                            property int currentStart: 0
                            property int maxItemsOnPage: model? model.pageSize() : 100
                            property int currentPage: currentStart / maxItemsOnPage + 1
                            property int totalPages: Math.ceil(table.model.totalRowCount() / maxItemsOnPage)
                            property bool forceLoading: false

                            Component.onCompleted: {
                                keyTab.table = table
                                loadValue()
                            }

                            itemDelegate: Item {
                                Text {
                                    anchors.fill: parent
                                    color: styleData.textColor
                                    elide: styleData.elideMode
                                    text: {
                                        if (!styleData.value)
                                            return ""

                                        if (styleData.column === 2 && keyType == "zset") {
                                            return parseFloat(Number(styleData.value).toFixed(20))
                                        }

                                        return binaryUtils.printable(styleData.value)
                                                            + (lineCount > 1 ? '...' : '')
                                    }
                                    wrapMode: Text.WrapAnywhere
                                    maximumLineCount: 1
                                }
                            }

                            Connections {
                                target: viewModel

                                onReplaceTab: {
                                    console.log("replace tab")
                                    table.model = viewModel.getValue(tabIndex)
                                    table.forceLoading = false
                                    table.currentStart = 0

                                    if (valueEditor.item)
                                        valueEditor.item.resetAndDisableEditor()

                                    table.loadValue()

                                    if (keyType === "string") {
                                        valueEditor.loadRowValue(0)
                                    }
                                }
                            }

                            MessageDialog {
                                id: valueErrorNotification
                                visible: false
                                modality: Qt.ApplicationModal
                                icon: StandardIcon.Warning
                                standardButtons: StandardButton.Ok
                            }

                            Connections {
                                target: table.model ? table.model : null

                                onError: {
                                    valueErrorNotification.text = error
                                    valueErrorNotification.open()
                                }

                                onRowsLoaded: {
                                    wrapper.hideLoader()
                                }
                            }

                            MessageDialog {
                                id: valueLoadingConfirmation
                                title: "Legacy Redis-Server detected!"
                                text: "You are connected to legacy redis-server, which doesn't support partial loading. "
                                      + "Do you really want to load " + table.model.totalRowCount() +" items?"
                                onYes: {
                                    table.forceLoading = true
                                    table.loadValue()
                                }
                                visible: false
                                modality: Qt.ApplicationModal
                                icon: StandardIcon.Warning
                                standardButtons: StandardButton.Yes | StandardButton.No
                            }

                            function goToPage(page) {
                                var firstItemOnPage = table.maxItemsOnPage * (page - 1)

                                if (table.currentStart === firstItemOnPage)
                                    return

                                table.currentStart = firstItemOnPage
                                loadValue()
                            }

                            function goToPrevPage() {
                                console.log('goto prev page')
                                if (table.currentPage - 1 < 1)
                                    return

                                goToPage(table.currentPage - 1)
                            }

                            function goToNextPage() {
                                console.log('goto next page')
                                if (table.totalPages < table.currentPage + 1)
                                    return

                                goToPage(table.currentPage + 1)
                            }

                            function loadValue() {
                                var columns = table.model.getColumnNames()

                                for (var index = 0; index < 3; index++)
                                {
                                    var column = table.getColumn(index)

                                    if (index >= columns.length) {
                                        column.visible = false
                                        continue
                                    }

                                    column.role = columns[index]
                                    column.title = columns[index]
                                    if (index > 0) column.width = table.width / (columns.length - 1) - 50
                                    column.visible = true
                                }

                                if (table.model.isPartialLoadingSupported()
                                        || table.model.totalRowCount() < maxItemsOnPage
                                        || table.forceLoading) {
                                    if (keyType != "string")
                                        wrapper.showLoader()
                                    table.model.loadRows(currentStart, maxItemsOnPage)
                                } else {
                                    // Legacy redis without SCAN support
                                    // Show warning message
                                    // to get upprove from user
                                    valueLoadingConfirmation.open()
                                }
                            }

                            onRowCountChanged: {
                                wrapper.hideLoader()
                            }
                        }

                        ColumnLayout {
                            Layout.fillHeight: true
                            Layout.preferredWidth: 150
                            Layout.maximumWidth: 150
                            Layout.alignment: Qt.AlignTop

                            Button {
                                Layout.preferredWidth: 150
                                text: "Add row";
                                iconSource: "qrc:/images/add.png"
                                onClicked: {
                                    addRowDialog.open()

                                    Analytics.reportEvent("value-editor", "add-row")
                                }

                                Dialog {
                                    id: addRowDialog
                                    title: "Add Row"

                                    width: 520
                                    height: 300
                                    modality: Qt.ApplicationModal

                                    Loader {
                                        id: valueAddEditor
                                        width: 500
                                        height: 350
                                        anchors.centerIn: parent
                                        property int currentRow: -1

                                        source: Editor.getEditorByTypeString(keyType)

                                        onLoaded: {
                                            item.state = "add"
                                        }
                                    }

                                    onAccepted: {
                                        if (!valueAddEditor.item)
                                            return false

                                        if (!valueAddEditor.item.isValueValid()) {
                                            valueAddEditor.item.markInvalidFields()
                                            return open()
                                        }

                                        var row = valueAddEditor.item.getValue()

                                        var model = viewModel.getValue(tabIndex)
                                        model.addRow(row)
                                        table.model.reload()
                                        valueAddEditor.item.reset()
                                    }

                                    visible: false
                                    standardButtons: StandardButton.Ok | StandardButton.Cancel
                                }
                            }

                            Button {
                                Layout.preferredWidth: 150
                                text: "Delete row"
                                iconSource: "qrc:/images/delete.png"
                                enabled: table.currentRow != -1

                                onClicked: {
                                    if (table.model.totalRowCount() == 1) {
                                        deleteRowConfirmation.text = "This is last row in this key, " +
                                                "if you remove this - key will be removed!"
                                    } else {
                                        deleteRowConfirmation.text = "Do you relly want to remove this row?"
                                    }
                                    deleteRowConfirmation.rowToDelete = table.currentRow
                                    deleteRowConfirmation.open()

                                    Analytics.reportEvent("value-editor", "delete-row")
                                }

                                MessageDialog {
                                    id: deleteRowConfirmation
                                    title: "Delete row"
                                    text: ""
                                    onYes: {
                                        console.log("remove row in key")
                                        table.model.deleteRow(rowToDelete)
                                    }
                                    visible: false
                                    modality: Qt.ApplicationModal
                                    icon: StandardIcon.Warning
                                    standardButtons: StandardButton.Yes | StandardButton.No
                                    property int rowToDelete
                                }

                            }

                            Button {
                                Layout.preferredWidth: 150
                                text: "Reload Value"
                                iconSource: "qrc:/images/refreshdb.png"
                                action: reLoadAction

                                Action {
                                    id: reLoadAction
                                    shortcut: StandardKey.Refresh
                                    onTriggered: {
                                        console.log("Reload value in tab")
                                        table.model.reload()
                                        valueEditor.clear()

                                        Analytics.reportEvent("value-editor", "reload-key")
                                    }
                                }
                            }

                            Item {
                                Layout.fillWidth: true
                                Layout.fillHeight: true
                            }

                            Pagination {
                                id: pagination
                                visible: showValueNavigation
                            }
                        }
                    }
                    // Table end

                    //Value editor
                    ColumnLayout {
                        id: editorWrapper
                        Layout.fillWidth: true
                        Layout.fillHeight: !showValueNavigation
                        spacing: 0

                        Connections {
                            target: table

                            onActivated: {
                                valueEditor.loadRowValue(row)
                            }
                        }

                        Loader {
                            id: valueEditor
                            Layout.fillWidth: true                            
                            Layout.fillHeight: true
                            Layout.minimumHeight: 180

                            Layout.maximumHeight: maxHeight
                            property int maxHeight: 9999

                            Connections {
                                target: viewModel

                                onReplaceTab: {
                                    if (showValueNavigation && keyIndex === index) {
                                        valueEditor.maxHeight = wrapper.height * 0.4
                                    } else {
                                        valueEditor.maxHeight = 9999
                                    }
                                }
                            }

                            property int currentRow: -1

                            source: {
                                if (keyType === "string") {
                                    table.loadValue()
                                }

                                return Editor.getEditorByTypeString(keyType)
                            }

                            onLoaded: {
                                if (valueEditor.item)
                                    valueEditor.item.resetAndDisableEditor()
                                if (keyType === "string") {
                                    valueEditor.loadRowValue(0)
                                }
                            }

                            function loadRowValue(row) {
                                if (valueEditor.item) {
                                    var rowValue = table.model.getRow(row, true)
                                    valueEditor.currentRow = row
                                    valueEditor.item.setValue(rowValue)
                                }
                            }

                            function clear() {
                                if (valueEditor.item) {
                                    currentRow = -1
                                    valueEditor.item.resetAndDisableEditor()
                                }
                            }
                        }

                        RowLayout {
                            Layout.fillWidth: true
                            Layout.minimumHeight: 40
                            Item { Layout.fillWidth: true}
                            Button {
                                text: "Save"

                                onClicked: {
                                    if (!valueEditor.item || !valueEditor.item.isValueChanged()) {
                                        savingConfirmation.text = "Nothing to save"
                                        savingConfirmation.open()
                                        return
                                    }

                                    var value = valueEditor.item.getValue()

                                    console.log(value, value["value"])
                                    table.model.updateRow(valueEditor.currentRow, value)

                                    savingConfirmation.text = "Value was updated!"
                                    savingConfirmation.open()

                                    Analytics.reportEvent("value-editor", "update-row")
                                }

                            }

                            MessageDialog {
                                id: savingConfirmation
                                title: "Save value"
                                text: ""
                                visible: false
                                modality: Qt.ApplicationModal
                                icon: StandardIcon.Information
                                standardButtons: StandardButton.Ok
                            }
                        }
                    }
                    //Value editor end
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
        }
    }
}

