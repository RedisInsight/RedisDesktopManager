import QtQuick 2.0
import QtQuick.Layouts 1.1
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.1
import QtQuick.Dialogs 1.2

import "."

AbstractEditor {
    id: root
    anchors.fill: parent

    property string originalValue: ""

    MultilineEditor {
        id: textArea
        Layout.fillWidth: true
        Layout.fillHeight: true
        text: ""
        enabled: originalValue != "" || !editingMode
    }

    function setValue(rowValue) {
        root.originalValue = rowValue['value']
        textArea.text = rowValue['value']
        textArea.binaryArray = rowValue['binary_value']

        console.log(rowValue['binary_value'])
    }

    function isValueChanged() {
        return originalValue != textArea.getText()
    }

    function resetAndDisableEditor() {
        root.originalValue = ""
        textArea.text = ""
    }

    function getValue() {
        return {"value": textArea.getText()}
    }

    function isValueValid() {
        var value = getValue()

        return value && value['value']
                && value['value'].length > 0
    }

    function markInvalidFields() {
        textArea.textColor = "black"
        // Fixme
    }


    function reset() {
        textArea.text = ""
    }
}
