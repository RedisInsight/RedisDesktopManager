import QtQuick 2.9
import QtQuick.Controls 2.2

CheckBox {
    id: checkBox

    indicator: Rectangle {
           implicitWidth: 16
           implicitHeight: 16
           x: checkBox.leftPadding
           y: parent.height / 2 - height / 2
           radius: 3
           border.color: checkBox.down ? "grey" : "#ccccc"

           Rectangle {
               width: 10
               height: 10
               x: 3
               y: 3
               radius: 2
               color: checkBox.down ? "grey" : "#ccccc"
               visible: checkBox.checked
           }
       }
}
