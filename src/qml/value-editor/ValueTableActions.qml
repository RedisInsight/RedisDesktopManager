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
