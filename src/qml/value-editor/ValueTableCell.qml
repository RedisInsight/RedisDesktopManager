import QtQuick 2.0
import QtQuick.Controls 2.0

Item {
    id: root

    property string text
    property alias textDelegate: textItem.sourceComponent
    property alias color: background.color
    property bool selected: false

    signal clicked

    Rectangle {
        id: background
        anchors.fill: parent
        border.color: root.selected ? sysPalette.midlight : sysPalette.mid
        border.width: 1
        color: root.selected ? sysPalette.highlight : sysPalette.base
        clip: true

        Loader {
            id: textItem
            anchors.centerIn: parent
            sourceComponent: Component {
                TextInput {
                    wrapMode: Text.WrapAnywhere
                    color: root.selected ? sysPalette.highlightedText : sysPalette.text
                    readOnly: true
                    selectByMouse: true
                    text: root.text
                    autoScroll: false
                }
            }
        }
    }

    function renderText(t) {
        if (t === "")
            return t

        if (qmlUtils.binaryStringLength(t) > 100) {
            return qmlUtils.printable(t, false, 100) + "..."
        }

        return qmlUtils.printable(t)
                + (textItem.lineCount > 1 ? '...' : '')
    }

    MouseArea {
        anchors.fill: parent
        enabled: !root.selected

        onClicked: {
            root.clicked()
        }
    }
}
