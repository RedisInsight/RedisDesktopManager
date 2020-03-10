import QtQuick 2.0
import QtQuick.Controls 2.13
import QtQuick.Layouts 1.1
import Qt.labs.settings 1.0
import "../../common/"

Item
{
    id: root

    property bool enabled
    property string textColor
    property bool showFormatters: true
    property string fieldLabel: qsTranslate("RDM","Value") + ":"
    property bool isEdited: false
    property var value
    property int valueSizeLimit: 150000
    property int valueCompression: 0
    property string formatterSettingsCategory: "formatters_value"    
    property alias readOnly: textView.readOnly

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
                showValidationError(qsTranslate("RDM", "Enter valid value"))
            }

            return callback(valid)
        });
    }

    function compress(val) {
        if (valueCompression > 0) {
            return qmlUtils.compress(val, valueCompression)
        } else {
            return val
        }
    }

    function loadRawValue(callback) {                
        if (formatterSelector.visible) {

            function process(formattedValue) {
                var formatter = formatterSelector.model[formatterSelector.currentIndex]

                 formatter.getRaw(formattedValue, function (error, raw) {
                     root.value = compress(raw)
                     return callback(error, compress(raw))
                 })
            }

            if (textView.format === "json") {
                Formatters.json.getRaw(textView.model.getText(), function (jsonError, plainText) {
                    if (jsonError) {
                        return callback(jsonError, "")
                    }

                    process(plainText)
                })
            } else {
                process(textView.model.getText())
            }
        } else {
            root.value = compress(textView.model.getText())
            return callback("", root.value)
        }
    }

    function loadFormattedValue(val) {
        var guessFormatter = false

        if (val) {
            root.value = val

            if (defaultFormatterSettings.defaultFormatterIndex === 0) {
                guessFormatter = true
            } else {
                formatterSelector.currentIndex = defaultFormatterSettings.defaultFormatterIndex
            }
        }

        if (!root.value) {
            console.log("Empty value. Skipping formatting stage");
            return;
        }

        if (qmlUtils.binaryStringLength(root.value) > valueSizeLimit) {
            root.showFormatters = false
            formatterSelector.currentIndex = 0
            guessFormatter = false
        } else {
            root.showFormatters = true
        }

        var isBin = qmlUtils.isBinaryString(root.value)
        binaryFlag.visible = isBin

        valueCompression = qmlUtils.isCompressed(root.value)

        if (valueCompression > 0) {
            root.value = qmlUtils.decompress(root.value)
            isBin = qmlUtils.isBinaryString(root.value)
        }

        // If current formatter is plain text - try to guess formatter
        if (guessFormatter && formatterSelector.currentIndex === 0) {
            _guessFormatter(isBin, function() {
                _loadFormatter(isBin)
            })
        } else {
            _loadFormatter(isBin)
        }
    }

    function _guessFormatter(isBin, callback) {
        console.log("Guessing formatter")

        var candidates = valueFormattersModel.guessFormatter(isBin)

        console.log("candidates:", candidates)

        if (Array.isArray(candidates)) {

            for (var index in candidates) {
                var cFormatter = formatterSelector.model[candidates[index]]

                cFormatter.isValid(root.value, function (isValid) {
                    if (isValid && formatterSelector.currentIndex == 0) {
                        formatterSelector.currentIndex = candidates[index]
                        callback()
                    }
                })

                if (formatterSelector.currentIndex !== 0)
                    break
            }
        } else {
            formatterSelector.currentIndex = candidates
            callback()
        }
    }

    function _loadFormatter(isBin) {
        if (!(0 < formatterSelector.currentIndex
              && formatterSelector.currentIndex < formatterSelector.count)) {
            formatterSelector.currentIndex = isBin? 2 : 0
        }

        var formatter = formatterSelector.model.get(formatterSelector.currentIndex)

        uiBlocker.visible = true

        if (formatter['name'] === 'JSON') {
            jsonFormattingWorker.sendMessage(String(root.value))
        } else {
            formatter.getFormatted(root.value, function (error, formatted, isReadOnly, format) {

                function process(error, formatted, stub, format) {
                    jsonFormattingWorker.processFormatted(error, formatted, stub, format)
                }

                textView.format = format

                if (format === "json") {
                    jsonFormattingWorker.sendMessage(String(formatted))
                } else {
                    process(error, formatted, isReadOnly, format);
                }
            })
        }
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
        root.valueCompression = 0
        hideValidationError()
    }

    function showValidationError(msg) {
        validationError.text = msg
        validationError.visible = true
    }

    function hideValidationError() {
        validationError.visible = false
    }

    WorkerScript {
        id: jsonFormattingWorker

        source: "./formatters/json-tools.js"
        onMessage: {
            textView.format = messageObject.format
            processFormatted(messageObject.error, messageObject.formatted, messageObject.isReadOnly, messageObject.format);
        }

        function processFormatted(error, formatted, isReadOnly, format) {
            if (error || !formatted) {
                uiBlocker.visible = false
                formatterSelector.currentIndex = isBin? 2 : 0 // Reset formatter to plain text
                notification.showError(error || qsTranslate("RDM","Unknown formatter error (Empty response)"))
                return
            }

            textView.textFormat = (format === "html")
                ? TextEdit.RichText
                : TextEdit.PlainText;

            defaultFormatterSettings.defaultFormatterIndex = formatterSelector.currentIndex
            textView.model = qmlUtils.wrapLargeText(formatted)
            textView.readOnly = isReadOnly
            root.isEdited = false
            uiBlocker.visible = false
        }
    }


    ColumnLayout {
        anchors.fill: parent

        RowLayout{
            Layout.fillWidth: true

            Text { text: root.fieldLabel }
            TextEdit {
                Layout.preferredWidth: 150
                text: qsTranslate("RDM", "Size: ") + qmlUtils.humanSize(qmlUtils.binaryStringLength(value));
                readOnly: true;
                selectByMouse: true
                color: "#ccc"
            }
            Text { id: binaryFlag; text: qsTranslate("RDM","[Binary]"); visible: false; color: "green"; }
            Text { text: qsTranslate("RDM"," [Compressed: ") + qmlUtils.compressionAlgName(root.valueCompression) + "]"; visible: root.valueCompression > 0; color: "red"; }
            Item { Layout.fillWidth: true }

            ImageButton {
                iconSource: "qrc:/images/copy.svg"
                tooltip: qsTranslate("RDM","Copy to Clipboard")

                onClicked: {
                    if (textView.model) {
                        qmlUtils.copyToClipboard(textView.model.getText())
                    }
                }
            }

            Text { visible: showFormatters; text: qsTranslate("RDM","View as:") }

            Settings {
                id: defaultFormatterSettings
                category: formatterSettingsCategory
                property int defaultFormatterIndex
            }

            BetterComboBox {
                id: formatterSelector
                visible: showFormatters
                width: 200
                model: valueFormattersModel
                textRole: "name"
                objectName: "rdm_value_editor_formatter_combobox"

                onActivated: {
                    currentIndex = index
                    loadFormattedValue()
                }
            }

            Text { visible: !showFormatters && qmlUtils.binaryStringLength(root.value) > valueSizeLimit; text: qsTranslate("RDM","Large value (>150kB). Formatters is not available."); color: "red"; }
        }

        Rectangle {
            id: texteditorWrapper
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.preferredHeight: 100

            color: "white"
            border.color: "#cccccc"
            border.width: 1
            clip: true

            ScrollView {
                anchors.fill: parent
                anchors.margins: 5

                ScrollBar.vertical.policy: ScrollBar.AlwaysOn

                ListView {
                    id: textView
                    anchors.fill: parent
                    cacheBuffer: 0

                    property int textFormat: TextEdit.PlainText
                    property bool readOnly: false
                    property string format

                    delegate:
                        Item {
                            width: texteditorWrapper.width
                            height: textAreaPart.contentHeight < texteditorWrapper.height? texteditorWrapper.height - 5 : textAreaPart.contentHeight

                            NewTextArea {
                                anchors.fill: parent
                                id: textAreaPart
                                objectName: "rdm_key_multiline_text_field_" + index


                                enabled: root.enabled
                                text: value

                                textFormat: textView.textFormat
                                readOnly: textView.readOnly

                                onPreeditTextChanged: {
                                    root.isEdited = true
                                    textView.model && textView.model.setTextChunk(index, textAreaPart.text)
                                }

                                onEditingFinished: {
                                    root.isEdited = true
                                    textView.model && textView.model.setTextChunk(index, textAreaPart.text)
                                }                                                                
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
