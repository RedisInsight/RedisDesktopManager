import QtQuick 2.0
import QtQuick.Controls 1.4 as LC
import "./platformutils.js" as PlatformUtils

LC.TableView {
    rowDelegate: Rectangle {
        height: 30
        color: styleData.selected ? sysPalette.highlight : styleData.alternate? sysPalette.alternateBase : "transparent"
    }
}
