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
        id: textEditor
        Layout.fillWidth: true
        Layout.fillHeight: true
        value: ""
        enabled: root.active || root.state !== "edit"
        showFormatters: root.state == "edit"
    }

    function initEmpty() {
        textEditor.initEmpty()
    }

    function validateValue(callback) {
        return textEditor.validate(callback);
    }

    function setValue(rowValue) {
        if (!rowValue)
            return

        active = true
        textEditor.loadFormattedValue(rowValue['value'])
    }

    function isEdited() {
        return textEditor.isEdited
    }

    function getValue() {
        return {"value": textEditor.value}
    }

    function reset() {
        textEditor.reset()
        active = false
    }
}
