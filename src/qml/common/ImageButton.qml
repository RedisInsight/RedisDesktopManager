import QtQuick 2.9
import QtQuick.Controls 2.3

BetterButton {
    id: root

    implicitWidth: 18
    implicitHeight: 18
    property alias imgWidth: img.width
    property alias imgHeight: img.height
    property alias imgSource: img.source
    property alias iconSource: img.source
    property bool showBorder: false

    property string tooltip

    MouseArea {
            id: mouseArea
            anchors.fill: parent
            onPressed: mouse.accepted = false
            cursorShape: Qt.PointingHandCursor
    }

    Image {
        id: img
        anchors.centerIn: parent
        source: "qrc:/images/clear.svg"
        width: 18
        height: 18
        sourceSize.width: width * 2
        sourceSize.height: height * 2
        opacity: root.enabled? 1.0: 0.8
    }

    background: Rectangle {
        implicitWidth: root.implicitWidth + 3
        implicitHeight: root.implicitHeight + 3
        opacity: root.enabled ? 1 : 0.3
        color: root.hovered ? sysPalette.highlight : "transparent"
        border.width: root.hovered ? 1 : root.showBorder ? 1 : 0
        border.color: root.hovered? sysPalette.highlight : sysPalette.mid
        radius: 5
   }

   BetterToolTip {
       title: root.tooltip
   }
}

