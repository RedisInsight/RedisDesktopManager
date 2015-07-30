import QtQuick 2.0
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.1

import "./formatters/formatters.js" as Formatters

ColumnLayout
{
    id: root
    property alias text: textArea.originalText
    property alias binaryArray: textArea.binaryArray
    property alias enabled: textArea.enabled
    property alias textColor: textArea.textColor
    property alias style: textArea.style
    property bool showFormatters: true

    function getText() {
        return textArea.formatter.getRaw(textArea.text)
    }

    RowLayout{
        visible: showFormatters

        Text { text: "Value:" }
        Item { Layout.fillWidth: true }
        Text { text: "View value as:" }

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
        textFormat: formatter && formatter.readOnly? TextEdit.RichText : TextEdit.PlainText
        readOnly: (formatter)? formatter.readOnly : enabled ? true : false

        onEnabledChanged: {
            console.log("Text editor was disabled")
        }

        text: {
            if (!formatter) return ''
            if (formatter.binary === true) return binaryArray? formatter.getFormatted(binaryArray) : ''
            else return formatter.getFormatted(originalText)
        }
        property string originalText
        property var binaryArray
        property var formatter: {
            var index = formatterSelector.currentIndex ? formatterSelector.currentIndex : Formatters.defaultFormatterIndex
            return Formatters.enabledFormatters[index]
        }
    }
}
