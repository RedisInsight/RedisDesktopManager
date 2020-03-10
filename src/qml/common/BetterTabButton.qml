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
    ToolTip.visible: ToolTip.text && hovered

    property var self
    property var tabRef

    signal closeClicked()

    onClicked: {
        tabs.activateTab(tabRef)
    }

    ImageButton {
        anchors.right: parent.right
        anchors.verticalCenter: parent.verticalCenter
        anchors.rightMargin: {
            var spacing = (parent.width - parent.contentItem.implicitWidth) / 2 - 20;
            return Math.max(spacing, 0)
        }

        onClicked: {
            for (var btnIndex in tabBar.contentChildren) {
                if (tabBar.contentChildren[btnIndex] === self) {
                    tabBar.removeItem(btnIndex)
                    break
                }
            }
            root.closeClicked()
        }
    }
}
