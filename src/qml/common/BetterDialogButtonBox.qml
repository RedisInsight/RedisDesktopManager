import QtQuick 2.13
import QtQuick.Controls 2.13

DialogButtonBox {
  spacing: 3

  background: Rectangle {
      implicitHeight: 40
      x: 1; y: 1
      width: parent.width - 2
      height: parent.height - 2
      color: sysPalette.base
  }
}
