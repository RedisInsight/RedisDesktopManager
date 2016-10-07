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
    property string fieldLabel: "Value:"
    property var value

    function getText() {
        if (textArea.formatter.binary)
            return binaryUtils.binaryListToValue(textArea.formatter.getRaw(textArea.text))
        else
            return textArea.formatter.getRaw(textArea.text)
    }

    function setValue(val) {
        value = val
        var isBin = binaryUtils.isBinaryString(val)

        binaryFlag.visible = false

        if (isBin) binaryFlag.visible = true        

        autoDetectFormatter(isBin)
    }

    function autoDetectFormatter(isBinary) {
        formatterSelector.currentIndex = Formatters.guessFormatter(isBinary, value)
    }

    RowLayout{
        visible: showFormatters
        Layout.fillWidth: true

        Text { text: root.fieldLabel }
        Text { id: binaryFlag; text: qsTr("[Binary]"); visible: false; color: "green"; }
        Text { id: compressedFlag; text: qsTr("[GZIP compressed]"); visible: false; color: "red"; } // TBD
        Item { Layout.fillWidth: true }
        Text { text: "View as:" }

        ComboBox {
            id: formatterSelector
            width: 200
            model: formattersModel
            textRole: "name"

            onCurrentIndexChanged: Formatters.defaultFormatterIndex = currentIndex
            Component.onCompleted: currentIndex = Formatters.defaultFormatterIndex
        }

        ListModel {
            id: formattersModel

            Component.onCompleted: {
                for (var index in Formatters.enabledFormatters) {
                    var f = Formatters.enabledFormatters[index]
                    var title = f.readOnly ? f.title + " (READ ONLY)" : f.title
                    append({'name': title})
                }
            }
        }
    }

    TextArea
    {
        id: textArea
        Layout.fillWidth: true        
        Layout.fillHeight: true
        Layout.preferredHeight: 100        
        textFormat: formatter && formatter.htmlOutput ? TextEdit.RichText : TextEdit.PlainText
        readOnly: (formatter)? formatter.readOnly : enabled ? true : false

        onEnabledChanged: {
            console.log("Text editor was disabled")
        }

        text: {
            if (!formatter || !value)
                return ''

            if (formatter.binary === true) {
                return formatter.getFormatted(binaryUtils.valueToBinary(value)) || ''
            } else {
                return formatter.getFormatted(binaryUtils.toUtf(value)) || ''
            }
        }

        property var formatter: {
            var index = formatterSelector.currentIndex ? formatterSelector.currentIndex : Formatters.defaultFormatterIndex
            return Formatters.enabledFormatters[index]
        }

        style: TextAreaStyle {
            renderType: Text.QtRendering
        }
        font { family: monospacedFont.name; pointSize: 12 }
        wrapMode: TextEdit.WrapAnywhere
    }
}
