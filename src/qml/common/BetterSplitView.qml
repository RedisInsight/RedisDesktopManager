import QtQuick 2.0
import QtQuick.Controls 2.13

SplitView {
    handle: Rectangle {
        implicitWidth: parent.orientation == Qt.Horizontal ? 3 : parent.width
        implicitHeight: parent.orientation == Qt.Vertical ? 3 : parent.height
        border.color: sysPalette.midlight
        border.width: 1
        color: sysPalette.mid
    }
}
