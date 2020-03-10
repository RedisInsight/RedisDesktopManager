import QtQuick 2.0

Item {
    id: root

    property alias text: textItem.text
    property alias color: background.color
    property bool selected: false

    signal clicked

    Rectangle {
        id: background
        anchors.fill: parent
        border.color: root.selected ? sysPalette.mid : "lightgrey"
        border.width: 1
        color: root.selected ? sysPalette.highlight : "transparent"

        Text {
            id: textItem
            anchors.centerIn: parent
            elide: Text.ElideRight
            wrapMode: Text.WrapAnywhere
            maximumLineCount: 1
            color: root.selected ? sysPalette.highlightText : sysPalette.text
        }
    }

    function renderText(t) {
        if (t === "")
            return t

        if (qmlUtils.binaryStringLength(t) > 1000) {
            return qmlUtils.printable(t, false, 1000) + "..."
        }

        return qmlUtils.printable(t)
                + (textItem.lineCount > 1 ? '...' : '')
    }

    MouseArea {
        anchors.fill: parent

        onClicked: {
            root.clicked()
        }
    }
}
