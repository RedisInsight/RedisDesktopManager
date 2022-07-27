import QtQuick 2.0
import QtQuick.Layouts 1.1
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.1

import "."

AbstractEditor {
    id: root
    anchors.fill: parent

    property bool active: false    
    property alias defaultFormatter: textEditor.defaultFormatter

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

    onKeyTypeChanged: {
        if (root.keyType === "ReJSON") {
            textEditor.hintFormatter("JSON")
        }
    }

    function initEmpty() {
        textEditor.initEmpty()      
    }

    function getValue(validateVal, callback) {
        if (!validateVal) {
            return callback(true, {"value": ""});
        }

        return textEditor.validate(function (valid, raw) {
            return callback(valid, {"value": raw});
        });
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

    function reset() {
        textEditor.reset()
        active = false
    }
}
