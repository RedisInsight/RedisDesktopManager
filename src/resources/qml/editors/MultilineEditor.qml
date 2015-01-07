import QtQuick 2.0
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.1

import "./formatters/formatters.js" as Formatters

ColumnLayout
{
    id: root
    property alias text: textArea.originalText
    property alias binaryText: textArea.binaryText
    property alias enabled: textArea.enabled
    property alias textColor: textArea.textColor

    function getText() {
        return textArea.formatter.getRaw(textArea.text)
    }

    RowLayout{
        Text {
            text: "View value as:"
        }

        ComboBox {
            id: formatterSelector
            width: 200
            model: ListModel {
                id: formattersModel
                ListElement { text: "PHP Serializer"; formatter: "php-serialized" }
                ListElement { text: "MSGPACK"; formatter: "msgpack" }
                ListElement { text: "Plain Text"; formatter: "plain" }
                ListElement { text: "JSON"; formatter: "json" }
            }
        }
    }

    TextArea
    {
        id: textArea
        Layout.fillWidth: true
        Layout.fillHeight: true

        text: {
            if (formattersModel.get(formatterSelector.currentIndex).formatter === "msgpack") {
                return binaryText? formatter.getFormatted(binaryText) : ''
            } else {
                return formatter.getFormatted(originalText)
            }
        }
        property string originalText
        property var binaryText
        property var formatter: Formatters.get(formattersModel.get(formatterSelector.currentIndex).formatter)
    }

    Component.onCompleted: {
        console.log("BINARY:", textArea.binaryText)
    }
}
