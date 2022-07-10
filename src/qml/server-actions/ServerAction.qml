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
        color: Qt.rgba(sysPalette.base.red, sysPalette.base.green, sysPalette.base.blue, 0.15)
        z: 1000

        Item {
            anchors.fill: parent

            ProgressBar {
                anchors.centerIn: parent
                indeterminate: true
            }
        }

        MouseArea {
            anchors.fill: parent
        }
    }
}
