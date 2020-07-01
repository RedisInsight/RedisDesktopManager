import QtQuick 2.9
import QtQuick.Controls 2.3

CheckBox {
    id: checkBox

    palette.windowText: sysPalette.windowText

    indicator: Rectangle {
           implicitWidth: 16
           implicitHeight: 16
           x: checkBox.leftPadding
           y: parent.height / 2 - height / 2
           radius: 3
           color: checkBox.down ? sysPalette.dark : sysPalette.base
           border.color: sysPalette.dark

           Rectangle {
               width: 10
               height: 10
               x: 3
               y: 3
               radius: 2
               color: checkBox.down ? sysPalette.dark : sysPalette.text
               visible: checkBox.checked
           }
       }
}
