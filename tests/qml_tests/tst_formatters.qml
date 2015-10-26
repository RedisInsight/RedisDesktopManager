import QtQuick 2.3
import QtTest 1.0

import "./../../src/resources/qml/editors/formatters/formatters.js" as Formatters

TestCase {
    name: "FormatterTests"

    function checkProperties(f, validBin, validRead) {
        verify(f.title.length != 0, "title")
        compare(f.binary, validBin)
        compare(f.readOnly, validRead)
    }

    function test_plain() {
        // given
        var plain = Formatters.plain
        var testValue = "plain_text!"

        // when
        checkProperties(plain, false, false)

        //then
        compare(plain.isValid(testValue), true)
        compare(plain.getFormatted(testValue), testValue)
        compare(plain.getRaw(testValue), testValue)
    }

    function test_php() {
        // given
        var phpFormatter = Formatters.phpserialized
        var testValue = 'a:6:{i:1;s:30:"PHP code tester Sandbox Online";'
                +'s:3:"foo";s:3:"bar";i:2;i:5;i:5;i:89009;s:4:"case";'
                +'s:12:"Random Stuff";s:11:"PHP Version";s:6:"5.5.18";}'
        var validResult = {
            "1": "PHP code tester Sandbox Online",
            "foo": "bar",
            "2": 5,
            "5": 89009,
            "case": "Random Stuff",
            "PHP Version": "5.5.18"
        }

        // when
        checkProperties(phpFormatter, false, false)

        // then
        compare(phpFormatter.isValid(testValue), true)
        compare(JSON.parse(phpFormatter.getFormatted(testValue)), validResult)
        compare(phpFormatter.getRaw(JSON.stringify(validResult, undefined, 4)), testValue)
    }
}
