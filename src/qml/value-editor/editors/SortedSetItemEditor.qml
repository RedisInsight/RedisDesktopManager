import QtQuick 2.0
import QtQuick.Layouts 1.1
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.1

import "."
import "../../common"

AbstractEditor {
    id: root
    anchors.fill: parent

    property bool active: false
    property alias defaultFormatter: textArea.defaultFormatter

    BetterLabel {
        Layout.fillWidth: true
        text: qsTranslate("RESP", "Score")
    }

    BetterTextField {
        id: scoreText

        Layout.fillWidth: true
        Layout.minimumHeight: 28

        text: ""
        enabled: root.active || root.state !== "edit"

        placeholderText: qsTranslate("RESP","Score")
        validator: DoubleValidator { locale: "C"; } // force point as decimal separator
        objectName: "rdm_key_zset_score_field"

        property bool isEdited: false

        onTextChanged: {
            scoreText.isEdited = true
        }

        function setValue(v) {
            text = Number(v)
            scoreText.isEdited = false
        }

        Connections {
            target: keyTab.keyModel ? keyTab.keyModel : null

            onValueUpdated: scoreText.isEdited = false
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
        showToolBar: root.state == "edit"
        showSaveBtn: root.state == "edit"
        showFormatters: root.state == "edit"
        objectName: "rdm_key_zset_text_field"
    }

    function initEmpty() {
        textArea.initEmpty()
    }

    function getValue(validateVal, callback) {
        if (!validateVal) {
            return callback(true, {"value": "", "score": scoreText.text});
        }

        return textArea.validate(function (valid, raw) {
            callback(valid, {"value": raw, "score": scoreText.text})
        });
    }

    function setValue(rowValue) {
        if (!rowValue)
            return

        active = true
        scoreText.setValue(rowValue['score'])
        textArea.loadFormattedValue(rowValue['value'])
    }

    function isEdited() {
        return textArea.isEdited || scoreText.isEdited
    }

    function reset() {
        root.active = false
        scoreText.reset()
        textArea.reset()
    }
}
