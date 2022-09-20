import QtQuick 2.5
import QtQuick.Controls 2.13
import QtQuick.Layouts 1.1
import QtQuick.Window 2.2
import Qt.labs.settings 1.0
import rdm.models 1.0
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
    property bool showValueSize: true
    property string fieldLabel: qsTranslate("RESP","Value") + ":"
    property bool isEdited: false
    property var value    
    property int valueCompression: 0
    property alias readOnly: textView.readOnly
    property string formatterSettingsPrefix: ""
    property string lastSelectedFormatterSetting: "last_selected_" + root.formatterSettingsPrefix + "formatter"
    property string lastSelectedManualDecompression: "last_selected_" + root.formatterSettingsPrefix + "decompression"
    property string defaultFormatter: "auto"

    property var __formatterCombobox: formatterSelector
    property var __textView: textView

    function __getFormattingContext()
    {
        return {
            "redis-key-name": root.parent.state === "new"? newKeyName.value : keyName,
            "redis-key-type": keyType,
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
                return callback(false, raw);
            }

            var valid = validationRule(raw)            

            if (valid) {
                hideValidationError()
            } else {
                showValidationError(qsTranslate("RESP", "Enter valid value"))
            }

            return callback(valid, raw)
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
                 var compressed = compress(raw);
                 return callback(error, compressed)
             }, __getFormattingContext())
        }

        if (textView.format === "json") {
            formatterSelector.model.getJSONFormatter().getRaw(textView.model.getText(), function (jsonError, plainText) {
                if (jsonError) {
                    return callback(jsonError, "")
                }

                process(plainText)
            }, __getFormattingContext())
        } else {
            process(textView.model.getText())
        }
    }

    function loadFormattedValue(val) {

        var guessFormatter = false;

        if (val) {
            root.value = val;
            guessFormatter = true;
        }

        var isBin = qmlUtils.isBinaryString(root.value)
        binaryFlag.visible = isBin

        if (isBin && qmlUtils.binaryStringLength(root.value) > appSettings.valueSizeLimit) {
            largeValueDialog.visible = true;
            root.showFormatters = false;
            textView.model = qmlUtils.wrapLargeText(qmlUtils.printable(root.value, false, 50000));
            textView.readOnly = true;
            saveBtn.enabled = false;
            return;
        } else {
            largeValueDialog.visible = false
            root.showFormatters = true
        }

        var continueFormatting = function (guessFormatter) {
            if (!root.value) {
                console.log("Empty value. Skipping formatting stage");
                return;
            }

            // NOTE(u_glide): -1 means "not set", 0 - unknown or not compressed
            if (valueCompression < 0) {
                var compressionMethod = qmlUtils.isCompressed(root.value);

                if (compressionMethod > 0) {
                    valueCompression = compressionMethod
                    root.value = qmlUtils.decompress(root.value, valueCompression)
                    isBin = qmlUtils.isBinaryString(root.value)

                    var compression = qmlUtils.compressionAlgName(valueCompression);

                    // NOTE(u_glide): hint PHP formatter if MAGENTO/PHP compression detected
                    if (guessFormatter && compression
                            && compression.startsWith("magento-session-")) {
                        formatterSelector._select("php");
                        guessFormatter = false;
                    }
                }

                // NOTE(u_glide): try to decompress using last "no magic" compression
                if (isBin && valueCompression < 0
                        && qmlUtils.binaryStringLength(root.value) <= appSettings.valueSizeLimit) {
                    noMagicCompressionSelector.loadLastUsed()
                }
            }

            // If current formatter is plain text - try to guess formatter
            if (guessFormatter) {
                _guessFormatter(root.value, isBin, function() {
                    _loadFormatter(isBin)
                })
            } else {
                _loadFormatter(isBin)
            }
        };

        if (guessFormatter) {
            console.log("Default formatter:", root.defaultFormatter)

            var formatterOverride = defaultFormatterSettings.value(
                root.formatterSettingsPrefix + keyName, ""
            );

            if (!formatterOverride) {
                if (root.defaultFormatter == "last_used") {
                    formatterOverride = defaultFormatterSettings.value(root.lastSelectedFormatterSetting, "");
                } else if (root.defaultFormatter != "auto") {
                    formatterOverride = root.defaultFormatter;
                }

                if (!formatterOverride || root.defaultFormatter == "auto") {
                    return continueFormatting(true)
                }
            }

            var expectedFormatter = formatterSelector.find(formatterOverride);

            if (expectedFormatter === -1) {
                console.log("Formatter", formatterOverride, " is not loaded. Fallback to guessing...")
                return continueFormatting(true)
            }

            console.log("Formatter override:", formatterOverride)

            var cFormatter = formatterSelector.model.get(expectedFormatter)

            return cFormatter.isValid(root.value, function (isValid) {
                var compressionMethod = qmlUtils.isCompressed(root.value);

                if (isValid || compressionMethod > 0) {
                    formatterSelector._select(formatterOverride)
                    continueFormatting(false)
                } else {
                    console.log("Formatter", formatterOverride, " cannot decode value. Fallback to guessing...")
                    continueFormatting(true)
                }
            }, __getFormattingContext())
        } else {
            continueFormatting(false)
        }
    }

    function hintFormatter(name) {
        if (showOnlyRWformatters) {
            rwFormatterSelector._select(name)
            formatterSelector._select(name)
        } else {
            formatterSelector._select(name)
        }
        _loadFormatter(false)
    }

    function _guessFormatter(value, isBin, callback) {
        console.log("Guessing formatter")

        var candidates = valueFormattersModel.guessFormatter(value, isBin)

        console.log("candidates:", candidates)

        if (Array.isArray(candidates)) {

            for (var index in candidates) {
                var cFormatter = formatterSelector.model[candidates[index]]

                cFormatter.isValid(root.value, function (isValid) {
                    if (isValid) {
                        formatterSelector.currentIndex = candidates[index]
                        callback()
                    }
                }, __getFormattingContext())

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

        function processFormattingResult(error, formatted, isReadOnly, format) {
            textView.textFormat = (format === "html")
                ? TextEdit.RichText
                : TextEdit.PlainText;

            console.log("format", format)

            if (error || (!formatted && root.value)) {
                if (formatted) {
                    textView.model = qmlUtils.wrapLargeText(formatted)
                } else if (!error) {
                    formatterSelector.currentIndex = valueFormattersModel.guessFormatter(root.value, isBin)
                    return _loadFormatter(isBin)
                }
                textView.readOnly = isReadOnly
                textView.format = "text"
                root.isEdited = false
                uiBlocker.visible = false

                var details
                if (error.length > 200) {
                    details = error
                    error = qsTranslate("RESP","Formatting error")
                } else {
                    details = ""
                }

                notification.showError(error || qsTranslate("RESP","Unknown formatter error (Empty response)"), details)
                return
            }

            if (format === "image") {
                imageView.source = formatted;
            } else {
                textView.model = qmlUtils.wrapLargeText(formatted)
            }
            textView.readOnly = isReadOnly
            textView.format = format
            root.isEdited = false
            uiBlocker.visible = false
        }

        formatter.getFormatted(root.value, function (error, formatted, isReadOnly, format) {
            textView.format = format

            if (format === "json" && formatter["name"] !== "JSON" && !error) {
                formatterSelector.model.getJSONFormatter().getFormatted(formatted, function (jsonError, plainText) {
                    if (jsonError) {
                        processFormattingResult(jsonError, formatted, isReadOnly, format)
                    } else {
                        processFormattingResult(jsonError, plainText, isReadOnly, format)
                    }
                }, __getFormattingContext())
            } else {
                processFormattingResult(error, formatted, isReadOnly, format)
            }
        }, __getFormattingContext())
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
        root.valueCompression = -1
        binaryFlag.visible = false
        saveBtnTimer.resetSaveBtn()
        hideValidationError()
    }

    function showValidationError(msg) {
        validationError.text = msg
        validationError.visible = true
    }

    function hideValidationError() {
        validationError.visible = false
    }

    ColumnLayout {
        anchors.fill: parent

        RowLayout {
            Layout.fillWidth: true
            spacing: 5

            BetterLabel { text: root.fieldLabel }
            TextEdit {                
                text: qsTranslate("RESP", "Size: ") + qmlUtils.humanSize(qmlUtils.binaryStringLength(value));
                readOnly: true;
                selectByMouse: true
                color: "#ccc"
                visible: showValueSize
            }
            BetterLabel { id: binaryFlag; text: qsTranslate("RESP","[Binary]"); visible: false; color: "green"; }
            Item { Layout.fillWidth: true }

            BetterLabel { visible: showFormatters; text: qsTranslate("RESP","View as:") }

            BetterComboBox {
                id: formatterSelector
                visible: showFormatters && !showOnlyRWformatters
                width: 200
                model: valueFormattersModel
                textRole: "name"
                objectName: "rdm_value_editor_formatter_combobox"

                onActivated: {
                    currentIndex = index                    
                    console.log("Set default formatter '" + currentText + "' for key " + keyName)
                    defaultFormatterSettings.setValue(root.formatterSettingsPrefix + keyName, currentText)
                    defaultFormatterSettings.setValue(root.lastSelectedFormatterSetting, currentText)
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
                visible: noMagicCompressionSelector.visible
                text: noMagicCompressionSelector.enabled? qsTranslate("RESP","Try to decompress:") :
                                                          qsTranslate("RESP","Decompressed:")
            }

            BetterComboBox {
                id: noMagicCompressionSelector

                Layout.preferredWidth: 120
                Layout.fillWidth: true

                objectName: "rdm_value_editor_compression_combobox"
                textRole: "text"

                visible: {
                    console.log("keyType:", keyType)
                    return binaryFlag.visible && keyType != "hyperloglog" && qmlUtils.binaryStringLength(root.value) <= appSettings.valueSizeLimit
                            || root.valueCompression > 0
                }

                onEnabledChanged: {
                    indicator.visible = enabled;
                }

                flat: !enabled

                enabled: {
                    return binaryFlag.visible && root.valueCompression < 1
                            || root.valueCompression >= firstNoMagicMethod;
                }

                displayText: {
                    if (0 < root.valueCompression && root.valueCompression < noMagicCompressionSelector.firstNoMagicMethod) {
                        return qmlUtils.compressionAlgName(root.valueCompression)
                    } else {
                        return currentText;
                    }
                }

                property int firstNoMagicMethod: {
                    var noMagicCompress = qmlUtils.compressionMethodsNoMagic();
                    return noMagicCompress[noMagicCompress.length - 1];
                }

                model: {
                   var noMagicCompress = qmlUtils.compressionMethodsNoMagic();

                   var modelList = [];
                    for (var index in noMagicCompress) {
                        var label = qmlUtils.compressionAlgName(noMagicCompress[index]);

                        if (label === "unknown") {
                            label = "";
                        }

                        modelList.push({"value": noMagicCompress[index], "text": label});
                    }

                   return modelList;
                }

                function loadLastUsed() {
                    var lastSelected = defaultCompressionSettings.value(
                                root.formatterSettingsPrefix + keyName,
                                defaultCompressionSettings.value(root.lastSelectedManualDecompression, "")
                    );                    

                    selectItem(lastSelected);
                }

                onActivated: {
                    console.log("Try to decompress as", currentText)

                    var expectedCompression = model[currentIndex]['value'];

                    if (expectedCompression == 0) {
                        valueCompression = 0
                        defaultCompressionSettings.setValue(root.formatterSettingsPrefix + keyName, "")
                        defaultCompressionSettings.setValue(root.lastSelectedManualDecompression, "")                        
                        root.loadFormattedValue()
                        return
                    }

                    var decompressed = qmlUtils.decompress(root.value, expectedCompression)

                    if (qmlUtils.binaryStringLength(decompressed) > 0) {
                        binaryFlag.visible = qmlUtils.isBinaryString(root.value)
                        valueCompression = expectedCompression;
                        root.loadFormattedValue(decompressed)
                        defaultCompressionSettings.setValue(root.formatterSettingsPrefix + keyName, currentText)
                        defaultCompressionSettings.setValue(root.lastSelectedManualDecompression, currentText)
                        noMagicCompressionSelector.enabled = false;
                    } else {
                        notification.showError(qsTranslate("RESP","Cannot decompress value using ") + currentText)
                        defaultCompressionSettings.setValue(root.formatterSettingsPrefix + keyName, "")
                        defaultCompressionSettings.setValue(root.lastSelectedManualDecompression, "")
                        valueCompression = 0
                        currentIndex = 0;
                    }
                }
            }

            BetterLabel {
                visible: !showFormatters && qmlUtils.binaryStringLength(root.value) > appSettings.valueSizeLimit
                text: qsTranslate("RESP","Large value (>150kB). Formatters are not available.")
                color: "red"
            }

            BetterButton {
                iconSource: PlatformUtils.getThemeIcon("add.svg")
                Layout.alignment: Qt.AlignHCenter

                text: qsTranslate("RESP","Add Element");
                visible: (keyType === "hyperloglog"
                          || keyType === "bf"
                          || keyType === "cf")

                onClicked: {
                    keyTab.addRowDialog.open()
                }
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
                        id: copyValueToClipboardBtn
                        iconSource: PlatformUtils.getThemeIcon("copy_2.svg")
                        implicitWidth: imgBtnWidth
                        implicitHeight: imgBtnHeight
                        imgWidth: imgBtnWidth
                        imgHeight: imgBtnHeight

                        Layout.alignment: Qt.AlignHCenter

                        tooltip: qsTranslate("RESP","Copy to Clipboard")
                        enabled: root.value !== ""

                        onClicked: copyValue()

                        function copyValue() {
                            if (value) {
                                qmlUtils.copyToClipboard(textView.model.getText())
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

                        enabled: root.value !== "" && root.showFormatters

                        shortcutText: qmlUtils.standardKeyToString(StandardKey.SaveAs)
                    }

                    SaveToFileButton {
                        id: saveAsRawBtn
                        objectName: "rdm_save_raw_value_to_file_btn"

                        raw: true

                        Layout.alignment: Qt.AlignHCenter

                        implicitWidth: imgBtnWidth
                        implicitHeight: imgBtnHeight
                        imgWidth: imgBtnWidth
                        imgHeight: imgBtnHeight

                        enabled: root.value !== ""
                    }
                }

                ImageButton {
                    id: fullScreenModeBtn

                    iconSource: editor.state === "default"?
                                    PlatformUtils.getThemeIcon("maximize.svg")
                                  : PlatformUtils.getThemeIcon("minimize.svg")
                    implicitWidth: imgBtnWidth
                    implicitHeight: imgBtnHeight
                    imgWidth: imgBtnWidth * 0.8
                    imgHeight: imgBtnHeight * 0.8

                    tooltip: (editor.state === "default"? "" : qsTranslate("RESP","Exit "))
                                                         + qsTranslate("RESP","Full Screen Mode")

                    onClicked: {
                        editor.state = editor.state === "default"? "full_screen" : "default"
                        editor.forceActiveFocus()
                    }
                }

                BetterButton {
                    id: saveBtn
                    objectName: "rdm_value_editor_save_btn"

                    state: "default"
                    implicitWidth: isMultiRow ? 100 : 105

                    text: qsTranslate("RESP","Save")
                    tooltip: qsTranslate("RESP","Save Changes") + " (" + shortcutText + ")"
                    visible: showSaveBtn

                    property string shortcutText: qmlUtils.standardKeyToString(StandardKey.Save)

                    onClicked: saveChanges()

                    function saveChanges() {
                        if (!valueEditor.item || !valueEditor.item.isEdited()) {
                            return
                        }

                        valueEditor.item.getValue(true, function (valid, row) {
                            if (!valid)
                                return;

                            saveBtnTimer.start()                            
                            keyTab.keyModel.updateRow(valueEditor.currentRow, row)
                        })
                    }

                    states: [
                        State {
                            name: "default"

                            PropertyChanges {
                                target: saveBtn
                                iconSource: PlatformUtils.getThemeIcon("save.svg")
                                enabled: !showOnlyRWformatters && root.value !== "" && valueEditor.item.isEdited() && keyType != "stream"
                            }
                        },
                        State {
                            name: "saving"

                            PropertyChanges {
                                target: saveBtn
                                iconSource: PlatformUtils.getThemeIcon("wait.svg")
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
            id: searchToolbar

            property int lastSearchResultPosition: -1

            color: sysPalette.base
            border.color: sysPalette.mid
            border.width: 1

            Layout.fillWidth: true
            Layout.preferredHeight: 50
            visible: false

            RowLayout {
                anchors.fill: parent
                anchors.rightMargin: 10
                anchors.leftMargin: 10

                Image {
                    source: PlatformUtils.getThemeIcon("search.svg")
                    Layout.preferredWidth: 20
                    Layout.preferredHeight: 20
                }

                BetterTextField {
                    id: searchField
                    objectName: "rdm_value_editor_search_field"
                    placeholderText: qsTranslate("RESP", "Search string")

                    onTextChanged: {
                        searchToolbar.lastSearchResultPosition = -1;
                        noResults.visible = false;
                    }

                    onAccepted: submitSearchButton.performSearch()

                    Layout.preferredWidth: 300
                }
                BetterButton {
                    id: submitSearchButton
                    objectName: "rdm_value_editor_search_btn"
                    text: searchToolbar.lastSearchResultPosition>=0 ? qsTranslate("RESP","Find Next") : qsTranslate("RESP","Find")
                    onClicked: {
                        performSearch()
                    }

                    function performSearch() {
                        noResults.visible = false;

                        var result = textView.model.searchText(searchField.text,
                                                               searchToolbar.lastSearchResultPosition,
                                                               searchRegexInText.checked)                        

                        if (result[0] >= 0) {
                            textView.currentIndex = result[0];
                            searchToolbar.lastSearchResultPosition = result[1] + result[3];
                            textView.currentItem.selectSearchResult(result[2], result[3], searchField.text);
                        } else {
                            noResults.text = searchToolbar.lastSearchResultPosition>=0 ? qsTranslate("RESP","Cannot find more results")
                                                                                       : qsTranslate("RESP","Cannot find any results");
                            if (searchToolbar.lastSearchResultPosition>=0) {
                                searchToolbar.lastSearchResultPosition = -1;
                            }

                            noResults.visible = true;
                        }
                    }
                }

                BetterCheckbox {
                    id: searchRegexInText
                    objectName: "rdm_value_editor_search_regex_checkbox"
                    text: qsTranslate("RESP","Regex")
                    onCheckedChanged: {
                        searchToolbar.lastSearchResultPosition = -1;
                        noResults.visible = false;
                    }
                }

                BetterLabel {
                    id: noResults
                    objectName: "rdm_value_editor_search_no_results"
                    visible: false
                }

                Item {
                    Layout.fillWidth: true
                }

                ImageButton {
                    objectName: "rdm_value_editor_search_clear_btn"
                    Layout.preferredWidth: 20
                    Layout.preferredHeight: 20

                    imgSource: PlatformUtils.getThemeIcon("clear.svg")
                    onClicked: {
                        searchToolbar.visible = false;
                        searchToolbar.lastSearchResultPosition = -1;
                        noResults.visible = false;
                        // TODO: clear results & selections
                    }

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
                id: valueScrollView
                anchors.fill: parent
                anchors.margins: 5
                visible: textView.format !== "image"

                ScrollBar.vertical.policy: ScrollBar.AlwaysOn
                ScrollBar.vertical.minimumSize: 0.05

                enabled: !(qmlUtils.isBinaryString(root.value) && qmlUtils.binaryStringLength(root.value) > appSettings.valueSizeLimit)

                ListView {
                    id: textView
                    anchors.fill: parent
                    cacheBuffer: 4
                    highlightMoveDuration: 0                    

                    Keys.onPressed: {
                       if (event.matches(StandardKey.Find)) {
                           searchField.forceActiveFocus()
                           searchToolbar.visible = true;
                       } else if (event.matches(StandardKey.SaveAs)) {
                           saveAsBtn.saveToFile()
                       } else if (event.matches(StandardKey.Save)) {
                           saveBtn.saveChanges()
                       }
                    }

                    property int textFormat: TextEdit.PlainText
                    property bool readOnly: false
                    property string format

                    delegate:
                        Rectangle {
                            color: "transparent"

                            width: texteditorWrapper.width
                            height: textAreaPart.contentHeight < texteditorWrapper.height? texteditorWrapper.height - 5 : textAreaPart.contentHeight

                            function selectSearchResult(from, len, txt) {
                                textAreaPart.select(from, from + len)
                                textView.contentY = textView.currentItem.y + textAreaPart.cursorRectangle.y
                            }

                            NewTextArea {
                                anchors.fill: parent
                                id: textAreaPart
                                objectName: "rdm_key_multiline_text_field_" + index

                                enabled: root.enabled
                                text: qmlUtils.isBinaryString(root.value) && qmlUtils.binaryStringLength(root.value) > appSettings.valueSizeLimit ?
                                          qmlUtils.printable(value, false, 50000) : value;  // Show first 50KB to fit chunkSize

                                textFormat: textView.textFormat
                                readOnly: textView.readOnly
                                highlightJSON: textView.format === "json"

                                onTextChanged: {
                                    root.isEdited = true
                                    textView.model && textView.model.setTextChunk(index, textAreaPart.text)
                                }

                                Keys.forwardTo: [textView]
                            }
                        }
                }  
            }

            Image {
                id: imageView
                anchors.fill: parent
                fillMode: Image.PreserveAspectFit
                visible: textView.format === "image"
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

        height: 150

        title: qsTranslate("RESP","Binary value is too large to display")
        visible: false
        footer: BetterDialogButtonBox {
            BetterButton {
                text: qsTranslate("RESP","OK")
                onClicked: largeValueDialog.close()
            }
        }

        RowLayout {
            anchors.fill: parent

            Text {                
                color: sysPalette.text
                text: qsTranslate("RESP","Save value to file")+ ": "
            }

            SaveToFileButton {
                objectName: "rdm_save_large_raw_value_to_file_dialog_btn"
                raw: true
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
}
