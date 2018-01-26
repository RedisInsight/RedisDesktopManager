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

    Text {
        Layout.fillWidth: true
        text: qsTr("Score:")
    }

    TextField {
        id: scoreText

        Layout.fillWidth: true
        Layout.minimumHeight: 28

        text: ""
        enabled: root.active || root.state !== "edit"
        property string valueHash: ""
        placeholderText: qsTr("Score")
        validator: DoubleValidator { locale: "C"; notation: DoubleValidator.StandardNotation } // force point as decimal separator
        objectName: "rdm_key_zset_score_field"

        function setValue(v) {
            valueHash = Qt.md5(v)
            text = parseFloat(Number(v).toFixed(20))
        }

        function isEdited() {
            return Qt.md5(text) != valueHash
        }

        function reset() {
            text = ""
        }
    }

    MultilineEditor {
        id: textArea
        Layout.fillWidth: true
        Layout.fillHeight: true
        value: ""
        enabled: root.active || root.state !== "edit"
        showFormatters: root.state == "edit"
        objectName: "rdm_key_zset_text_field"
    }

    function initEmpty() {
        textArea.initEmpty()
    }

    function validateValue(callback) {
        return textArea.validate(callback);
    }

    function setValue(rowValue) {
        if (!rowValue)
            return

        active = true
        scoreText.setValue(rowValue['score'])
        textArea.loadFormattedValue(rowValue['value'])
    }

    function isEdited() {
        return textArea.isEdited || scoreText.isEdited()
    }

    function getValue() {
        return {"value": textArea.value, "score": scoreText.text}
    }    

    function reset() {
        root.active = false
        scoreText.reset()
        textArea.reset()
    }
}
