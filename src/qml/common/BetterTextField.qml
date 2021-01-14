import QtQuick 2.9
import QtQuick.Controls 2.3

TextField {
    id: control
    property bool validationError: false
    property int  bgImplicitWidth: 200
    property int  bgImplicitHeight: 30
    property string tooltip

    selectByMouse: true

    color: sysPalette.text
    selectionColor: sysPalette.highlight
    selectedTextColor: sysPalette.highlightedText

    background: Rectangle {
        implicitWidth: control.bgImplicitWidth
        implicitHeight: control.bgImplicitHeight
        color: sysPalette.button
        border.width: control.validationError? 2 : 1
        border.color: {
            if (control.validationError || !control.acceptableInput)
                return "#d12f24"

            return control.activeFocus ? sysPalette.highlight : sysPalette.mid
        }
    }

    BetterToolTip {
        title: control.tooltip
    }
}
