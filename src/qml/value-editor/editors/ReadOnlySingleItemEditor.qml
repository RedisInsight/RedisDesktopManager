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
        enabled: false
        showToolBar: false
        showSaveBtn: false
        showFormatters: false
        showValueSize: false
        objectName: "rdm_key_value_field"

        function validationRule(raw) {
            return true;
        }
    }

    onKeyTypeChanged: {        
       textEditor.hintFormatter("JSON")
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
