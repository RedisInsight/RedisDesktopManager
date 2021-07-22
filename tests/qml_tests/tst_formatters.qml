import QtQuick 2.3
import QtQml.Models 2.13
import QtTest 1.0

import "./../../src/qml/value-editor/editors/formatters/"

TestCase {
    name: "FormatterTests"

    ValueFormatters {
        id: valueFormatters
    }

    function test_plain() {
        // given
        var plain = valueFormatters.get(0)
        var testValue = "plain_text!"

        // checks
        verify(plain.name.length !== 0, "title")

        plain.getFormatted(testValue, function (error, formatted, readOnly, format){
            compare(formatted, testValue)
        })

        plain.getRaw(testValue, function (error, plain){
            compare(plain, testValue)
        })
    }
}
