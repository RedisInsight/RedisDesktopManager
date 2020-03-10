import QtQuick 2.9
import QtQuick.Controls 2.3

TextField {
    id: control
    property bool validationError: false
    selectByMouse: true
    mouseSelectionMode: TextInput.SelectWords

    background: Rectangle {
        implicitWidth: 200
        implicitHeight: 30
        color: control.palette.base
        border.width: control.validationError? 2 : 1
        border.color: {
            if (control.validationError)
                return "#d12f24"

            return control.activeFocus ? control.palette.highlight : control.palette.mid
        }
    }
}
