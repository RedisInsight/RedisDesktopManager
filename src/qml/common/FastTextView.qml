import QtQuick 2.0
import QtQuick.Controls 2.13

Rectangle {
    id: root

    property alias model: listView.model
    property alias delegate: listView.delegate
    property bool showLineNumbers: true

    function positionViewAtEnd() {
        listView.positionViewAtEnd()
    }

    function dumpText() {        
        var allStrings = "";
        for (var id in root.model) {
            allStrings += root.model[id] + "\n"
        }
        return allStrings
    }

    ScrollView {
        anchors.fill: parent
        anchors.margins: 10

        ScrollBar.vertical.policy: ScrollBar.AlwaysOn

        ListView {
            id: listView
            width: root.width - 20

            delegate: TextEdit {
                color: sysPalette.text
                width: listView.width
                readOnly: true
                selectByMouse: true
                text: {
                    if (root.showLineNumbers) {
                        return (index+1) + ". " + modelData
                    } else {
                        return modelData
                    }
                }
                wrapMode: Text.WrapAnywhere
            }
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
                qmlUtils.copyToClipboard(root.dumpText())
            }
        }
    }
}
