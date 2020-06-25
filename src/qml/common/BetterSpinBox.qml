import QtQuick 2.3
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.3

SpinBox {
    implicitHeight: 30
    editable: true
    textFromValue: renderText

    function renderText(value, locale) { return value }

    palette.text: sysPalette.windowText
    palette.highlight: sysPalette.highlight
    palette.highlightedText: sysPalette.highlightedText
    palette.button : sysPalette.button
    palette.base : sysPalette.base
    palette.mid: sysPalette.mid
    palette.buttonText: sysPalette.buttonText
}
