import QtQuick 2.3
import QtQuick.Layouts 1.1
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.1
import QtQuick.Dialogs 1.2
import QtQuick.Window 2.2
import MeasurementProtocol 1.0
import "./editors/editor.js" as Editor
import "./../common"
import rdm.models 1.0

Repeater {

    BetterTab {
        id: keyTab
        width: approot.width
        height: approot.height
        icon: "qrc:/images/key.svg"

        closable: true
        onClose: {
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
        property var valueEditor
        property var searchModel

        property variant keyModel: keyName ? viewModel.getValue(tabIndex) : null

        onKeyModelChanged: {
            // On tab reload
            if (keyModel && table) {
                table.forceLoading = false
                table.currentStart = 0
                table.searchField.text = ""

                if (valueEditor.item)
                    valueEditor.item.resetAndDisableEditor()                

                table.loadValue()
            }
        }

        property Component searchModelComponent: Component {
            SortFilterProxyModel {
                source: keyTab.keyModel
                sortOrder: table.sortIndicatorOrder
                sortCaseSensitivity: Qt.CaseInsensitive
                sortRole: keyTab.keyModel ? table.getColumn(table.sortIndicatorColumn).role : ""

                filterString: "*" + table.searchField.text + "*"
                filterSyntax: SortFilterProxyModel.Wildcard
                filterCaseSensitivity: Qt.CaseInsensitive
                filterRole: keyTab.keyModel ? table.getColumn(1).role : ""
            }
        }

        Keys.onPressed: {
            if (!keyModel)
                return

            var reloadKey = event.key == Qt.Key_F5
                    || (event.key == Qt.Key_R && (event.modifiers & Qt.ControlModifier))
                    || (event.key == Qt.Key_R && (event.modifiers & Qt.MetaModifier))

            if (reloadKey) {
                console.log("Reload")
                keyModel.reload()
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
                    Text { visible: showValueNavigation; text: "Size: "+ valuesCount }
                    Item { Layout.preferredWidth: 5}
                    Text { text: qsTr("TTL:"); font.bold: true }
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

                                Text { text: qsTr("New name:") }
                                TextField {
                                    id: newKeyName;
                                    Layout.fillWidth: true;
                                }
                            }

                            onAccepted: {
                                if (newKeyName.text.length == 0) {
                                    return open()
                                }

                                viewModel.renameKey(keyTab.tabIndex, newKeyName.text)
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
                        text: qsTr("Delete")
                        iconSource: "qrc:/images/delete.svg"

                        MessageDialog {
                            id: deleteConfirmation
                            title: qsTr("Delete key")
                            text: qsTr("Do you really want to delete this key?")
                            onYes: {
                                console.log("remove key")
                                viewModel.removeKey(keyTab.tabIndex)
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
                        text: qsTr("Reload Value")
                        action: reLoadAction
                        visible: !showValueNavigation
                    }

                    Button {
                        text: qsTr("Set TTL")
                        Dialog {
                            id: setTTLConfirmation
                            title: qsTr("Set key TTL")

                            width: 520

                            RowLayout {
                                implicitWidth: 500
                                implicitHeight: 100
                                width: 500

                                Text { text: qsTr("New TTL:") }
                                TextField {
                                    id: newTTL;
                                    Layout.fillWidth: true;
                                }
                            }

                            onAccepted: {
                                if (newTTL.text.length == 0) {
                                    return open()
                                }

                                viewModel.setTTL(keyTab.tabIndex, newTTL.text)
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

                            property var searchField

                            Layout.fillWidth: true
                            Layout.fillHeight: true
                            Layout.minimumHeight: 100

                            sortIndicatorVisible: true

                            TableViewColumn{ width: 50 }
                            TableViewColumn{ width: 150 }
                            TableViewColumn{ width: table.width - 200}                            

                            model: searchModel ? searchModel : null

                            property int currentStart: 0
                            property int maxItemsOnPage: keyTab.keyModel ? keyTab.keyModel.pageSize() : 100
                            property int currentPage: currentStart / maxItemsOnPage + 1
                            property int totalPages: Math.ceil(keyTab.keyModel.totalRowCount() / maxItemsOnPage)
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

                                        if (!styleData.value || keyType === "string") {
                                            return ""
                                        }

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

                            MessageDialog {
                                id: valueErrorNotification
                                visible: false
                                modality: Qt.ApplicationModal
                                icon: StandardIcon.Warning
                                standardButtons: StandardButton.Ok
                            }

                            Connections {
                                id: keyModelConnections

                                onError: {
                                    valueErrorNotification.text = error
                                    valueErrorNotification.open()
                                }

                                onRowsLoaded: {
                                    console.log("rows loaded")

                                    wrapper.hideLoader()

                                    keyTab.searchModel = keyTab.searchModelComponent.createObject(keyTab)

                                    console.log(keyType)

                                    if (keyType === "string") {
                                        valueEditor.loadRowValue(0)
                                    } else {
                                        var columns = columnNames

                                        for (var index = 0; index < 3; index++)
                                        {
                                            var column = table.getColumn(index)

                                            if (index >= columns.length) {
                                                column.visible = false
                                                continue
                                            }

                                            column.role = columns[index]
                                            column.title = columns[index]
                                            //if (index > 0) column.width = table.width / (columns.length - 1) - 50
                                            column.visible = true
                                            column.resizeToContents()
                                        }
                                    }
                                }
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
                                console.log("Load value")
                                if (!keyTab.keyModel) {
                                    console.log("Model is not ready")
                                    return
                                }

                                keyModelConnections.target = keyTab.keyModel
                                wrapper.showLoader()
                                keyTab.keyModel.loadRows(currentStart, maxItemsOnPage)
                            }

                            onRowCountChanged: wrapper.hideLoader()
                            onActivated: valueEditor.loadRowValue(table.model.getOriginalRowIndex(row))
                            onClicked: valueEditor.loadRowValue(table.model.getOriginalRowIndex(row))
                        }

                        ColumnLayout {
                            Layout.fillHeight: true
                            Layout.preferredWidth: 200
                            Layout.maximumWidth: 200
                            Layout.alignment: Qt.AlignTop

                            Button {
                                Layout.preferredWidth: 195
                                text: qsTr("Add Row");
                                iconSource: "qrc:/images/add.svg"
                                onClicked: {
                                    addRowDialog.open()

                                    Analytics.reportEvent("value-editor", "add-row")
                                }

                                Dialog {
                                    id: addRowDialog
                                    title: qsTr("Add Row")

                                    width: 550
                                    height: 400
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
                                        keyTab.keyModel.reload()
                                        valueAddEditor.item.reset()
                                    }

                                    visible: false
                                    standardButtons: StandardButton.Ok | StandardButton.Cancel
                                }
                            }

                            Button {
                                Layout.preferredWidth: 195
                                text: qsTr("Delete row")
                                iconSource: "qrc:/images/delete.svg"
                                enabled: table.currentRow != -1

                                onClicked: {
                                    if (keyTab.keyModel.totalRowCount() == 1) {
                                        deleteRowConfirmation.text = qsTr("The row is the last one in the key. After removing it key will be deleted.")
                                    } else {
                                        deleteRowConfirmation.text = qsTr("Do you really want to remove this row?")
                                    }

                                    var rowIndex = table.model.getOriginalRowIndex(table.currentRow)
                                    console.log("Original row index in model:", rowIndex)

                                    deleteRowConfirmation.rowToDelete = rowIndex
                                    deleteRowConfirmation.open()

                                    Analytics.reportEvent("value-editor", "delete-row")
                                }

                                MessageDialog {
                                    id: deleteRowConfirmation
                                    title: qsTr("Delete row")
                                    text: ""
                                    onYes: {
                                        console.log("remove row in key")
                                        keyTab.keyModel.deleteRow(rowToDelete)
                                    }
                                    visible: false
                                    modality: Qt.ApplicationModal
                                    icon: StandardIcon.Warning
                                    standardButtons: StandardButton.Yes | StandardButton.No
                                    property int rowToDelete
                                }

                            }

                            Button {
                                Layout.preferredWidth: 195
                                text: qsTr("Reload Value")
                                iconSource: "qrc:/images/refresh.svg"
                                action: reLoadAction

                                Action {
                                    id: reLoadAction
                                    shortcut: StandardKey.Refresh
                                    onTriggered: {
                                        console.log("Reload value in tab")
                                        keyTab.keyModel.reload()
                                        valueEditor.clear()

                                        Analytics.reportEvent("value-editor", "reload-key")
                                    }
                                }
                            }

                            TextField {
                                id: searchField

                                Layout.preferredWidth: 195
                                placeholderText: qsTr("Search on page...")

                                Component.onCompleted: {
                                    table.searchField = searchField
                                }
                            }

                            Item {
                                Layout.fillWidth: true
                                Layout.fillHeight: true
                            }

                            Pagination {
                                id: pagination
                                Layout.maximumWidth: 150
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

                            Component.onCompleted: {
                                keyTab.valueEditor = valueEditor
                            }

                            property int currentRow: -1

                            source: Editor.getEditorByTypeString(keyType)

                            onLoaded: {
                                if (valueEditor.item)
                                    valueEditor.item.resetAndDisableEditor()
                            }

                            function loadRowValue(row) {                                
                                if (valueEditor.item) {
                                    var rowValue = keyTab.keyModel.getRow(row, true)

// TODO: Show dialog here with options:  View in read-only mode, Save to file, Ignore warning
//                                    if (binaryUtils.binaryStringLength(rowValue['value']) > 150000) {
//                                        console.log("Extra large value")
//                                        return
//                                    }

                                    valueEditor.currentRow = row
                                    valueEditor.item.setValue(rowValue)
                                } else {
                                    console.log("cannot load row value - item is missing")
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
                                text: qsTr("Save")

                                onClicked: {
                                    if (!valueEditor.item || !valueEditor.item.isValueChanged()) {
                                        savingConfirmation.text = qsTr("Nothing to save")
                                        savingConfirmation.open()
                                        return
                                    }

                                    var value = valueEditor.item.getValue()

                                    console.log(value, value["value"])
                                    keyTab.keyModel.updateRow(valueEditor.currentRow, value)

                                    savingConfirmation.text = qsTr("Value was updated!")
                                    savingConfirmation.open()
                                }

                            }

                            MessageDialog {
                                id: savingConfirmation
                                title: qsTr("Save value")
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

