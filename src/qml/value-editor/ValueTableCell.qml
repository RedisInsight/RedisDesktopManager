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
        border.color: root.selected ? sysPalette.midlight : sysPalette.mid
        border.width: 1
        color: root.selected ? sysPalette.highlight : sysPalette.base
        clip: true

        Text {
            id: textItem
            anchors.centerIn: parent
            elide: Text.ElideRight
            wrapMode: Text.WrapAnywhere
            maximumLineCount: 1
            color: root.selected ? sysPalette.highlightedText : sysPalette.text
            textFormat: Text.PlainText
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

        onClicked: {
            root.clicked()
        }
    }
}
