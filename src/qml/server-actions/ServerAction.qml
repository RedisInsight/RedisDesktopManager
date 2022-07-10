import QtQuick 2.0
import QtQuick.Controls 2.13

Item {
    id: root
    property var model
    property alias uiBlocked: uiBlocker.visible

    function stopTimer() {}

    Component.onCompleted: {
        uiBlocker.visible = true
    }

    Rectangle {
        id: uiBlocker
        visible: false
        anchors.fill: parent
        color: Qt.rgba(0, 0, 0, 0.1)
        z: 1000

        Item {
            anchors.fill: parent
            BusyIndicator { anchors.centerIn: parent; running: true }
        }

        MouseArea {
            anchors.fill: parent
        }
    }
}
