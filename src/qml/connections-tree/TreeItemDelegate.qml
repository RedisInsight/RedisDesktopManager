import QtQuick 2.14
import QtQuick.Layouts 1.1
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4
import QtQml.Models 2.2
import QtQuick.Window 2.2
import "./../common/platformutils.js" as PlatformUtils
import "./../common"

FocusScope {
    id: root

    property bool sortConnections: false
    property var treeRoot

    function __getIconColorMappings(userColor) {
        if (!userColor)
            return {}

        if (approot.darkModeEnabled) {
            return {
                "unknown": {
                    "#979798": qmlUtils.changeColorAlpha(userColor, 150)
                },
                "standalone": {
                    "#DC423C": userColor,
                    "#9A2928": qmlUtils.changeColorAlpha(userColor, 200)
                },
                "cluster": {
                    "#5856D6": userColor
                },
                "sentinel": {
                    "#DC423C": userColor
                },
                "database": {
                    "#DC423C": userColor
                }
            }
        } else {
            return {
                "unknown": {
                    "#B8BEC9": qmlUtils.changeColorAlpha(userColor, 150)
                },
                "standalone": {
                    "#DC423C": userColor,
                    "#9A2928": qmlUtils.changeColorAlpha(userColor, 200)
                },
                "cluster": {
                    "#5E5CE6": userColor
                },
                "sentinel": {
                    "#DC423C": userColor
                },
                "database": {
                    "#DC423C": userColor
                }
            }
        }
    }

    MouseArea {
        id: dragArea
        anchors.fill: parent

        acceptedButtons: root.sortConnections ? Qt.LeftButton : Qt.RightButton | Qt.MiddleButton

        drag.target: root.sortConnections ? wrapper : null
        drag.axis: Drag.YAxis

        property bool held: false

        hoverEnabled: true
        propagateComposedEvents: !root.sortConnections

        onReleased: {
            if (root.sortConnections) {
                wrapper.Drag.drop()
                held = false
                wrapper.color = "transparent"
                wrapper.border.width = 0
                return
            }
        }

        onPressed: {
            if (root.sortConnections && styleData.value["type"] === "server") {
                held = true
                wrapper.border.width = 1
                wrapper.border.color = sysPalette.light
                return
            }
        }

        onClicked: {
            console.log("Catch event to item")

            if (mouse.button === Qt.RightButton) {
                mouse.accepted = true
                connectionTreeSelectionModel.setCurrentIndex(styleData.index, 1)
                connectionsManager.sendEvent(styleData.index, "right-click")
                return
            }

            if (mouse.button === Qt.MiddleButton) {
                mouse.accepted = true
                connectionsManager.sendEvent(styleData.index, "mid-click")
                return
            }
        }

        Rectangle {
            id: wrapper
            objectName: "rdm_tree_view_item"
            height: PlatformUtils.isOSXRetina(Screen) ? 20 : 30
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.verticalCenter: parent.verticalCenter
            anchors.rightMargin: 10
            color: "transparent"

            property var itemIndex: styleData.index

            Drag.active: dragArea.held
            Drag.hotSpot.x: width / 3
            Drag.hotSpot.y: height / 3

            states: State {
                when: dragArea.held

                ParentChange {
                    target: wrapper
                    parent: treeRoot
                }
                PropertyChanges {
                    target: wrapper
                    anchors.leftMargin: 30
                }
                AnchorChanges {
                    target: wrapper
                    anchors {
                        verticalCenter: undefined
                    }
                }
            }

            property bool itemEnabled: styleData.value["state"] === true
            property bool itemLocked: styleData.value["locked"] === true
            property string itemType: styleData.value["type"] ? styleData.value["type"] : ""
            property string userColor: styleData.value["user_color"] ? styleData.value["user_color"] : ""
            property string itemIconSource: {
                if (itemLocked) {
                    return PlatformUtils.getThemeIcon("wait.svg")
                }

                var type = itemType

                if (type === "server") {
                    var server_type = styleData.value["server_type"]
                    var serverIcon = ""

                    if (server_type === "unknown") {
                        serverIcon = PlatformUtils.getThemeIcon(
                                    "server_offline.svg")
                    } else if (server_type === "standalone") {
                        serverIcon = PlatformUtils.getThemeIcon("server.svg")
                    } else {
                        serverIcon = PlatformUtils.getThemeIcon(
                                    server_type + ".svg")
                    }

                    if (userColor) {
                        return qmlUtils.replaceColorsInSvg(
                                    serverIcon, root.__getIconColorMappings(
                                        userColor)[server_type])
                    } else {
                        return serverIcon
                    }
                } else if (type === "database") {
                    if (styleData.value["live_update"] === true) {
                        return PlatformUtils.getThemeIcon("live_update.svg")
                    } else {
                        var icon = PlatformUtils.getThemeIcon(type + ".svg")
                        if (userColor) {
                            return qmlUtils.replaceColorsInSvg(
                                        icon, root.__getIconColorMappings(
                                            userColor)[type])
                        } else {
                            return icon
                        }
                    }
                } else if (type === "namespace" || type == "server_group"
                           && styleData.isExpanded) {
                    return PlatformUtils.getThemeIcon(type + "_open.svg")
                } else {
                    if (type !== "") {
                        return PlatformUtils.getThemeIcon(type + ".svg")
                    } else {
                        return ""
                    }
                }
            }

            Image {
                id: itemIcon
                anchors.left: parent.left
                anchors.verticalCenter: parent.verticalCenter
                sourceSize.width: 25
                sourceSize.height: 25
                source: wrapper ? wrapper.itemIconSource : ""
                cache: true
                asynchronous: true
            }

            Text {
                objectName: "rdm_tree_view_item_text"
                anchors.left: itemIcon.right
                anchors.leftMargin: 3
                anchors.verticalCenter: parent.verticalCenter
                text: wrapper.itemEnabled ? styleData.value["name"] : styleData.value["name"]
                                            + qsTranslate("RESP", " (Removed)")
                color: wrapper.itemEnabled ? styleData.selected ? sysPalette.highlightedText : sysPalette.windowText : inactiveSysPalette.windowText
            }

            Rectangle {
                id: menuWrapper
                implicitWidth: styleData.value["type"] === "database"? 200 : 150
                anchors {
                    right: wrapper.right
                    top: wrapper.top
                    bottom: wrapper.bottom
                    rightMargin: 20
                }
                height: parent.height
                visible: styleData.selected && wrapper.itemEnabled
                color: {
                    var baseColor = sysPalette.highlight;
                    if (styleData.selected) {
                        return Qt.rgba(baseColor.r, baseColor.g, baseColor.b, 0.85)
                    } else {
                        return "transparent"
                    }
                }

                Loader {
                    id: menuLoader
                    anchors {
                        right: menuWrapper.right
                        top: menuWrapper.top
                        bottom: menuWrapper.bottom
                    }
                    height: parent.height
                    asynchronous: false

                    source: {
                        if (!(styleData.selected && styleData.value["type"]))
                            return ""

                        return "./menu/" + styleData.value["type"] + ".qml"
                    }

                    onLoaded: {
                        wrapper.forceActiveFocus()
                        menuWrapper.width = item.width
                    }
                }
            }

            focus: true
            Keys.forwardTo: menuLoader.item ? [menuLoader.item] : []
        }

        DropArea {
            anchors {
                fill: parent
            }

            onEntered: {
                if (styleData.value["type"] === "server_group") {
                    wrapper.border.width = 1
                    wrapper.border.color = sysPalette.highlight
                }
            }

            onDropped: {
                wrapper.border.width = 0
                connectionsManager.dropItemAt(drag.source.itemIndex,
                                              styleData.index)
            }

            onExited: {
                wrapper.border.width = 0
            }
        }
    }
}
