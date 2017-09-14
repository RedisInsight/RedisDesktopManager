import QtQuick 2.0

Item {
    id: root
    implicitWidth: 18
    implicitHeight: 18
    property alias imgWidth: img.width
    property alias imgHeight: img.height
    property alias imgSource: img.source

    signal clicked

    Image {
        id: img
        anchors.centerIn: parent
        source: "qrc:/images/clear.svg"
        width: 18
        height: 18
        sourceSize.width: width * 2
        sourceSize.height: height * 2
    }

    MouseArea {
        id: marea
        anchors.fill: parent
        onClicked: root.clicked()
    }
}

