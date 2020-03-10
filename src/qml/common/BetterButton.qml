import QtQuick 2.9
import QtQuick.Controls 2.3

Button {
    id: root

    property string iconSource
    property string tooltip

    icon.source: root.iconSource
    icon.width: 18
    icon.height: 18
    icon.color: "transparent"
    implicitHeight: 30


    ToolTip.visible: root.tooltip && hovered
    ToolTip.text: root.tooltip
}

