import QtQuick 2.0
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.2
import QtQuick.Layouts 1.1
import QtWebEngine 1.0

import "./formatters/formatters.js" as Formatters

ColumnLayout
{
    id: root

    property bool enabled
    property string textColor
    property string backgroundColor
    property bool showFormatters: true
    property string fieldLabel: qsTr("Value:")
    property var value

    function getText() {
        // FIXME
    }

    function setValue(val) {
        value = val
        loadFormattedValue()
    }

    function loadFormattedValue() {
        var isBin = binaryUtils.isBinaryString(root.value)

        binaryFlag.visible = false        

        if (isBin) binaryFlag.visible = true

        //formatterSelector.currentIndex = Formatters.guessFormatter(isBin)

        var formatter = formatterSelector.model[formatterSelector.currentIndex]

        uiBlocker.visible = true

        formatter.instance.getFormatted(root.value, function (formatted, isReadOnly, format) {

// TODO: fixme
//            if (format == "json") {
//                // 1 is JSON
//                return formatterSelector.model[1].instance.getFormatted(formatted, function (formattedJson, r, f) {
//                    webView.setText(formattedJson, false, isReadOnly)
//                    uiBlocker.visible = false
//                })
//            } else {
//                webView.setText(formatted, format === "html", isReadOnly)
//            }

//            uiBlocker.visible = false
        })
    }

    RowLayout{
        visible: showFormatters
        Layout.fillWidth: true

        Text { text: root.fieldLabel }
        TextEdit { text: "size: " + binaryUtils.humanSize(binaryUtils.binaryStringLength(value)); readOnly: true; color: "#ccc"  }
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

//    WebEngineView {
//        id: webView
//        Layout.fillWidth: true
//        Layout.fillHeight: true
//        Layout.preferredHeight: 100

//        onJavaScriptConsoleMessage: {
//            console.log("Web:", message)
//        }

//        function setText(text, html, readOnly) {

//            if (html) {
//                webView.loadHtml("<html><body style='width: 100%; height: 100%;'>" + text +"</body>")
//            } else {
//                var attr = "";

//                if (readOnly) {
//                    attr = "readonly";
//                }

//                webView.loadHtml("<html><body><textarea "+ attr +" id='textarea' style='width: 100%; height: 100%; resize: none;'>" + text +"</textarea></body>")
//            }

//        }
//    }


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
