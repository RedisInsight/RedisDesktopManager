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
        id: keyText
        fieldLabel: qsTranslate("RDM","Key:")
        Layout.fillWidth: true
        Layout.minimumHeight: 30
        Layout.preferredHeight: root.state == "new"? 70: 140

        value: ""
        enabled: root.active || root.state !== "edit"
        showToolBar: root.state == "edit"
        showSaveBtn: root.state == "edit"
        showFormatters: root.state == "edit"
        objectName: "rdm_key_hash_key_field"
        formatterSettingsPrefix: "hash_key_"
    }

    MultilineEditor {
        id: textArea
        Layout.fillWidth: true
        Layout.fillHeight: true        
        enabled: root.active || root.state !== "edit"
        showToolBar: root.state == "edit"
        showFormatters: root.state == "edit"
        objectName: "rdm_key_hash_text_field"

        function validationRule(raw) {
            return true;
        }
    }

    function initEmpty() {
        keyText.initEmpty()
        textArea.initEmpty()
    }

    function validateValue(callback) {
        keyText.validate(function (keyTextValid) {            
            if (!root.validateVal) {
                return callback(keyTextValid);
            } else {
                textArea.validate(function (textAreaValid) {
                    return callback(keyTextValid && textAreaValid);
                });
            }
        });
    }

    function setValue(rowValue) {
        if (!rowValue)
            return       

        active = true
        keyText.loadFormattedValue(rowValue['key'])
        textArea.loadFormattedValue(rowValue['value'])
    }

    function isEdited() {
        return textArea.isEdited || keyText.isEdited
    }

    function getValue() {
        return {"value": textArea.value, "key": keyText.value}
    }

    function reset() {
        textArea.reset()
        keyText.reset()
        active = false
    }
}
