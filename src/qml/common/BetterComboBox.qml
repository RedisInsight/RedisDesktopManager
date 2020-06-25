import QtQuick 2.0
import QtQuick.Controls 2.13

ComboBox {
    id: root
    implicitHeight: 30

    palette.button: sysPalette.button
    palette.text: sysPalette.text
    palette.windowText: sysPalette.text
    palette.buttonText: sysPalette.text
    palette.highlightedText: sysPalette.buttonText
    palette.highlight: sysPalette.highlight
    palette.dark: sysPalette.dark
    palette.window: sysPalette.window
}
