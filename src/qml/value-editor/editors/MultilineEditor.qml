import QtQuick 2.0
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.2
import QtQuick.Layouts 1.1
import Qt.labs.settings 1.0
import "../../common/"
import "./formatters/formatters.js" as Formatters

ColumnLayout
{
    id: root

    property bool enabled
    property string textColor
    property string backgroundColor
    property bool showFormatters: true
    property string fieldLabel: qsTr("Value:")    
    property bool isEdited: false
    property var value    
    property int valueSizeLimit: 150000

    function initEmpty() {
        // init editor with empty model
        textView.model = qmlUtils.wrapLargeText("")
        textView.readOnly = false
        textView.textFormat = TextEdit.PlainText
    }

    function validationRule(raw)
    {
        return qmlUtils.binaryStringLength(raw) > 0
    }

    function validate(callback) {
        loadRawValue(function (error, raw) {            

            if (error) {                
                notification.showError(error)
                return callback(false);
            }            

            var valid = validationRule(raw)

            if (valid) {
                hideValidationError()
            } else {
                showValidationError("Enter value")
            }

            return callback(valid)
        });
    }

    function loadRawValue(callback) {                       
       var formatter = formatterSelector.model[formatterSelector.currentIndex]

        formatter.instance.getRaw(textView.model.getText(), function (error, raw) {
            root.value = raw
            return callback(error, raw)
        })
    }

    function loadFormattedValue(val) {
        if (val) {
            root.value = val
        }

        if (!root.value) {
            console.log("Empty value. Skipping formatting stage");
            return;
        }

        if (binaryUtils.binaryStringLength(root.value) > valueSizeLimit) {
            root.showFormatters = false
            formatterSelector.currentIndex = 0
        } else {
            root.showFormatters = true
        }

        var isBin = binaryUtils.isBinaryString(root.value)
        binaryFlag.visible = isBin

        var formatter = formatterSelector.model[formatterSelector.currentIndex]

        uiBlocker.visible = true                

        formatter.instance.getFormatted(root.value, function (error, formatted, isReadOnly, format) {

            if (error || !formatted) {
                uiBlocker.visible = false
                formatterSelector.currentIndex = isBin? 2 : 0 // Reset formatter to plain text
                notification.showError(error || qsTr("Unknown formatter error (Empty response)"))
                return
            }

            if (format === "json") {
                // 1 is JSON
                return formatterSelector.model[1].instance.getFormatted(formatted, function (formattedJson, r, f) {
                    textView.model = qmlUtils.wrapLargeText(formattedJson)
                    textView.readOnly = isReadOnly
                    textView.textFormat = TextEdit.PlainText
                    root.isEdited = false
                    uiBlocker.visible = false
                })
            } else {                
                textView.model = qmlUtils.wrapLargeText(formatted)
                textView.readOnly = isReadOnly
                root.isEdited = false

                if (format === "html")
                    textView.textFormat = TextEdit.RichText
                else
                    textView.textFormat = TextEdit.PlainText
            }

            uiBlocker.visible = false
        })
    }

    function reset() {
        if (textView.model)
            textView.model.cleanUp()

        if (textView.model) {
            qmlUtils.deleteTextWrapper(textView.model)
        }

        textView.model = null
        root.value = ""
        root.isEdited = false
        hideValidationError()
    }

    function showValidationError(msg) {
        validationError.text = msg
        validationError.visible = true
    }

    function hideValidationError() {
        validationError.visible = false
    }

    RowLayout{
        Layout.fillWidth: true

        Text { text: root.fieldLabel }
        TextEdit { text: qsTr("size: ") + binaryUtils.humanSize(binaryUtils.binaryStringLength(value)); readOnly: true; color: "#ccc"  }
        Text { id: binaryFlag; text: qsTr("[Binary]"); visible: false; color: "green"; }        
        Item { Layout.fillWidth: true }

        ImageButton {
            iconSource: "qrc:/images/copy.svg"
            tooltip: qsTr("Copy to Clipboard")

            onClicked: {
                if (textView.model) {
                    qmlUtils.copyToClipboard(textView.model.getText())
                }
            }
        }

        Text { visible: showFormatters; text: qsTr("View as:") }

        ComboBox {
            id: formatterSelector
            visible: showFormatters
            width: 200
            model: Formatters.buildFormattersModel()
            textRole: "name"
            objectName: "rdm_value_editor_formatter_combobox"

            onCurrentIndexChanged: loadFormattedValue()

            Settings {
                id: defaultFormatterSettings
                category: "formatters"
                property alias defaultFormatterIndex: formatterSelector.currentIndex
            }
        }

        Text { visible: !showFormatters && binaryUtils.binaryStringLength(root.value) > valueSizeLimit; text: qsTr("Large value (>150kB). Formatters is not available."); color: "red"; }
    }

    Rectangle {
        id: texteditorWrapper
        Layout.fillWidth: true
        Layout.fillHeight: true
        Layout.preferredHeight: 100        

        color: "white"
        border.color: "#cccccc"
        border.width: 1

        ScrollView {
            anchors.fill: parent
            anchors.margins: 5

            ListView {
                id: textView
                anchors.fill: parent
                cacheBuffer: 0

                property int textFormat: TextEdit.PlainText
                property bool readOnly: false

                delegate:
                    NewTextArea {
                        id: textAreaPart
                        objectName: "rdm_key_multiline_text_field_" + index
                        width: textView.width
                        height: textAreaPart.contentHeight < texteditorWrapper.height? texteditorWrapper.height - 5 : textAreaPart.contentHeight

                        enabled: root.enabled
                        text: value

                        textFormat: textView.textFormat
                        readOnly: textView.readOnly

                        onTextChanged: {
                            root.isEdited = true
                            textView.model && textView.model.setTextChunk(index, textAreaPart.text)
                        }
                    }
                }
            }            
    }

    Text {
        id: validationError
        color: "red"
        visible: false
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
