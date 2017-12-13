import QtQuick 2.0

Text {
    property string html
    property string styleString: '<style>a {color: darkblue; text-decoration: none; vertical-align: top;}</style>'

    textFormat: Qt.RichText
    text: styleString  + html
    wrapMode: Text.Wrap
    onLinkActivated: Qt.openUrlExternally(link)

    MouseArea {
        anchors.fill: parent
        cursorShape: parent.hoveredLink ? Qt.PointingHandCursor : Qt.ArrowCursor
        acceptedButtons: Qt.NoButton
    }
}
