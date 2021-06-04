import QtQuick 2.13
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.13

SpinBox {
    id: control
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

    contentItem: TextInput {
        id: spinBoxTextInput
        z: 2
        text: control.displayText

        font: control.font
        color: control.palette.text
        selectionColor: control.palette.highlight
        selectedTextColor: control.palette.highlightedText
        horizontalAlignment: Qt.AlignHCenter
        verticalAlignment: Qt.AlignVCenter

        readOnly: !control.editable
        selectByMouse: control.editable
        validator: control.validator
        inputMethodHints: control.inputMethodHints

        Rectangle {
            x: -6 - (control.down.indicator ? 1 : 0)
            y: -6
            width: control.width - (control.up.indicator ? control.up.indicator.width - 1 : 0) - (control.down.indicator ? control.down.indicator.width - 1 : 0)
            height: control.height
            visible: control.activeFocus
            color: "transparent"
            border.color: control.palette.highlight
            border.width: 2
        }
    }

    onFocusChanged: {
        if (focus == true) {
            spinBoxTextInput.selectAll()
        }
    }
}
