import QtQuick 2.0
import QtQuick.Layouts 1.1
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.1
import QtQuick.Dialogs 1.2
import QtQml.Models 2.2
import "."

TreeView {
    id: root
    alternatingRowColors: false
    headerVisible: false

    TableViewColumn {
        title: "item"
        role: "icon"
        width: 25
        delegate: Item {
            Image {
                anchors.centerIn: parent
                sourceSize.width: 25
                sourceSize.height: 25
                source: styleData.value
                cache: true
                asynchronous: true
            }
        }
    }

    TableViewColumn {
        id: itemColumn
        title: "item"
        role: "name"
        width: root.width - 20
    }

    itemDelegate: Item {
        id: itemRoot

        Item {
            id: wrapper
            height: 30
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.verticalCenter: parent.verticalCenter
            anchors.rightMargin: 10


            Text {
                anchors.left: parent.left
                anchors.verticalCenter: parent.verticalCenter
                //elide: styleData.elideMode
                text: styleData.value
                anchors.leftMargin: {
                    var itemDepth = connectionsManager.getItemDepth(styleData.index)
                    return itemDepth * 10 + 15
                }
            }

            Loader {
                id: menuLoader
                anchors {right: wrapper.right; top: wrapper.top; bottom: wrapper.bottom; }
                anchors.rightMargin: 20
                height: parent.height
                visible: styleData.selected
                asynchronous: true

                source: {
                    if (!styleData.selected
                            || !connectionsManager
                            || !styleData.index)
                        return ""

                    var type = connectionsManager.getItemType(styleData.index)

                    if (type != undefined) {
                        return "./menu/" + type + ".qml"
                    } else {
                        return ""
                    }
                }
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

    selectionMode: SelectionMode.SingleSelection

    selection: ItemSelectionModel {
        id: connectionTreeSelectionModel
        model: connectionsManager
    }

    model: connectionsManager

    rowDelegate: Rectangle {
        height: 30
        color: styleData.selected ? "#e2e2e2" : "white" //sysPalette.highlight
    }

    onClicked: {
        if (!connectionsManager)
            return

        connectionsManager.sendEvent(index, "click")
    }

    onExpanded: connectionsManager.setExpanded(index)
    onCollapsed: connectionsManager.setCollapsed(index)

    Connections {
        target: connectionsManager;
        onExpand: {
            if (!root.isExpanded(index)) {                                
                root.expand(index)

                // Hack to prevent rendering issues
                root.__listView.contentY = root.__listView.contentY + 20
                root.__listView.forceLayout()
            }
        }
    }
}
