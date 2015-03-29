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
        text: "Score:"
    }

    TextField {
        id: scoreText

        Layout.fillWidth: true
        Layout.minimumHeight: 28

        text: ""
        enabled: originalValue != "" || state !== "edit"
        property string originalValue: ""
        placeholderText: "Score"
        validator: DoubleValidator { locale: "C" } // force point as decimal separator
    }

    MultilineEditor {
        id: textArea
        Layout.fillWidth: true
        Layout.fillHeight: true
        text: ""
        enabled: originalValue != "" || state !== "edit"
        showFormatters: root.state != "new"
        property string originalValue: ""
    }

    function setValue(rowValue) {
        scoreText.originalValue = rowValue['score']
        scoreText.text = rowValue['score']
        textArea.originalValue = rowValue['value']
        textArea.text = rowValue['value']
        textArea.binaryArray = rowValue['binary_value']
    }

    function isValueChanged() {
        return textArea.originalValue != textArea.getText()
                || scoreText.originalValue != scoreText.text
    }

    function resetAndDisableEditor() {
        textArea.originalValue = ""
        textArea.text = ""
        scoreText.originalValue = ""
        scoreText.text = ""
    }

    function getValue() {
        return {"value": textArea.getText(), "score": scoreText.text}
    }    

    function isValueValid() {
        var value = getValue()

        return value && value['score'] && value['value']
                && value['score'].length > 0
                && value['value'].length > 0
    }

    function markInvalidFields() {
        scoreText.textColor = "black"
        textArea.textColor = "black"
        // Fixme
    }

    function reset() {
        textArea.text = ""
        scoreText.text = ""
    }

}
