import QtQuick 2.0
import QtQuick.Layouts 1.1
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4
import QtQuick.Dialogs 1.2
import QtQml.Models 2.2
import QtQuick.Window 2.2
import "./../common/platformutils.js" as PlatformUtils
import "."

TreeView {
    id: root
    alternatingRowColors: false
    headerVisible: false
    focus: true
    horizontalScrollBarPolicy: Qt.ScrollBarAsNeeded
    verticalScrollBarPolicy: Qt.ScrollBarAsNeeded

    model: connectionsManager

    style: TreeViewStyle {
        indentation: 12

        rowDelegate: Rectangle {
            height: PlatformUtils.isOSXRetina(Screen) ? 25 : 30
            color: styleData.selected ? "#e2e2e2" : "white"
        }
    }

    TableViewColumn {
        id: itemColumn
        title: "item"
        role: "metadata"

        delegate: FocusScope {
            id: itemRoot

            Item {
                id: wrapper
                objectName: "rdm_tree_view_item"
                height: PlatformUtils.isOSXRetina(Screen) ? 20 : 30
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.verticalCenter: parent.verticalCenter
                anchors.rightMargin: 10

                property bool itemEnabled: styleData.value["state"] === true

                Image {
                    id: itemIcon
                    anchors.left: parent.left
                    anchors.verticalCenter: parent.verticalCenter
                    sourceSize.width: 25
                    sourceSize.height: 25
                    source: {
                        var locked = styleData.value["locked"]                        

                        if (locked === true) {
                            return "qrc:/images/wait.svg"
                        }

                        var type = styleData.value["type"]

                        if (type == "server") {
                            var server_type = styleData.value["server_type"]                            

                            if (server_type == "unknown") {
                                return "qrc:/images/server_offline.svg"
                            } else if (server_type == "standalone") {
                                return "qrc:/images/server.svg"
                            } else {
                                return "qrc:/images/" + server_type + ".svg"
                            }
                        } else if (type == "namespace" && styleData.isExpanded) {
                            return "qrc:/images/" + type + "_open.svg"
                        } else {
                            return "qrc:/images/" + type + ".svg"
                        }

                    }
                    cache: true
                    asynchronous: true
                }

                Text {
                    objectName: "rdm_tree_view_item_text"
                    anchors.left: itemIcon.right
                    anchors.verticalCenter: parent.verticalCenter
                    text: wrapper.itemEnabled ? styleData.value["name"] : styleData.value["name"] + qsTr(" (Removed)")
                    color: wrapper.itemEnabled ? "black": "#ccc"
                }

                Loader {
                    id: menuLoader
                    anchors { right: wrapper.right; top: wrapper.top; bottom: wrapper.bottom; rightMargin: 20 }
                    height: parent.height
                    visible: styleData.selected && wrapper.itemEnabled
                    asynchronous: false

                    source: {
                        if (!(styleData.selected && styleData.value["type"]))
                            return ""

                       return "./menu/" + styleData.value["type"] + ".qml"
                    }

                    onLoaded: wrapper.forceActiveFocus()
                }

                MouseArea {
                    anchors.fill: parent

                    acceptedButtons: Qt.RightButton | Qt.MiddleButton

                    onClicked: {
                        console.log("Catch event to item")

                        if(mouse.button == Qt.RightButton) {
                            mouse.accepted = true
                            connectionTreeSelectionModel.setCurrentIndex(styleData.index, 1)
                            connectionsManager.sendEvent(styleData.index, "right-click")
                            return
                        }

                        if (mouse.button == Qt.MiddleButton) {
                            mouse.accepted = true
                            connectionsManager.sendEvent(styleData.index, "mid-click")
                            return
                        }
                    }
                }

                focus: true
                Keys.forwardTo: menuLoader.item ? [menuLoader.item] : []
            }
        }
    }    

    selectionMode: SelectionMode.SingleSelection

    selection: ItemSelectionModel {
        id: connectionTreeSelectionModel
        model: connectionsManager
    }

    onClicked: connectionsManager && connectionsManager.sendEvent(index, "click")
    onExpanded: connectionsManager.setExpanded(index)
    onCollapsed: connectionsManager.setCollapsed(index)

    Connections {
        target: connectionsManager
        onExpand: !root.isExpanded(index) && root.expand(index)
    }
}
