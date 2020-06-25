import QtQuick 2.0
import QtQuick.Controls 2.13

TabButton {
    id: root

    implicitHeight: 30
    leftPadding: 5
    spacing: 0
    icon.width: 18
    icon.height: 18
    icon.color: "transparent"

    property var self
    property var tabRef
    property string tooltip

    signal closeClicked()

    onClicked: {
        tabs.activateTab(tabRef)
    }

    palette.brightText: sysPalette.text
    palette.dark: sysPalette.button
    palette.mid: sysPalette.window
    palette.window: sysPalette.base
    palette.windowText: sysPalette.windowText

    ImageButton {
        anchors.right: parent.right
        anchors.verticalCenter: parent.verticalCenter
        anchors.rightMargin: {
            var spacing = (parent.width - parent.contentItem.implicitWidth) / 2 - 20;
            return Math.max(spacing, 0)
        }

        onClicked: {
            root.closeClicked()
        }
    }

    BetterToolTip {
        title: tooltip
    }
}
