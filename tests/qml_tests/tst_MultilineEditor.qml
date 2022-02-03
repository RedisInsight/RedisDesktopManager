import QtQuick 2.3
import QtQml.Models 2.13
import QtTest 1.0
import Qt.labs.settings 1.0
import rdm.models 1.0

import "./../../src/qml/value-editor/editors/formatters/"
import "./../../src/qml/value-editor/editors/"

TestCase {
    name: "FormatterTests"

    property string validJson: '{"test": 123}'

    SystemPalette {
        id: sysPalette
    }

    Settings {
        id: defaultFormatterSettings
        category: "formatter_overrides"

        function cleanup() {
            testUtils.removeAppSetting("formatter_overrides")
            defaultFormatterSettings.sync()
        }
    }

    Item {
        id: appSettings

        property string valueEditorFont: "sans-serif"
    }

    ValueFormatters {
        id: valueFormattersModel
    }

    QtObject {
        id: valueEditor

        property var item: QtObject {
            function isEdited() {
                return false;
            }
        }
    }

    MultilineEditor {
        id: editor

        // ValueEditor fake properties
        property bool isMultiRow: true
        property string keyType: "list"
        property string keyName: "fake_list"
    }

    function init() {
        defaultFormatterSettings.cleanup()
        editor.defaultFormatter = "auto"
    }

    function test_loadFormattedValue() {       
        editor.loadFormattedValue(validJson)

        verify(editor.__formatterCombobox.currentText === "JSON")
        verify(editor.__textView.format === "json")
        verify(!editor.__textView.readOnly)
        verify(editor.__textView.textFormat === TextEdit.PlainText)
    }

    function test_loadFormattedValue_withDefaultFormatter() {
        editor.defaultFormatter = "HEX TABLE"
        defaultFormatterSettings.setValue(editor.lastSelectedFormatterSetting, "Plain Text")
        defaultFormatterSettings.sync()

        editor.loadFormattedValue(validJson)

        verify(editor.__formatterCombobox.currentText === "HEX TABLE")
        verify(editor.__textView.format === "html")
        verify(editor.__textView.readOnly)
        verify(editor.__textView.textFormat === TextEdit.RichText)
    }

    function test_loadFormattedValue_withLastSelectedFormatter() {
        editor.defaultFormatter = "last_used"
        defaultFormatterSettings.setValue(editor.lastSelectedFormatterSetting, "Plain Text")
        defaultFormatterSettings.sync()

        editor.loadFormattedValue(validJson)

        verify(editor.__formatterCombobox.currentText === "Plain Text")
        verify(editor.__textView.format === "plain")
        verify(!editor.__textView.readOnly)
        verify(editor.__textView.textFormat === TextEdit.PlainText)
    }

    function test_loadFormattedValue_withLastSelectedFormatter_and_key_override() {
        defaultFormatterSettings.setValue(editor.keyName, "HEX")
        defaultFormatterSettings.setValue(editor.lastSelectedFormatterSetting, "Plain Text")
        defaultFormatterSettings.sync()

        editor.loadFormattedValue(validJson)

        verify(editor.__formatterCombobox.currentText === "HEX")
        verify(editor.__textView.format === "plain")
        verify(!editor.__textView.readOnly)
        verify(editor.__textView.textFormat === TextEdit.PlainText)
    }

}
