import QtQuick 2.0
import QtQuick.Controls 2.13

ComboBox {
    id: root
    implicitHeight: 30

    palette.windowText: sysPalette.text
    palette.buttonText: sysPalette.text
    palette.highlightedText: sysPalette.text
    palette.highlight: sysPalette.highlight

    Connections {
        target: approot

        onPaletteChanged: {
            root.palette = approot.palette
            root.palette.windowText = sysPalette.text
            root.palette.buttonText = sysPalette.text
            root.palette.highlightedText = sysPalette.text
            root.palette.highlight = sysPalette.highlight
        }
    }
}
