import QtQuick 2.3
import QtQuick.Layouts 1.1
import QtQuick.Controls 1.3
import QtQuick.Controls.Styles 1.1
import "."

TabView {
    id: root

    style: TabViewStyle {
        tab: Rectangle {
            color: "#cccccc"
            implicitWidth: layout.implicitWidth + 3
            implicitHeight: 30
            radius: 3

            Rectangle {
                id: content
                color: styleData.selected ? "white" : "#e2e2e2"
                anchors.fill: parent
                anchors.topMargin: control.tabPosition == Qt.TopEdge ? 1 : 0
                anchors.rightMargin: 1
                anchors.leftMargin: 1
                anchors.bottomMargin: control.tabPosition == Qt.BottomEdge ? 1 : 0

                RowLayout {
                    id: layout
                    anchors.fill: parent
                    anchors.rightMargin: 8

                    Item { Layout.preferredWidth: 3 }

                    AnimatedImage {
                        source: {
                            var icon = root.getTab(styleData.index) !== undefined ? root.getTab(styleData.index).icon : ""

                            if (icon && icon.indexOf(".gif") > -1) {
                                visible = true
                                return icon
                            } else {
                                visible = false
                                return ""
                            }
                        }

                        width: 20
                        height: 20
                    }

                    Image {
                        source: {
                            var icon = root.getTab(styleData.index) !== undefined ? root.getTab(styleData.index).icon : ""

                            if (icon && icon.indexOf(".gif") == -1) {
                                visible = true
                                return icon
                            } else {
                                visible = false
                                return ""
                            }
                        }

                        sourceSize.width: 20
                        sourceSize.height: 20
                    }

                    Text {
                        color: sysPalette.text
                        Layout.fillWidth: true
                        Layout.minimumWidth: implicitWidth
                        text: styleData.title
                    }

                    Item {
                        visible: root.getTab(styleData.index) !== undefined && !root.getTab(styleData.index).closable
                        Layout.preferredWidth: 3
                    }

                    ImageButton {
                        visible: root.getTab(styleData.index) !== undefined && root.getTab(styleData.index).closable

                        Layout.preferredWidth: 18
                        Layout.preferredHeight: 18

                        imgSource: "qrc:/images/clear.svg"
                        onClicked: root.getTab(styleData.index).closeTab(styleData.index)
                    }
                }
            }
        }

        frame: Rectangle {
            color: "#e2e2e2"

            Rectangle {
                color: "white"
                anchors.fill: parent
                anchors.bottomMargin: 1
                anchors.leftMargin: 1
                anchors.rightMargin: 1
                anchors.topMargin: 1
            }
        }

        leftCorner: Item {}
        rightCorner: Item {}
    }

}
