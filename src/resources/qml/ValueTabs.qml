import QtQuick 2.0
import QtQuick.Layouts 1.1
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.1
import QtQuick.Dialogs 1.2

Repeater {

    Tab {
        id: keyTab

        function close(index) {

            if (valueEditor.item && valueEditor.item.isValueChanged()) {
                // TODO: show "Unsaved changes detected" warnings
                return
            }

            viewModel.closeTab(tabIndex)
        }

        title: keyName
        property int tabIndex: keyIndex

        ColumnLayout {
            anchors.fill: parent
            anchors.margins: 5
            spacing: 1

            RowLayout {
                Layout.preferredHeight: 40
                Layout.minimumHeight: 40
                Layout.fillWidth: true
                spacing: 1

                Text { text: "Key:" }
                TextField {
                    id: keyNameField
                    Layout.fillWidth: true
                    text: keyName
                    readOnly: true                    
                }

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
                            TextField { id: newKeyName; Layout.fillWidth: true;}
                        }

                        onAccepted: {
                            console.log(newKeyName.text)
                            viewModel.renameKey(keyTab.keyIndex, newKeyName.text)
                        }

                        visible: false
                        modality: Qt.WindowModal
                        standardButtons: StandardButton.Ok | StandardButton.Cancel
                    }

                    onClicked: {
                        newKeyName.text = keyNameField.text
                        renameConfirmation.open()
                    }
                }

                //Button { text: "Reload" } // TBD

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
                        modality: Qt.WindowModal
                        icon: StandardIcon.Warning
                        standardButtons: StandardButton.Yes | StandardButton.No
                    }

                    onClicked: {
                        deleteConfirmation.open()
                    }
                }
            }

            RowLayout {
                Layout.fillWidth: true
                Layout.preferredHeight: 40
                Layout.minimumHeight: 40

                Text { text: "Type:"; font.bold: true }
                Text { text: keyType.toUpperCase()  }
                Text { text: "TTL:"; font.bold: true }
                Text { text: keyTtl}

                Item { Layout.fillWidth: true}

                Button { text: "Add row"; visible: showValueNavigation}
                Button {
                    text: "Delete row"
                    visible: showValueNavigation
                    enabled: table.currentRow != -1
                }
                Button { text: "Reload"}
            }          

            TableView {
                id: table
                visible: showValueNavigation
                Layout.fillWidth: true
                Layout.fillHeight: true
                Layout.minimumHeight: 100

                TableViewColumn{ width: 30 }
                TableViewColumn{ width: 100 }
                TableViewColumn{ width: table.width - 130}

                model: viewModel.getValue(tabIndex)

                property int currentStart: 0
                property int maxItemsOnPage: 100
                property int currentPage: currentStart / maxItemsOnPage + 1
                property int totalPages: Math.ceil(table.model.totalRowCount() / maxItemsOnPage)
                property bool forceLoading: false

                Component.onCompleted: {
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
                    id: valueLoadingConfirmation
                    title: "Legacy Redis-Server detected!"
                    text: "You are connected to legacy redis-server, which doesn't support partial loading. "
                            + "Do you really want to load " + table.model.totalRowCount() +" items?"
                    onYes: {
                        table.forceLoading = true
                        table.loadValue()
                    }
                    visible: false
                    modality: Qt.WindowModal
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

            RowLayout {
                id: pagination
                visible: showValueNavigation
                Layout.fillWidth: true
                Layout.preferredHeight: 40
                Layout.minimumHeight: 40
                Button {
                    text: "⇤"
                    onClicked: table.goToFirstPage()
                }
                Button {
                    text: "⇦"
                    onClicked: table.goToPrevPage()
                }
                Text {
                    text: "Page " + table.currentPage + " of " + table.totalPages
                            + " (Items:" + (table.currentStart+1) + "-"
                            + (table.currentStart+table.maxItemsOnPage)
                            + " of " + table.model.totalRowCount() + ")"
                }
                TextField { text: "1"; Layout.fillWidth: true; readOnly: false}
                Button {
                    text: "Goto Page"
                    onClicked: {}
                }
                Button {
                    text: "⇨"
                    onClicked: table.goToNextPage()
                }
                Button {
                    text: "⇥"
                    onClicked: table.goToLastPage()
                }
            }

            ColumnLayout {
                Layout.fillWidth: true
                Layout.fillHeight: !showValueNavigation
                Layout.minimumHeight: 250

                Text {
                    Layout.fillWidth: true

                    visible: showValueNavigation

                    textFormat: Text.RichText
                    text: "<b>Note:</b> Double click on row or press ENTER on selected row to activate editing"
                }

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
                        if (keyType === "string") {
                            table.loadValue()
                            return "./editors/SingleItemEditor.qml"
                        } else if (keyType === "list" || keyType === "set") {
                            return "./editors/SingleItemEditor.qml"
                        } else if (keyType === "zset") {
                            return "./editors/SortedSetItemEditor.qml"
                        } else if (keyType === "hash") {
                            return "./editors/HashItemEditor.qml"
                        } else {
                            console.error("Editor for type " + keyType + " is not defined!")
                        }
                    }

                    onLoaded: {
                        console.log("VALUE EDITOR LOADED!")

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
                        modality: Qt.WindowModal
                        icon: StandardIcon.Warning
                        standardButtons: StandardButton.Ok
                    }
                }

            }
        }
    }
}

