import QtQuick 2.13
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.13

SpinBox {
    implicitHeight: 30
    editable: true
    textFromValue: renderText

    function renderText(value, locale) { return value }

    palette.text: sysPalette.text
    palette.highlight: sysPalette.highlight
    palette.highlightedText: sysPalette.highlightedText
    palette.button : sysPalette.button
    palette.base : sysPalette.base
    palette.mid: disabledSysPalette.highlight
    palette.buttonText: sysPalette.dark
}
