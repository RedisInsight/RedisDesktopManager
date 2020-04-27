import QtQuick 2.0
import QtQuick.Controls 2.13

Rectangle {
    id: root

    property alias model: listView.model
    property alias delegate: listView.delegate

    ScrollView {
        anchors.fill: parent
        anchors.margins: 10

        ScrollBar.vertical.policy: ScrollBar.AlwaysOn

        ListView {
            id: listView
            width: root.width * 0.9

            delegate: Label { text: (index+1) + ". " + modelData}
        }
    }

    MouseArea {
        anchors.fill: parent
        acceptedButtons: Qt.RightButton

        onClicked: {
            menu.x = mouseX
            menu.y = mouseY
            menu.open()
        }
    }

    Menu {
        id: menu
        z: 255

        MenuItem {
            text: "Copy"
            icon.source: "qrc:/images/copy.svg"
            icon.color: "transparent"

            onTriggered: {
                var allStrings = "";
                for (var id in affectedKeysListView.model) {
                    allStrings += affectedKeysListView.model[id] + "\n"
                }
                qmlUtils.copyToClipboard(allStrings)
                allStrings = ""
            }
        }
    }
}
