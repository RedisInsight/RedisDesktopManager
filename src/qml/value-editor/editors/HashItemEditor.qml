import QtQuick 2.0
import QtQuick.Layouts 1.1
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.1
import QtQuick.Dialogs 1.2

import "."

AbstractEditor {
    id: root
    anchors.fill: parent    

    MultilineEditor {
        id: keyText
        fieldLabel: qsTr("Key:")
        Layout.fillWidth: true
        Layout.minimumHeight: 80
        Layout.preferredHeight: 90

        value: ""
        enabled: originalValue != "" || root.state !== "edit"
        property var originalValue: ""
        showFormatters: root.state != "new"

        style: TextAreaStyle {
            backgroundColor: (!keyText.value && keyText.enabled
                              && keyText.readOnly == false) ? "lightyellow" : "white"
        }
    }


    MultilineEditor {
        id: textArea
        Layout.fillWidth: true
        Layout.fillHeight: true        
        enabled: keyText.originalValue != "" || root.state !== "edit"
        property var originalValue: ""
        showFormatters: root.state != "new"

        style: TextAreaStyle {
            backgroundColor: (!textArea.value && textArea.enabled
                              && textArea.readOnly == false) ? "lightyellow" : "white"
        }
    }

    function setValue(rowValue) {
        if (!rowValue)
            return       

        keyText.originalValue = rowValue['key']
        keyText.setValue(rowValue['key'])
        textArea.originalValue = rowValue['value']
        textArea.setValue(rowValue['value'])
    }

    function isValueChanged() {
        return textArea.originalValue != textArea.getText()
                || keyText.originalValue != keyText.text
    }

    function resetAndDisableEditor() {
        textArea.value = ""
        textArea.originalValue = ""
        keyText.originalValue = ""
        keyText.value = ""
    }

    function getValue() {
        return {"value": textArea.getText(), "key": keyText.getText()}
    }

    function isValueValid() {
        var value = getValue()

        return value && value['key'] && value['key'].length > 0
    }

    function markInvalidFields() {
        keyText.textColor = "black"
        textArea.textColor = "black"
        // Fixme
    }

    function reset() {
        textArea.originalValue = ""
        textArea.value = ""
        keyText.originalValue = ""
        keyText.value = ""
    }
}
