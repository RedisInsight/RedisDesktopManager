import QtQuick 2.0
import QtQuick.Layouts 1.1
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.1
import QtQuick.Dialogs 1.2

import "."

AbstractEditor {
    id: root
    anchors.fill: parent

    property bool active: false

    MultilineEditor {
        id: textArea
        Layout.fillWidth: true
        Layout.fillHeight: true
        value: ""
        enabled: root.active || root.state !== "edit"
        showFormatters: root.state == "edit"
    }

    function validateValue(callback) {
        return textArea.validate(callback);
    }

    function setValue(rowValue) {
        if (!rowValue)
            return

        active = true
        textArea.loadFormattedValue(rowValue['value'])
    }

    function isEdited() {
        return textArea.isEdited
    }

    function getValue() {
        return {"value": textArea.value}
    }

    function reset() {
        textArea.reset()        
        active = false
    }
}
