import QtQuick 2.0
import QtQuick.Layouts 1.1
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.1
import QtQuick.Dialogs 1.2

import "."
import "../../common"

AbstractEditor {
    id: root
    anchors.fill: parent    

    property bool active: false

    BetterLabel {
        Layout.fillWidth: true
        text: qsTranslate("RDM","ID")
    }

    BetterTextField {
            id: idValue

            Layout.fillWidth: true
            Layout.minimumHeight: 28

            text: ""
            objectName: "rdm_key_stream_id_field"
            property string valueHash: ""
            enabled: root.active || root.state !== "edit"
            readOnly: root.state == "edit"

            function reset() {
                text = (root.state == "add" || root.state == "new")? "*" : ""
            }

            function isEdited() {
                return Qt.md5(text) != valueHash
            }

            function setValue(v) {
                valueHash = Qt.md5(v)
                text = v
            }

            function validate(callback) {
                return callback(text == "*" || text.indexOf("-") !== -1);
            }
    }

    MultilineEditor {
        id: textArea
        Layout.fillWidth: true
        Layout.fillHeight: true        
        enabled: root.active || root.state !== "edit"
        showToolBar: root.state == "edit"
        showSaveBtn: root.state == "edit"
        showFormatters: root.state == "edit"
        objectName: "rdm_key_stream_text_field"

        fieldLabel: qsTranslate("RDM","Value (represented as JSON object)") + ":"

        function validationRule(raw) {
            try {
                var obj = JSON.parse(textArea.value);

                return typeof obj === "object";
            } catch (e) {
                console.log("Json parsing error:", e)
                return false
            }
        }
    }

    function initEmpty() {        
        textArea.initEmpty()
        idValue.reset()
    }

    function validateValue(callback) {
        idValue.validate(function (keyTextValid) {
            textArea.validate(function (textAreaValid) {
                return callback(keyTextValid && textAreaValid);
            });
        });
    }

    function setValue(rowValue) {
        if (!rowValue)
            return       

        active = true
        idValue.setValue(rowValue['id'])
        textArea.loadFormattedValue(rowValue['value'])
        textArea.readOnly = root.state == "edit"
    }

    function isEdited() {
        return textArea.isEdited || idValue.isEdited()
    }

    function getValue() {
        return {"value": textArea.value, "id": idValue.text}
    }

    function reset() {
        textArea.reset()
        idValue.reset()
        active = false
    }
}
