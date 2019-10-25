import QtQuick 2.3
import QtTest 1.0

import "./../../src/qml/value-editor/editors/formatters/formatters.js" as Formatters

TestCase {
    name: "FormatterTests"

    function test_plain() {
        // given
        var plain = Formatters.plain
        var testValue = "plain_text!"

        // checks
        verify(plain.title.length !== 0, "title")

        plain.getFormatted(testValue, function (error, formatted, readOnly, format){
            compare(formatted, testValue)
        })

        plain.getRaw(testValue, function (error, plain){
            compare(plain, testValue)
        })
    }
}
