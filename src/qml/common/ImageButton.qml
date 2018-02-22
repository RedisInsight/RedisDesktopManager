import QtQuick 2.9
import QtQuick.Controls 2.2

Button {
    id: root    

    implicitWidth: 18
    implicitHeight: 18
    property alias imgWidth: img.width
    property alias imgHeight: img.height
    property alias imgSource: img.source    
    property alias iconSource: img.source

    property string tooltip

    Image {
        id: img
        anchors.centerIn: parent
        source: "qrc:/images/clear.svg"
        width: 18
        height: 18
        sourceSize.width: width * 2
        sourceSize.height: height * 2
    }

    background: Rectangle {
        implicitWidth: root.implicitWidth + 3
        implicitHeight: root.implicitHeight + 3
        opacity: enabled ? 1 : 0.3
        color: root.hovered ? "#eee" : "transparent"
        border.width: root.hovered ? 1 : 0
        border.color: "#eee"
        radius: 5
   }

   ToolTip.visible: hovered
   ToolTip.text: root.tooltip

}

