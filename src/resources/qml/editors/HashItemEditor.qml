import QtQuick 2.0
import QtQuick.Layouts 1.1
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.1
import QtQuick.Dialogs 1.2

import "."

AbstractEditor {
    id: root
    anchors.fill: parent

    Text {
        Layout.fillWidth: true
        text: "Key:"
    }

    TextField {
        id: keyText

        Layout.fillWidth: true
        Layout.minimumHeight: 35

        text: ""
        enabled: originalValue != "" || !editingMode
        property string originalValue: ""
    }

    Text {
        Layout.fillWidth: true
        text: "Value:"
    }

    TextArea {
        id: textArea
        Layout.fillWidth: true
        Layout.fillHeight: true
        text: ""
        enabled: originalValue != "" || !editingMode
        property string originalValue: ""
    }

    function setValue(rowValue) {

        if (!rowValue)
            return

        keyText.originalValue = rowValue['key']
        keyText.text = rowValue['key']
        textArea.originalValue = rowValue['value']
        textArea.text = rowValue['value']
    }

    function isValueChanged() {
        return textArea.originalValue != textArea.text
                || keyText.originalValue != keyText.text
    }

    function resetAndDisableEditor() {
        textArea.originalValue = ""
        textArea.text = ""
        keyText.originalValue = ""
        keyText.text = ""
    }

    function getValue() {
        return {"value": textArea.text, "key": keyText.text}
    }
}
