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

ColumnLayout {
    Layout.fillHeight: true
    Layout.preferredWidth: 200
    Layout.maximumWidth: 200
    Layout.alignment: Qt.AlignTop
    Layout.bottomMargin: 10

    BetterButton {
        objectName: "rdm_value_tab_add_row_btn"
        Layout.fillWidth: true
        text: qsTranslate("RESP","Add Row")
        iconSource: PlatformUtils.getThemeIcon("add.svg")
        onClicked: {
            addRowDialog.open()
        }

        BetterDialog {
            id: addRowDialog
            title: keyType === "hyperloglog"? qsTranslate("RESP","Add Element to HLL")
                                            : qsTranslate("RESP","Add Row")

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

                        source: keyTab.keyModel ? Editor.getEditorByTypeString(keyType, true) : ""

                        onLoaded: {
                            item.state = "add"                            
                            item.initEmpty()
                            keyTab.addRowDialog = addRowDialog
                        }
                    }
                }
            }

            footer: BetterDialogButtonBox {
                BetterButton {
                    DialogButtonBox.buttonRole: DialogButtonBox.AcceptRole
                    objectName: "rdb_add_row_dialog_add_button"
                    text: qsTranslate("RESP","Add")

                    onClicked: {
                        if (!valueAddEditor.item)
                            return false

                        valueAddEditor.item.getValue(true, function (valid, row){
                            if (!valid) {
                                return;
                            }                            

                            keyTab.keyModel.addRow(row)
                            keyTab.keyModel.reload()
                            valueAddEditor.item.reset()
                            valueAddEditor.item.initEmpty()
                            addRowDialog.close()
                        });
                    }
                }

                BetterButton {
                    text: qsTranslate("RESP", "Cancel")
                    DialogButtonBox.buttonRole: DialogButtonBox.RejectRole
                }
            }

            visible: false
        }
    }

    BetterButton {
        objectName: "rdm_value_editor_delete_row_btn"
        Layout.fillWidth: true
        text: qsTranslate("RESP","Delete row")
        iconSource: PlatformUtils.getThemeIcon("delete.svg")
        enabled: table.currentRow != -1

        onClicked: {
            if (keyTab.keyModel.totalRowCount === 1) {
                deleteRowConfirmation.text = qsTranslate("RESP","The row is the last one in the key. After removing it key will be deleted.")
            } else {
                deleteRowConfirmation.text = qsTranslate("RESP","Do you really want to remove this row?")
            }

            var rowIndex = table.currentStart + table.model.getOriginalRowIndex(table.currentRow)

            console.log("removing row", rowIndex)

            deleteRowConfirmation.rowToDelete = rowIndex
            deleteRowConfirmation.open()
        }

        BetterMessageDialog {
            id: deleteRowConfirmation
            title: qsTranslate("RESP","Delete row")
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
        text: qsTranslate("RESP","Reload Value")
        iconSource: PlatformUtils.getThemeIcon("refresh.svg")
        action: reLoadAction

        Action {
            id: reLoadAction
            shortcut: StandardKey.Refresh
            onTriggered: {
                reloadValue()
            }


        }
    }

    RowLayout {
        Layout.fillWidth: true

        BetterTextField {
            id: searchField

            Layout.fillWidth: true

            readOnly: keyTab.keyModel ? keyTab.keyModel.singlePageMode : false
            placeholderText: qsTranslate("RESP","Search on page...")

            Component.onCompleted: {
                table.searchField = searchField
            }
        }

        BetterButton {
            id: clearGlobalSearch
            visible: keyTab.keyModel ? keyTab.keyModel.singlePageMode : false

            iconSource: PlatformUtils.getThemeIcon("clear.svg")

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
        iconSource: PlatformUtils.getThemeIcon("loader.svg")
        text: qsTranslate("RESP","Full Search")

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
