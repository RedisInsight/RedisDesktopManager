import QtQuick 2.3
import QtQuick.Layouts 1.1
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.1
import QtQuick.Dialogs 1.2
import QtQuick.Window 2.2
import "./editors/editor.js" as Editor
import "./parts"


Repeater {

    Tab {
        id: keyTab

        function close(index) {

//            console.log(valueEditor!=undefined)

//            if (valueEditor != undefined && valueEditor.item && valueEditor.item.isValueChanged()) {
//                // TODO: show "Unsaved changes detected" warnings
//                return
//            }

            viewModel.closeTab(tabIndex)
        }       

        title: keyName
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

        ColumnLayout {
            anchors.fill: parent
            anchors.margins: 5
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

                Item { Layout.preferredWidth: 10}
                Text { text: "TTL:"; font.bold: true }
                Text { text: keyTtl}
                Item { Layout.preferredWidth: 10}

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
                    }
                }

                Button {
                    text: "Delete"

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
                    }
                }

                Button {
                    text: "Reload Value"
                    action: reLoadAction
                    visible: !showValueNavigation
                }
            }

            RowLayout {
                Layout.fillWidth: true
                Layout.preferredHeight: 40
                Layout.minimumHeight: 40
                visible: showValueNavigation

                Text {
                    Layout.fillWidth: true                    
                    textFormat: Text.RichText
                    text: "<b>Note:</b> Double click on row or press ENTER on selected row to activate editing"
                    color: "#cccccc"
                }

                Item { Layout.fillWidth: true}

                Button {
                    text: "Reload Value"
                    action: reLoadAction                    
                    Action {
                        id: reLoadAction                        
                        shortcut: StandardKey.Refresh
                        onTriggered: {
                            console.log("Reload")
                            table.model.reload()
                        }
                    }
                }

                Item { Layout.preferredWidth: 15}

                Button {
                    text: "Add row";

                    onClicked: addRowDialog.open()

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
                    text: "Delete row"                                                                                
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
            }          

            TableView {
                id: table
                visible: showValueNavigation
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

                function goToFirstPage() {
                    console.log('goto first page')
                    goToPage(1)
                }

                function goToLastPage() {
                    console.log('goto last page')
                    goToPage(table.totalPages)
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
                        column.visible = true
                    }

                    if (table.model.isPartialLoadingSupported()
                            || table.model.totalRowCount() < maxItemsOnPage
                            || table.forceLoading) {
                        table.model.loadRows(currentStart, maxItemsOnPage)
                    } else {
                        // Legacy redis without SCAN support
                        // Show warning message
                        // to get upprove from user
                        valueLoadingConfirmation.open()
                    }

                    // TODO: show loader with fadeout
                }
            }

            Pagination {
                id: pagination
                visible: showValueNavigation
                Layout.fillWidth: true
                Layout.preferredHeight: 40
                Layout.minimumHeight: 40
            }

            Item {
                Layout.fillWidth: true
                Layout.preferredHeight: 10
            }

            Rectangle {
                color: "#cccccc"
                Layout.fillWidth: true
                Layout.preferredHeight: 1
            }

            Item {
                Layout.fillWidth: true
                Layout.preferredHeight: 10
            }

            ColumnLayout {
                Layout.fillWidth: true
                Layout.fillHeight: !showValueNavigation
                Layout.minimumHeight: 250

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

                    property int currentRow: -1

                    source: {
                        if (keyType === "string")
                            table.loadValue()                                                    

                        return Editor.getEditorByTypeString(keyType)
                    }

                    onLoaded: {
                        if (keyType === "string")
                            valueEditor.loadRowValue(0)
                    }

                    function loadRowValue(row) {
                        if (valueEditor.item) {
                            var rowValue = table.model.getRow(row, true)
                            valueEditor.currentRow = row
                            valueEditor.item.setValue(rowValue)
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
        }
    }
}

