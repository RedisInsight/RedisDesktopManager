import QtQuick 2.0
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.2
import QtQuick.Layouts 1.1

import "./formatters/formatters.js" as Formatters

ColumnLayout
{
    id: root

    property alias enabled: textArea.enabled
    property alias textColor: textArea.textColor
    property alias style: textArea.style
    property bool showFormatters: true
    property string fieldLabel: qsTr("Value:")
    property var value

    function getText() {
        return textArea.formatter.getRaw(textArea.text)
    }

    function setValue(val) {
        value = val
        loadFormattedValue()
    }

    function loadFormattedValue() {
        var isBin = binaryUtils.isBinaryString(root.value)

        binaryFlag.visible = false
        textArea.textFormat = TextEdit.PlainText

        if (isBin) binaryFlag.visible = true

        // FIXME: autoDetectFormatter

        var formatter = formatterSelector.model[formatterSelector.currentIndex]

        uiBlocker.visible = true

        formatter.instance.getFormatted(root.value, function (formatted, isReadOnly, format) {

            if (isReadOnly) {
                textArea.readOnlyValue = true
            }

            if (format == "json") {
                // 1 is JSON
                return formatterSelector.model[1].instance.getFormatted(formatted, function (formattedJson, r, f) {
                    textArea.text = formattedJson
                    uiBlocker.visible = false
                })
            } else {
                if (format == "html")
                    textArea.textFormat = TextEdit.RichText

                textArea.text = formatted
            }

            uiBlocker.visible = false
        })
    }

    RowLayout{
        visible: showFormatters
        Layout.fillWidth: true

        Text { text: root.fieldLabel }
        Text { id: binaryFlag; text: qsTr("[Binary]"); visible: false; color: "green"; }        
        Item { Layout.fillWidth: true }
        Text { text: "View as:" }

        ComboBox {
            id: formatterSelector
            width: 200
            model: Formatters.buildFormattersModel()
            textRole: "name"

            onCurrentIndexChanged: {
                Formatters.defaultFormatterIndex = currentIndex
                loadFormattedValue()
            }
            Component.onCompleted: currentIndex = Formatters.defaultFormatterIndex
        }       
    }

    TextArea
    {
        id: textArea
        Layout.fillWidth: true        
        Layout.fillHeight: true
        Layout.preferredHeight: 100        

        readOnly: (readOnlyValue)? readOnlyValue : enabled ? true : false

        property bool readOnlyValue: false

        style: TextAreaStyle { renderType: Text.QtRendering }

        font { family: monospacedFont.name; pointSize: 12 }

        wrapMode: TextEdit.WrapAnywhere
    }

    Rectangle {
        id: uiBlocker
        visible: false
        anchors.fill: parent
        color: Qt.rgba(0, 0, 0, 0.1)

        Item {
            anchors.fill: parent
            BusyIndicator { anchors.centerIn: parent; running: true }
        }

        MouseArea { anchors.fill: parent }
    }
}
