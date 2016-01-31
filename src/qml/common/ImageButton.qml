import QtQuick 2.0

Item {
    id: root
    property alias imgWidth: img.width
    property alias imgHeight: img.height
    property alias imgSource: img.source

    signal clicked

    Image {
        id: img
        anchors.centerIn: parent
        source: "qrc:/images/clear.png"
        width: 18
        height: 18
    }

    MouseArea {
        anchors.fill: parent
        onClicked: root.clicked()
    }
}

