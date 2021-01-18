import QtQuick 2.5
import QtQuick.Controls 2.13
import QtQuick.Layouts 1.1
import QtQuick.Window 2.2
import Qt.labs.settings 1.0
import "../../common/"
import "../../common/platformutils.js" as PlatformUtils

Item
{
    id: root

    property bool enabled
    property string textColor
    property int imgBtnWidth: PlatformUtils.isOSXRetina(Screen)? 18 : 22
    property int imgBtnHeight: PlatformUtils.isOSXRetina(Screen)? 18 : 22
    property bool showToolBar: false
    property bool showSaveBtn: false
    property bool showFormatters: true
    property bool showOnlyRWformatters: false
    property string fieldLabel: qsTranslate("RDM","Value") + ":"
    property bool isEdited: false
    property var value    
    property int valueCompression: 0
    property string formatterSettingsCategory: "formatters_value"
    property alias readOnly: textView.readOnly
    property string _jsFormatterStyles: {
        return "font-size: "
                + appSettings.valueEditorFontSize
                + "pt; font-family: "
                + appSettings.valueEditorFont
    }
    property var _jsFormatterColorMap: {
        if (sysPalette.base.hslLightness < 0.4) {
            return {
                string: '#05a605',
                number: '#008cff',
                boolean: '#d62929',
                null: '#a8a8a8',
                key: '#fcfcfc'
            };
        } else {
            return {
                string: '#008000',
                number: '#0000ff',
                boolean: '#b22222',
                null: '#808080',
                key: '#000000'
            };
        }
    }

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
        function process(formattedValue) {
            var formatter = valueFormattersModel.get(formatterSelector.currentIndex)

             formatter.getRaw(formattedValue, function (error, raw) {
                 if (formatter.type === "external") {
                    root.value = compress(qmlUtils.b64toByteArray(raw))
                 } else {
                    root.value = compress(raw)
                 }
                 return callback(error, root.value)
             })
        }

        if (textView.format === "json") {
            formatterSelector.model.getJSONFormatter().getRaw(textView.model.getText(), function (jsonError, plainText) {
                if (jsonError) {
                    return callback(jsonError, "")
                }

                process(plainText)
            })
        } else {
            process(textView.model.getText())
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

        var isBin = qmlUtils.isBinaryString(root.value)
        binaryFlag.visible = isBin

        if (qmlUtils.binaryStringLength(root.value) > appSettings.valueSizeLimit) {
            root.showFormatters = false
            formatterSelector.currentIndex = formatterSelector.model.getDefaultFormatter(isBin)
            guessFormatter = false
        } else {
            root.showFormatters = true
        }

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

        if (isBin && qmlUtils.binaryStringLength(root.value) > appSettings.valueSizeLimit) {
            largeValueDialog.visible = true
        } else {
            largeValueDialog.visible = false
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
        if (!(0 <= formatterSelector.currentIndex
              && formatterSelector.currentIndex < formatterSelector.count)) {
            formatterSelector.currentIndex = formatterSelector.model.getDefaultFormatter(isBin)
        }

        var formatter = formatterSelector.model.get(formatterSelector.currentIndex)

        uiBlocker.visible = true

        if (formatter["name"] === "JSON") {
            jsonFormattingWorker.sendMessage({"isReadOnly": false,
                                              "data": String(root.value),
                                              "style": _jsFormatterStyles,
                                              "color_map": _jsFormatterColorMap})
        } else {
            formatter.getFormatted(root.value, function (error, formatted, isReadOnly, format) {

                function process(error, formatted, stub, format) {
                    jsonFormattingWorker.processFormatted(error, formatted, stub, format)
                }

                textView.format = format

                if (format === "json") {
                    jsonFormattingWorker.sendMessage({"error": error,
                                                      "isReadOnly": isReadOnly,
                                                      "data": String(formatted),
                                                      "style": _jsFormatterStyles,
                                                      "color_map": _jsFormatterColorMap})
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
            processFormatted(messageObject.error, messageObject.formatted, messageObject.isReadOnly, messageObject.format);
        }

        function processFormatted(error, formatted, isReadOnly, format) {
            textView.textFormat = (format === "html")
                ? TextEdit.RichText
                : TextEdit.PlainText;

            if (error || !formatted) {
                if (formatted) {
                    defaultFormatterSettings.defaultFormatterIndex = formatterSelector.currentIndex
                    textView.model = qmlUtils.wrapLargeText(formatted)
                } else {
                    var isBin = false
                    formatterSelector.currentIndex = valueFormattersModel.guessFormatter(isBin) // Reset formatter to plain text
                }
                textView.readOnly = isReadOnly
                root.isEdited = false
                uiBlocker.visible = false

                var details
                if (error.length > 200) {
                    details = error
                    error = qsTranslate("RDM","Formatting error")
                } else {
                    details = ""
                }

                notification.showError(error || qsTranslate("RDM","Unknown formatter error (Empty response)"), details)
                return
            }

            defaultFormatterSettings.defaultFormatterIndex = formatterSelector.currentIndex
            textView.model = qmlUtils.wrapLargeText(formatted)
            textView.readOnly = isReadOnly
            root.isEdited = false
            uiBlocker.visible = false
        }
    }

    ColumnLayout {
        anchors.fill: parent

        RowLayout {
            Layout.fillWidth: true
            spacing: 5

            BetterLabel { text: root.fieldLabel }
            TextEdit {
                Layout.preferredWidth: 150
                text: qsTranslate("RDM", "Size: ") + qmlUtils.humanSize(qmlUtils.binaryStringLength(value));
                readOnly: true;
                selectByMouse: true
                color: "#ccc"
            }
            BetterLabel { id: binaryFlag; text: qsTranslate("RDM","[Binary]"); visible: false; color: "green"; }
            BetterLabel {
                objectName: "rdm_value_editor_compressed_value_label"
                text: qsTranslate("RDM"," [Compressed: ") + qmlUtils.compressionAlgName(root.valueCompression) + "]";
                visible: root.valueCompression > 0;
                color: "red";
            }
            Item { Layout.fillWidth: true }

            BetterLabel { visible: showFormatters; text: qsTranslate("RDM","View as:") }

            Settings {
                id: defaultFormatterSettings
                category: formatterSettingsCategory
                property int defaultFormatterIndex
            }

            BetterComboBox {
                id: formatterSelector
                visible: showFormatters && !showOnlyRWformatters
                width: 200
                model: valueFormattersModel
                textRole: "name"
                objectName: "rdm_value_editor_formatter_combobox"

                onActivated: {
                    currentIndex = index
                    loadFormattedValue()
                }
            }

            BetterComboBox {
                id: rwFormatterSelector
                visible: showFormatters && showOnlyRWformatters
                width: 200
                model: valueFormattersModel.rwFormatters
                textRole: "name"
                objectName: "rdm_value_editor_rw_formatter_combobox"

                onActivated: {
                    formatterSelector.currentIndex = valueFormattersModel.getFormatterIndex(currentText);
                }
            }

            BetterLabel {
                visible: !showFormatters && qmlUtils.binaryStringLength(root.value) > appSettings.valueSizeLimit
                text: qsTranslate("RDM","Large value (>150kB). Formatters are not available.")
                color: "red"
            }

            RowLayout {
                id: valueEditorToolBar
                Layout.preferredWidth: isMultiRow ? 200 : 208
                Layout.maximumWidth: isMultiRow ? 200 : 208

                visible: showToolBar                

                RowLayout {
                    Layout.fillWidth: true
                    Layout.preferredWidth: 98

                    ImageButton {
                        iconSource: "qrc:/images/add.svg"
                        implicitWidth: imgBtnWidth
                        implicitHeight: imgBtnHeight
                        imgWidth: imgBtnWidth
                        imgHeight: imgBtnHeight

                        Layout.alignment: Qt.AlignHCenter

                        tooltip: qsTranslate("RDM","Add Element to HLL");
                        visible: keyType === "hyperloglog"

                        onClicked: {
                            addRowDialog.open()
                        }
                    }

                    ImageButton {
                        id: copyValueToClipboardBtn
                        iconSource: "qrc:/images/copy.svg"
                        implicitWidth: imgBtnWidth
                        implicitHeight: imgBtnHeight
                        imgWidth: imgBtnWidth
                        imgHeight: imgBtnHeight

                        Layout.alignment: Qt.AlignHCenter

                        tooltip: qsTranslate("RDM","Copy to Clipboard")
                        enabled: root.value !== ""

                        onClicked: copyValue()

                        function copyValue() {                            
                            if (value) {
                                qmlUtils.copyToClipboard(value)
                            }
                        }
                    }

                    SaveToFileButton {
                        id: saveAsBtn
                        objectName: "rdm_save_value_to_file_btn"

                        Layout.alignment: Qt.AlignHCenter

                        implicitWidth: imgBtnWidth
                        implicitHeight: imgBtnHeight
                        imgWidth: imgBtnWidth
                        imgHeight: imgBtnHeight

                        enabled: root.value !== ""
                    }
                }

                BetterButton {
                    id: saveBtn
                    objectName: "rdm_value_editor_save_btn"

                    state: "default"
                    implicitWidth: isMultiRow ? 100 : 105

                    text: qsTranslate("RDM","Save")
                    tooltip: qsTranslate("RDM","Save Changes") + " (" + shortcutText + ")"
                    visible: showSaveBtn

                    property string shortcutText: ""

                    onClicked: saveChanges()

                    function saveChanges() {
                        if (!valueEditor.item || !valueEditor.item.isEdited()) {
                            return
                        }

                        valueEditor.item.validateValue(function (result) {
                            if (!result)
                                return;

                            var value = valueEditor.item.getValue()

                            saveBtnTimer.start()
                            keyTab.keyModel.updateRow(valueEditor.currentRow, value)
                        })
                    }

                    states: [
                        State {
                            name: "default"

                            PropertyChanges {
                                target: saveBtn
                                iconSource: "qrc:/images/save.svg"
                                enabled: !showOnlyRWformatters && root.value !== "" && valueEditor.item.isEdited() && keyType != "stream"
                            }
                        },
                        State {
                            name: "saving"

                            PropertyChanges {
                                target: saveBtn
                                iconSource: "qrc:/images/wait.svg"
                                enabled: false
                            }
                        }
                    ]

                    Connections {
                        target: keyTab.keyModel ? keyTab.keyModel : null

                        function onValueUpdated() {
                            root.isEdited = false
                            saveBtnTimer.resetSaveBtn()
                        }

                        function onError() { saveBtnTimer.resetSaveBtn() }
                    }

                    Timer {
                        id: saveBtnTimer
                        interval: 500
                        repeat: true
                        triggeredOnStart: true
                        onTriggered: saveBtn.state = "saving"

                        function resetSaveBtn() {
                            saveBtnTimer.stop()
                            saveBtn.state = "default"
                        }
                    }
                }

                Item {
                    width: 100
                    visible: !showSaveBtn && keyType == "hash"
                }
            }

        }

        Rectangle {
            id: texteditorWrapper
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.preferredHeight: 100

            color: sysPalette.base
            border.color: sysPalette.mid
            border.width: 1
            clip: true

            ScrollView {
                anchors.fill: parent
                anchors.margins: 5

                ScrollBar.vertical.policy: ScrollBar.AlwaysOn
                ScrollBar.vertical.minimumSize: 0.05

                enabled: !(qmlUtils.isBinaryString(root.value) && qmlUtils.binaryStringLength(root.value) > appSettings.valueSizeLimit)

                ListView {
                    id: textView
                    anchors.fill: parent
                    cacheBuffer: 4

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
                                text: qmlUtils.isBinaryString(root.value) && qmlUtils.binaryStringLength(root.value) > appSettings.valueSizeLimit ?
                                          qmlUtils.printable(value, false, 50000) : value;  // Show first 50KB to fit chunkSize

                                textFormat: textView.textFormat
                                readOnly: textView.readOnly

                                onTextChanged: {
                                    root.isEdited = true
                                    textView.model && textView.model.setTextChunk(index, textAreaPart.text)
                                }

                                Keys.forwardTo: [saveShortcut, saveAsShortcut]

                            }

                            Shortcut {
                                id: saveShortcut
                                sequence: StandardKey.Save
                                onActivated: saveBtn.saveChanges()

                                Component.onCompleted: {
                                    saveBtn.shortcutText = saveShortcut.nativeText
                                }
                            }

                            Shortcut {
                                id: saveAsShortcut
                                sequence: StandardKey.SaveAs
                                onActivated: saveAsBtn.saveToFile()

                                Component.onCompleted: {
                                    saveAsBtn.shortcutText = saveAsShortcut.nativeText
                                }
                            }
                        }
                    }
                }
        }

        BetterLabel {
            id: validationError
            color: "red"
            visible: false
        }

    }

    BetterDialog {
        id: largeValueDialog
        title: qsTranslate("RDM","Binary value is too large to display")
        visible: false
        footer: BetterDialogButtonBox {
            BetterButton {
                text: qsTranslate("RDM","OK")
                onClicked: largeValueDialog.close()
            }
        }

        RowLayout {
            Text {
                color: sysPalette.text
                text: qsTranslate("RDM","Save value to file: ")
            }

            SaveToFileButton {
                objectName: "rdm_save_large_value_to_file_dialog_btn"
            }
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
