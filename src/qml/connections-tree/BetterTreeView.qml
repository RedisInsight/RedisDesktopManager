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
        role: "name"
        width: root.width
    }

    selectionMode: SelectionMode.SingleSelection

    selection: ItemSelectionModel {
        id: connectionTreeSelectionModel
        model: connectionsManager
    }

    model: connectionsManager

    rowDelegate: Rectangle {
        height: 28
        color: styleData.selected ? "#e2e2e2" : "white" //sysPalette.highlight
    }

    itemDelegate: Item {
       id: itemRoot

        Loader {
            id: itemIcon
            width: 21
            height: 21
            asynchronous: true

            anchors {left: itemRoot.left; verticalCenter: itemRoot.verticalCenter; }

            sourceComponent: Component {
                Image {
                    anchors.centerIn: parent

                    sourceSize.width: 25
                    sourceSize.height: 25

                    source: {
                        if (!connectionsManager || !styleData.index)
                            return ""

                        var icon = connectionsManager.getItemIcon(styleData.index)

                        if (icon != undefined) {
                            return icon
                        } else {
                            return ""
                        }
                    }
                }
            }
        }

        Item {
            id: itemText
            anchors {left: itemIcon.right; top: itemRoot.top; bottom: itemRoot.bottom; leftMargin: 5 }

            Text {
                anchors.verticalCenter: parent.verticalCenter
                //color: styleData.selected ? sysPalette.highlightedText : styleData.textColor
                elide: styleData.elideMode
                text: styleData.value
            }
        }

        Loader {
            id: menuLoader
            anchors {right: itemRoot.right; top: itemRoot.top; bottom: itemRoot.bottom; }
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
            anchors.left: itemIcon.left
            anchors.top: parent.top
            anchors.right: parent.right
            anchors.bottom: parent.bottom

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
            }
        }
    }
}
