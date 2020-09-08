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
                            newTTL.text = ""+keyTtl
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
                        action: reLoadAction
                        visible: !isMultiRow
                        iconSource: "qrc:/images/refresh.svg"
                    }
                }

                BetterSplitView {
                    orientation: Qt.Vertical
                    Layout.fillHeight: true
                    Layout.fillWidth: true

                    // Table
                    RowLayout {
                        id: navigationTable
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
                                    anchors.fill: parent

                                    ScrollBar.horizontal.policy: ScrollBar.AlwaysOff
                                    ScrollBar.vertical.policy: ScrollBar.AlwaysOn

                                    TableView {
                                        id: table
                                        objectName: "rdm_value_tab_table"

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
                                                    onClicked: table.currentRow = row
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
                                                    onClicked: table.currentRow = row
                                                }
                                            }

                                            DelegateChoice {
                                                column: 2

                                                ValueTableCell {
                                                    objectName: "rdm_value_table_cell_col3"
                                                    implicitWidth: table.valueColumnWidth
                                                    implicitHeight: 30

                                                    selected: table.currentRow === row
                                                    onClicked: table.currentRow = row

                                                    text: {
                                                        if (display === "" || !isMultiRow) {
                                                            return ""
                                                        }

                                                        if (keyType == "zset") {
                                                            return parseFloat(Number(display).toFixed(20))
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

                        ColumnLayout {
                            Layout.fillHeight: true
                            Layout.preferredWidth: 200
                            Layout.maximumWidth: 200
                            Layout.alignment: Qt.AlignTop
                            Layout.bottomMargin: 10

                            BetterButton {
                                objectName: "rdm_value_tab_add_row_btn"
                                Layout.fillWidth: true
                                text: qsTranslate("RDM","Add Row")
                                iconSource: "qrc:/images/add.svg"
                                onClicked: {
                                    addRowDialog.open()
                                }

                                BetterDialog {
                                    id: addRowDialog
                                    title: keyType === "hyperloglog"? qsTranslate("RDM","Add Element to HLL")
                                                                    : qsTranslate("RDM","Add Row")

                                    width: 550
                                    height: 400

                                    contentItem: Rectangle {
                                        color: sysPalette.base
                                        implicitWidth: 800
                                        implicitHeight: PlatformUtils.isOSX()? 680 : 600

                                        ColumnLayout {
                                            anchors.fill: parent
                                            anchors.margins: 10

                                            Loader {
                                                id: valueAddEditor

                                                Layout.fillWidth: true
                                                Layout.fillHeight: true

                                                property int currentRow: -1
                                                objectName: "rdm_add_row_dialog"

                                                source: keyTab.keyModel ? Editor.getEditorByTypeString(keyType) : ""

                                                onLoaded: {
                                                    item.state = "add"
                                                    item.initEmpty()
                                                }
                                            }
                                        }
                                    }

                                    footer: BetterDialogButtonBox {
                                        BetterButton {
                                            DialogButtonBox.buttonRole: DialogButtonBox.AcceptRole
                                            objectName: "rdb_add_row_dialog_add_button"
                                            text: qsTranslate("RDM","Add")

                                            onClicked: {
                                                if (!valueAddEditor.item)
                                                    return false

                                                valueAddEditor.item.validateValue(function (result){
                                                    if (!result) {
                                                        return;
                                                    }

                                                    var row = valueAddEditor.item.getValue()

                                                    keyTab.keyModel.addRow(row)
                                                    keyTab.keyModel.reload()
                                                    valueAddEditor.item.reset()
                                                    valueAddEditor.item.initEmpty()
                                                    addRowDialog.close()
                                                });
                                            }
                                        }

                                        BetterButton {
                                            text: qsTranslate("RDM", "Cancel")
                                            DialogButtonBox.buttonRole: DialogButtonBox.RejectRole
                                        }
                                    }

                                    visible: false
                                }
                            }

                            BetterButton {
                                objectName: "rdm_value_editor_delete_row_btn"
                                Layout.fillWidth: true
                                text: qsTranslate("RDM","Delete row")
                                iconSource: "qrc:/images/delete.svg"
                                enabled: table.currentRow != -1

                                onClicked: {
                                    if (keyTab.keyModel.totalRowCount === 1) {
                                        deleteRowConfirmation.text = qsTranslate("RDM","The row is the last one in the key. After removing it key will be deleted.")
                                    } else {
                                        deleteRowConfirmation.text = qsTranslate("RDM","Do you really want to remove this row?")
                                    }

                                    console.log("removing row", table.currentRow)

                                    deleteRowConfirmation.rowToDelete = table.currentRow
                                    deleteRowConfirmation.open()
                                }

                                BetterMessageDialog {
                                    id: deleteRowConfirmation
                                    title: qsTranslate("RDM","Delete row")
                                    text: ""
                                    onYesClicked: {
                                        console.log("remove row in key")
                                        keyTab.keyModel.deleteRow(rowToDelete)
                                        table.resetCurrentRow()
                                        valueEditor.clear()
                                        table.model.invalidate()
                                    }
                                    visible: false
                                    property int rowToDelete
                                }

                            }

                            BetterButton {
                                objectName: "rdm_value_editor_reload_value_btn"
                                Layout.fillWidth: true
                                text: qsTranslate("RDM","Reload Value")
                                iconSource: "qrc:/images/refresh.svg"
                                action: reLoadAction

                                Action {
                                    id: reLoadAction
                                    shortcut: StandardKey.Refresh
                                    onTriggered: {
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
                                }
                            }

                            RowLayout {
                                Layout.fillWidth: true

                                BetterTextField {
                                    id: searchField

                                    Layout.fillWidth: true

                                    readOnly: keyTab.keyModel ? keyTab.keyModel.singlePageMode : false
                                    placeholderText: qsTranslate("RDM","Search on page...")

                                    Component.onCompleted: {
                                        table.searchField = searchField
                                    }
                                }

                                BetterButton {
                                    id: clearGlobalSearch
                                    visible: keyTab.keyModel ? keyTab.keyModel.singlePageMode : false

                                    iconSource: "qrc:/images/clear.svg"

                                    onClicked: {
                                        wrapper.showLoader()
                                        searchField.text = ""
                                        keyTab.keyModel.singlePageMode = false
                                        reLoadAction.trigger()
                                    }
                                }
                            }

                            BetterButton {
                                id: globalSearch

                                Layout.fillWidth: true
                                iconSource: "qrc:/images/execute.svg"
                                text: qsTranslate("RDM","Full Search")

                                onClicked: {
                                    wrapper.showLoader()
                                    keyTab.keyModel.singlePageMode = true
                                    keyTab.keyModel.loadRows(0, keyTab.keyModel.totalRowCount)
                                }
                            }

                            Item {
                                Layout.fillWidth: true
                                Layout.fillHeight: true
                            }

                            Pagination {
                                id: pagination
                                Layout.fillWidth: true
                                visible: keyTab.keyModel ? isMultiRow : false
                            }
                        }
                    }
                    // Table end

                    // Value editor
                    ColumnLayout {
                        id: editorWrapper
                        SplitView.fillWidth: true
                        SplitView.fillHeight: !isMultiRow
                        Layout.topMargin: 10
                        SplitView.minimumHeight: 230
                        spacing: 10

                        Loader {
                            id: valueEditor
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
