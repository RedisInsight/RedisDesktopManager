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

RowLayout {

    Layout.fillWidth: true
    Layout.fillHeight: false
    Layout.bottomMargin: 10
    SplitView.minimumHeight: 250
    visible: keyModel? isMultiRow : false

    ColumnLayout {
        id: tableLayout
        Layout.fillWidth: true
        Layout.fillHeight: true
        Layout.minimumHeight: 100

        RowLayout {
            Layout.minimumHeight: 15
            Layout.preferredHeight: 30
            Layout.fillWidth: true
            spacing: 1

            Repeater {
                id: tableHeader
                model: keyTab.keyModel? keyTab.keyModel.columnNames : []

                Rectangle {  // Table header cell
                    Layout.preferredHeight: 30
                    Layout.preferredWidth: 75
                    Layout.fillWidth: index !== 0
                    color: sysPalette.window

                    BetterLabel {
                        anchors.centerIn: parent
                        text: modelData
                        color: sysPalette.windowText
                    }

                    BetterLabel {  // Sort indicator
                        anchors.margins: 10
                        anchors.right: parent.right
                        anchors.verticalCenter: parent.verticalCenter
                        font.pointSize: 8
                        text: "▲"
                        color: sysPalette.mid
                        visible: false
                    }

                    MouseArea {
                        anchors.fill: parent

                        onClicked: {
                            var role = tableHeader.model[index]
                            var order = (role == table.model.sortRole) ? 1 - table.model.sortOrder : Qt.AscendingOrder

                            for (var i = 0; i < tableHeader.model.length; i++) {
                                tableHeader.itemAt(i).children[1].visible = false
                            }
                            tableHeader.itemAt(index).children[1].text = (order === Qt.AscendingOrder) ? "▲" : "▼"
                            tableHeader.itemAt(index).children[1].visible = true

                            table.sort(role, order)
                        }
                    }
                }  // Table header cell end
            }
        }

        Item {
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.minimumHeight: 100

            ScrollView {
                id: tableScrollView
                anchors.fill: parent

                ScrollBar.horizontal.policy: ScrollBar.AlwaysOff
                ScrollBar.vertical.policy: ScrollBar.AlwaysOn


                TableView {
                    id: table
                    objectName: "rdm_value_tab_table"

                    focus: true

                    width: parent.width
                    onWidthChanged: forceLayout()
                    clip: true
                    columnSpacing: 1
                    rowSpacing: 1
                    reuseItems: false
                    model: searchModel ? searchModel : null

                    property int currentRow: -1
                    property var searchField
                    property var currentStart: 0
                    property int maxItemsOnPage: keyTab.keyModel ? keyTab.keyModel.pageSize : 100
                    property int currentPage: currentStart / maxItemsOnPage + 1
                    property int totalPages: keyTab.keyModel ? Math.ceil(keyTab.keyModel.totalRowCount / maxItemsOnPage) : 0
                    property bool forceLoading: false
                    property int firstColumnWidth: 75
                    property int valueColumnWidth:  keyTab.keyModel && keyTab.keyModel.columnNames.length == 2? tableLayout.width - table.firstColumnWidth - table.columnSpacing
                                                                                                              : (tableLayout.width - table.firstColumnWidth - table.columnSpacing) / 2
                    Component.onCompleted: keyTab.table = table

                    delegate: DelegateChooser {

                        DelegateChoice {
                            column: 0

                            ValueTableCell {
                                objectName: "rdm_value_table_cell_col1"
                                implicitWidth: table.firstColumnWidth
                                implicitHeight: 30
                                text: Number(row) + 1
                                selected: table.currentRow === row
                                onClicked: {
                                    table.currentRow = row
                                    table.forceActiveFocus()
                                }
                            }
                        }

                        DelegateChoice {
                            column: 1

                            ValueTableCell {
                                objectName: "rdm_value_table_cell_col2"
                                implicitWidth: table.valueColumnWidth
                                implicitHeight: 30
                                text: renderText(display)
                                selected: table.currentRow === row
                                onClicked: {
                                    table.currentRow = row
                                    table.forceActiveFocus()
                                }
                            }
                        }

                        DelegateChoice {
                            column: 2

                            ValueTableCell {
                                objectName: "rdm_value_table_cell_col3"
                                implicitWidth: table.valueColumnWidth
                                implicitHeight: 30

                                selected: table.currentRow === row
                                onClicked: {
                                    table.currentRow = row
                                    table.forceActiveFocus()
                                }

                                text: {
                                    if (display === "" || !isMultiRow) {
                                        return ""
                                    }

                                    if (keyType == "zset") {
                                        return Number(display)
                                    }

                                    return renderText(display)
                                }
                            }
                        }
                    }

                    OkDialog {
                        id: valueErrorNotification
                        visible: false
                    }

                    Connections {
                        id: keyModelConnections
                        ignoreUnknownSignals: true

                        target: keyTab.keyModel ? keyTab.keyModel : null

                        onError: {
                            valueErrorNotification.text = error
                            valueErrorNotification.open()
                        }

                        onIsLoadedChanged: {
                            console.log("model loaded (qml)")
                            if (keyTab.keyModel.totalRowCount === 0) {
                                console.log("Load rows count")
                                keyTab.keyModel.loadRowsCount()
                            } else {
                                console.log("Load rows")
                                keyTab.keyModel.loadRows(currentStart, maxItemsOnPage)
                            }
                        }

                        onTotalRowCountChanged: {
                            keyTab.keyModel.loadRows(table.currentStart, table.maxItemsOnPage)
                        }

                        onRowsLoaded: {
                            console.log("rows loaded")

                            wrapper.hideLoader()
                            keyTab.searchModel = keyTab.searchModelComponent.createObject(keyTab)

                            if (isMultiRow) {
                                valueEditor.clear()
                            } else {
                                valueEditor.loadRowValue(0)
                            }

                            table.forceLayout()
                        }
                    }

                    function goToPage(page) {
                        var firstItemOnPage = table.maxItemsOnPage * (page - 1)

                        if (table.currentStart === firstItemOnPage)
                            return

                        table.currentStart = firstItemOnPage
                        resetCurrentRow()
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
                            console.log("Model is not ready", keyViewModel)
                            return
                        }
                        wrapper.showLoader()
                        if (isMultiRow && keyTab.keyModel.totalRowCount === 0) {
                            console.log("Load rows count")
                            keyTab.keyModel.loadRowsCount()
                        } else {
                            console.log("Load rows")
                            keyTab.keyModel.loadRows(currentStart, maxItemsOnPage)
                        }
                    }

                    function resetCurrentRow() {
                        table.currentRow = -1
                    }

                    function sort(role, order) {
                        table.model.setSortRole(role)
                        table.model.setSortOrder(order)
                    }

                    onRowsChanged: wrapper.hideLoader()
                    onCurrentRowChanged: {
                        console.log("Current row in table changed: ", currentRow)
                        if (currentRow >= 0) {
                            valueEditor.loadRowValue(currentRow)
                        }
                    }
                }
            }
        }
    }

    ValueTableActions {}
}
