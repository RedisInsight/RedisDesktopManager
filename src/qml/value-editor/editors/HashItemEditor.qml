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
        fieldLabel: qsTr("Key:")
        Layout.fillWidth: true
        Layout.minimumHeight: 80
        Layout.preferredHeight: 90

        value: ""
        enabled: root.active || root.state !== "edit"
        showFormatters: root.state != "new"        
    }

    MultilineEditor {
        id: textArea
        Layout.fillWidth: true
        Layout.fillHeight: true        
        enabled: root.active || root.state !== "edit"
        showFormatters: root.state != "new"        
    }

    function validateValue(callback) {
        return textArea.validate(function (textAreaValid) {
            keyText.validate(function (keyTextValid) {
                return callback(textAreaValid && keyTextValid);
            });
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
