import QtQuick 2.3
import QtQuick.Layouts 1.1
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.1
import QtQuick.Dialogs 1.2
import QtQuick.Window 2.2
import "./editors/editor.js" as Editor
import "./../common/platformutils.js" as PlatformUtils
import "./../common"
import rdm.models 1.0

Repeater {

    BetterTab {
        id: keyTab
        width: approot.width
        height: approot.height
        icon: "qrc:/images/key.svg"
        tabType: "value"
        objectName: "rdm_value_tab"

        closable: true
        onClose: {
            //            console.log(valueEditor!=undefined)

            //            if (valueEditor != undefined && valueEditor.item && valueEditor.item.isValueChanged()) {
            //                // TODO: show "Unsaved changes detected" warnings
            //                return
            //            }
            valueEditor.clear()
            valuesModel.closeTab(tabIndex)
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
                sortOrder: table.sortIndicatorOrder
                sortCaseSensitivity: Qt.CaseInsensitive
                sortRole: keyTab.keyModel && keyTab.keyModel.isLoaded ? table.getColumn(table.sortIndicatorColumn).role : ""

                filterString: table.searchField.text
                filterSyntax: SortFilterProxyModel.Wildcard
                filterCaseSensitivity: Qt.CaseInsensitive
                filterRole: keyTab.keyModel && keyTab.keyModel.isLoaded ? table.getColumn(1).role : ""

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
                visible: !loadingModel
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
                        objectName: "rdm_key_name_field"
                    }

                    Button {
                        text: qsTranslate("RDM","Rename")

                        Dialog {
                            id: renameConfirmation
                            title: qsTranslate("RDM","Rename key")

                            width: 520

                            RowLayout {
                                implicitWidth: 500
                                implicitHeight: 100
                                width: 500

                                Text { text: qsTranslate("RDM","New name:") }
                                TextField {
                                    id: newKeyName;
                                    Layout.fillWidth: true;
                                    objectName: "rdm_rename_key_field"
                                }
                            }

                            onAccepted: {
                                if (newKeyName.text.length == 0) {
                                    return open()
                                }

                                keyTab.keyModel.renameKey(newKeyName.text)
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

                    Item { visible: keyModel.isLoaded && isMultiRow; Layout.preferredWidth: 5}
                    Text {
                        visible: isMultiRow || keyType === "hyperloglog";
                        text:  qsTranslate("RDM","Size: ") + keyRowsCount
                    }
                    Item { Layout.preferredWidth: 5}

                    Button {
                        text: qsTranslate("RDM","TTL:") + keyTtl
                        objectName: "rdm_key_ttl_value"

                        Dialog {
                            id: setTTLConfirmation
                            title: qsTranslate("RDM","Set key TTL")

                            width: 520

                            RowLayout {
                                implicitWidth: 500
                                implicitHeight: 100
                                width: 500

                                Text { text: qsTranslate("RDM","New TTL:") }
                                TextField {
                                    id: newTTL;
                                    Layout.fillWidth: true;
                                    objectName: "rdm_set_ttl_key_field"
                                }
                            }

                            onAccepted: {
                                if (newTTL.text.length == 0) {
                                    return open()
                                }

                                keyTab.keyModel.setTTL(newTTL.text)
                            }

                            visible: false
                            modality: Qt.ApplicationModal
                            standardButtons: StandardButton.Ok | StandardButton.Cancel
                        }

                        onClicked: {
                            newTTL.text = ""+keyTtl
                            setTTLConfirmation.open()
                        }
                    }


                    Item { Layout.preferredWidth: 5}                    

                    Button {
                        text: qsTranslate("RDM","Delete")
                        iconSource: "qrc:/images/delete.svg"

                        MessageDialog {
                            id: deleteConfirmation
                            title: qsTranslate("RDM","Delete key")
                            text: qsTranslate("RDM","Do you really want to delete this key?")
                            onYes: {                                
                                keyTab.keyModel.removeKey()
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
                        text: qsTranslate("RDM","Reload Value")
                        action: reLoadAction
                        visible: !isMultiRow
                        iconSource: "qrc:/images/refresh.svg"
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

                    // Table
                    RowLayout {
                        id: navigationTable
                        Layout.fillWidth: true
                        Layout.fillHeight: false
                        visible: isMultiRow

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

                            property var currentStart: 0
                            property int maxItemsOnPage: keyTab.keyModel ? keyTab.keyModel.pageSize : 100
                            property int currentPage: currentStart / maxItemsOnPage + 1
                            property int totalPages: keyTab.keyModel ? Math.ceil(keyTab.keyModel.totalRowCount / maxItemsOnPage) : 0
                            property bool forceLoading: false

                            Component.onCompleted: {
                                keyTab.table = table
                            }

                            itemDelegate: Item {
                                Text {
                                    anchors.fill: parent
                                    color: styleData.textColor                                    
                                    text: {

                                        if (styleData.value === "" || !isMultiRow) {
                                            return ""
                                        }

                                        if (styleData.column === 2 && keyType == "zset") {
                                            return parseFloat(Number(styleData.value).toFixed(20))
                                        }

                                        if (qmlUtils.binaryStringLength(styleData.value) > 1000) {
                                            return qmlUtils.printable(styleData.value, false, 1000) + "..."
                                        }

                                        return qmlUtils.printable(styleData.value)
                                                            + (lineCount > 1 ? '...' : '')
                                    }
                                    elide: Text.ElideRight
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

                                target: keyTab.keyModel

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
                                        var columns = keyTab.keyModel.columnNames                                        

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
                                        valueEditor.clear()
                                    } else {
                                        valueEditor.loadRowValue(0)
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

                            onRowCountChanged: wrapper.hideLoader()
                            onActivated: valueEditor.loadRowValue(table.model.getOriginalRowIndex(row))
                            onClicked: valueEditor.loadRowValue(table.model.getOriginalRowIndex(row))

                            Connections {
                                target: table.selection
                                onSelectionChanged:{                                   
                                    if (table.currentRow !== -1) {
                                        console.log("Selection changed", table.currentRow)
                                        return valueEditor.loadRowValue(table.model.getOriginalRowIndex(table.currentRow))
                                    }
                                }
                            }
                        }

                        ColumnLayout {
                            Layout.fillHeight: true
                            Layout.preferredWidth: 200
                            Layout.maximumWidth: 200
                            Layout.alignment: Qt.AlignTop

                            Button {
                                Layout.preferredWidth: 195
                                text: qsTranslate("RDM","Add Row")
                                iconSource: "qrc:/images/add.svg"
                                onClicked: {
                                    addRowDialog.open()
                                }

                                Dialog {
                                    id: addRowDialog
                                    title: keyType === "hyperloglog"? qsTranslate("RDM","Add Element to HLL")
                                                                    : qsTranslate("RDM","Add Row")

                                    width: 550
                                    height: 400
                                    modality: Qt.ApplicationModal

                                    contentItem: Rectangle {
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

                                                source: Editor.getEditorByTypeString(keyType)

                                                onLoaded: {
                                                    item.state = "add"
                                                    item.initEmpty()
                                                }
                                            }

                                            RowLayout {
                                                Layout.fillWidth: true

                                                Item {
                                                    Layout.fillWidth: true
                                                }

                                                Button {
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
                                            }
                                        }
                                    }

                                    visible: false
                                }
                            }

                            Button {
                                Layout.preferredWidth: 195
                                text: qsTranslate("RDM","Delete row")
                                iconSource: "qrc:/images/delete.svg"
                                enabled: table.currentRow != -1

                                onClicked: {
                                    if (keyTab.keyModel.totalRowCount === 1) {
                                        deleteRowConfirmation.text = qsTranslate("RDM","The row is the last one in the key. After removing it key will be deleted.")
                                    } else {
                                        deleteRowConfirmation.text = qsTranslate("RDM","Do you really want to remove this row?")
                                    }

                                    var rowIndex = table.model.getOriginalRowIndex(table.currentRow)
                                    console.log("Original row index in model:", rowIndex)

                                    deleteRowConfirmation.rowToDelete = rowIndex
                                    deleteRowConfirmation.open()
                                }

                                MessageDialog {
                                    id: deleteRowConfirmation
                                    title: qsTranslate("RDM","Delete row")
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

                                            if (table.currentPage > table.totalPages) {
                                                table.goToPage(1)
                                            }
                                        }
                                    }
                                }
                            }

                            RowLayout {
                                Layout.preferredWidth: 195

                                TextField {
                                    id: searchField

                                    Layout.fillWidth: true

                                    readOnly: keyTab.keyModel.singlePageMode
                                    placeholderText: qsTranslate("RDM","Search on page...")

                                    Component.onCompleted: {
                                        table.searchField = searchField
                                    }
                                }

                                Button {
                                    id: clearGlobalSearch
                                    visible: keyTab.keyModel.singlePageMode

                                    iconSource: "qrc:/images/clear.svg"

                                    onClicked: {
                                        wrapper.showLoader()
                                        searchField.text = ""
                                        keyTab.keyModel.singlePageMode = false
                                        reLoadAction.trigger()
                                    }
                                }
                            }

                            Button {
                                id: globalSearch

                                Layout.preferredWidth: 195
                                iconSource: "qrc:/images/execute.svg"
                                text: qsTranslate("RDM","Search through All values")

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
                                Layout.maximumWidth: 150
                                visible: isMultiRow
                            }
                        }
                    }
                    // Table end

                    //Value editor
                    ColumnLayout {
                        id: editorWrapper
                        Layout.fillWidth: true
                        Layout.fillHeight: !isMultiRow
                        spacing: 0

                        Loader {
                            id: valueEditor
                            Layout.fillWidth: true
                            Layout.fillHeight: true
                            Layout.minimumHeight: 180
                            Layout.maximumHeight: {
                                if (isMultiRow) {
                                    return (approot.height - bottomTabView.height - navigationTable.height
                                            - editorButtonsRow.height - 50)
                                } else {
                                    return approot.height - bottomTabView.height - editorButtonsRow.height - 50
                                }
                            }

                            Component.onCompleted: {
                                keyTab.valueEditor = valueEditor
                            }

                            property int currentRow: -1

                            source: Editor.getEditorByTypeString(keyType)

                            function loadRowValue(row) {
                                console.log("loading row value", row)
                                if (valueEditor.item) {
                                    var rowValue = keyTab.keyModel.getRow(row, true)
                                    valueEditor.currentRow = row
                                    valueEditor.item.setValue(rowValue)
                                } else {
                                    console.log("cannot load row value - item is missing")
                                }
                            }

                            function clear() {
                                if (valueEditor.item) {
                                    currentRow = -1
                                    valueEditor.item.reset()
                                }
                            }

                            onLoaded: clear()
                        }

                        RowLayout {
                            id: editorButtonsRow
                            Layout.fillWidth: true
                            Layout.minimumHeight: 40
                            Item { Layout.fillWidth: true}

                            Button {
                                visible: keyType === "hyperloglog"
                                Layout.preferredWidth: 195
                                text: qsTranslate("RDM","Add Element to HLL");
                                iconSource: "qrc:/images/add.svg"
                                onClicked: {
                                    addRowDialog.open()
                                }
                            }

                            Button {
                                text: qsTranslate("RDM","Save")

                                enabled: keyType != "stream"

                                onClicked: {
                                    if (!valueEditor.item || !valueEditor.item.isEdited()) {
                                        savingConfirmation.text = qsTranslate("RDM","Nothing to save")
                                        savingConfirmation.open()
                                        return
                                    }

                                    valueEditor.item.validateValue(function (result){
                                        if (!result)
                                            return;

                                        var value = valueEditor.item.getValue()
                                        keyTab.keyModel.updateRow(valueEditor.currentRow, value)

                                        savingConfirmation.text = qsTranslate("RDM","Value was updated!")
                                        savingConfirmation.open()
                                    })
                                }
                            }

                            MessageDialog {
                                id: savingConfirmation
                                title: qsTranslate("RDM","Save value")
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
                visible: loadingModel
                anchors.fill: parent
                color: loadingModel? Qt.rgba(0, 0, 0, 0) : Qt.rgba(0, 0, 0, 0.1)

                Item {
                    anchors.fill: parent

                    ColumnLayout {
                        anchors.centerIn: parent;

                        BusyIndicator { Layout.alignment: Qt.AlignHCenter;  running: true }

                        Text {
                            visible: loadingModel
                            text: keyTitle
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

