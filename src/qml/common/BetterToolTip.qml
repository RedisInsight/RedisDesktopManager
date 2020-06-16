import QtQuick 2.9
import QtQuick.Controls 2.3

ToolTip {
    property string title

    visible: title && hovered
    contentItem: Text {
           text: title
           color: sysPalette.text
    }

    background: Rectangle {
        border.width: 1
        color: sysPalette.base
        border.color: sysPalette.mid
    }
}
