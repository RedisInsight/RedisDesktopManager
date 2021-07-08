import QtQuick 2.13
import QtQuick.Layouts 1.1
import QtQuick.Controls 2.13
import QtQuick.Controls.Styles 1.1
import QtQuick.Window 2.2
import "./editors/editor.js" as Editor
import "./../common/platformutils.js" as PlatformUtils
import "./../common"
import "./filters"
import rdm.models 1.0
import Qt.labs.qmlmodels 1.0

Item {
    id: root

    property var resizeGuide: null

    RowLayout {
        anchors.fill: parent

        ColumnLayout {
            id: tableLayout
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
                        objectName: "rdm_value_tab_table_header_col" + index
                        Layout.preferredHeight: 30
                        Layout.minimumWidth: {
                            if (table.valueColumnWidthOverrides && table.valueColumnWidthOverrides[index] !== undefined) {
                                return table.valueColumnWidthOverrides[index];
                            }

                            if (index === 0)
                                return table.firstColumnWidth
                            else
                                return table.valueColumnWidth
                        }
                        color: sysPalette.window

                        BetterLabel {
                            anchors.centerIn: parent
                            text: {
                                if (modelData === "rowNumber") {
                                    return "#";
                                } else {
                                    return modelData
                                }
                            }
                            color: sysPalette.windowText
                        }

                        BetterLabel {  // Sort indicator
                            anchors.margins: 10
                            anchors.right: parent.right
                            anchors.verticalCenter: parent.verticalCenter
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


                        Rectangle {
                            id: resizeHandler

                            visible: index > 0 && index < keyTab.keyModel.columnNames.length - 1

                            color: "transparent"

                            height: parent.height
                            implicitWidth: 5

                            anchors {
                                top: parent.top
                                topMargin: 2
                                bottom: parent.bottom
                                bottomMargin: 2
                                right: parent.right
                            }

                            MouseArea {
                                id: resizeMouseArea

                                anchors.fill: parent

                                cursorShape: Qt.SizeHorCursor
                                hoverEnabled: true
                                drag {
                                    target: resizeHandler
                                    minimumX: 20
                                    smoothed: false
                                }

                                onPressed: {
                                    resizeHandler.anchors.right = undefined;
                                    if (root.resizeGuide !== null) {
                                        root.resizeGuide.destroy();
                                    }
                                    var guide = resizeMarker.createObject(resizeHandler);
                                    root.resizeGuide = guide;
                                    guide.open();
                                }

                                onReleased: {
                                    if (resizeHandler.x > 0) {
                                        table.setColumnWidth(index, resizeHandler.x);
                                    }
                                    resizeHandler.anchors.right = resizeHandler.parent.right;
                                    if (root.resizeGuide !== null) {
                                        root.resizeGuide.destroy();
                                    }
                                }
                            }

                        }

                        Component {
                            id: resizeMarker

                            Popup {
                                y: resizeHandler.y + parent.height - 6

                                height: table.height + 6
                                width: 2

                                background: Rectangle {
                                    color: sysPalette.window
                                }

                                contentItem: Item {}
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
                        clip: true
                        width: parent.width
                        onWidthChanged: forceLayout()

                        columnSpacing: 1
                        rowSpacing: 1
                        reuseItems: false
                        model: searchModel ? searchModel : null

                        // Proxy model row index from 0 to maxItemsOnPage
                        property int currentRow: -1
                        property var searchField
                        property var currentStart: 0
                        property int maxItemsOnPage: keyTab.keyModel ? keyTab.keyModel.pageSize : 100
                        property int currentPage: currentStart / maxItemsOnPage + 1
                        property int totalPages: keyTab.keyModel ? Math.ceil(keyTab.keyModel.totalRowCount / maxItemsOnPage) : 0
                        property bool forceLoading: false
                        property int firstColumnWidth: 75
                        property int valueColumnWidth:  keyTab.keyModel && keyTab.keyModel.columnNames.length == 2? root.width - 200 - table.firstColumnWidth - table.columnSpacing
                                                                                                                  : (root.width - 200 - table.firstColumnWidth - table.columnSpacing) / 2
                        property var valueColumnWidthOverrides: QtObject {}

                        Keys.onUpPressed: {
                            if (currentRow > 0) {
                                currentRow--;
                            } else {
                                currentRow = 0;
                            }
                        }

                        Keys.onDownPressed: {
                            if (currentRow < rows - 1) {
                                currentRow++;
                            } else {
                                currentRow = rows - 1;
                            }
                        }

                        columnWidthProvider: function (column) {
                            if (column === 0) {
                                return firstColumnWidth
                            }

                            if (valueColumnWidthOverrides && valueColumnWidthOverrides[column] !== undefined) {
                                return valueColumnWidthOverrides[column]
                            }

                            return valueColumnWidth
                        }

                        property int minColWidth: 50

                        function setColumnWidth(index, width) {
                            if (width < minColWidth)
                                width = minColWidth

                            if (keyTab.keyModel.columnNames.length == 2) {
                                table.valueColumnWidthOverrides[index] = width;
                            } else {
                                for (var i=1; i < 3; i++)
                                {
                                    if (i === index) {
                                        table.valueColumnWidthOverrides[i] = width;
                                    } else {
                                        table.valueColumnWidthOverrides[i] = root.width - 200 - table.firstColumnWidth - table.columnSpacing - width;

                                        if (table.valueColumnWidthOverrides[i] < minColWidth)
                                            return setColumnWidth(i, minColWidth)
                                    }
                                }
                            }

                            table.forceLayout()
                            tableHeader.model = []
                            tableHeader.model = keyTab.keyModel.columnNames
                        }

                        Connections {
                            target: root

                            function onWidthChanged() {
                                table.valueColumnWidthOverrides = {};
                            }
                        }


                        Component.onCompleted: keyTab.table = table

                        delegate: DelegateChooser {

                            DelegateChoice {
                                column: 0


                                // NOTE: rowNumber - key model zero based index from 0 to rowsCount
                                // NOTE: row - from 0 to pageSize

                                ValueTableCell {
                                    objectName: "rdm_value_table_cell_col1"
                                    implicitWidth: table.firstColumnWidth
                                    implicitHeight: 30
                                    text: Number(rowNumber) + 1
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

                            function onError(error) {
                                valueErrorNotification.text = error
                                valueErrorNotification.open()
                            }

                            function onIsLoadedChanged() {
                                console.log("model loaded (qml)")
                                if (keyTab.keyModel.totalRowCount === 0) {
                                    console.log("Load rows count")
                                    keyTab.keyModel.loadRowsCount()
                                } else {
                                    console.log("Load rows")
                                    keyTab.keyModel.loadRows(currentStart, maxItemsOnPage)
                                }
                            }

                            function onTotalRowCountChanged() {
                                keyTab.keyModel.loadRows(table.currentStart, table.maxItemsOnPage)
                            }

                            function onRowsLoaded() {
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
                                valueEditor.loadRowValue(currentStart + table.model.getOriginalRowIndex(currentRow))
                            }
                        }
                    }
                }
            }

            Loader {
                id: filtersLoader

                Layout.fillWidth: true
                Layout.preferredHeight: 40
                visible: status === Loader.Ready

                source: keyModel && (keyType === "list" || keyType === "stream") ?
                            "./filters/" + String(keyType)[0].toUpperCase()
                            + String(keyType).substring(1) +"Filters.qml"  : ""
            }

        }

        ValueTableActions {}
    }
}
