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
    property string keyType: ""

    MultilineEditor {
        id: textEditor
        Layout.fillWidth: true
        Layout.fillHeight: true
        value: ""
        enabled: root.active || root.state !== "edit"
        showToolBar: root.state == "edit"
        showSaveBtn: root.state == "edit"
        showFormatters: true
        showOnlyRWformatters: root.state == "add" || root.state == "new"
        objectName: "rdm_key_value_field"

        function validationRule(raw) {
            if (root.keyType === "string") return true;
            return qmlUtils.binaryStringLength(raw) > 0
        }
    }

    function initEmpty() {
        textEditor.initEmpty()
    }

    function validateValue(callback) {
        if (!root.validateVal) {
            return callback(true);
        }

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
