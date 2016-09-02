import QtQuick 2.3
import QtTest 1.0

import "./../../src/qml/value-editor/editors/formatters/formatters.js" as Formatters

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

    function test_php_data() {
        return [{
                    value: 'a:6:{i:1;s:30:"PHP code tester Sandbox Online";'
                           +'s:3:"foo";s:3:"bar";i:2;i:5;i:5;i:89009;s:4:"case";'
                           +'s:12:"Random Stuff";s:11:"PHP Version";s:6:"5.5.18";}',
                    validResult: {
                        "1": "PHP code tester Sandbox Online",
                        "foo": "bar",
                        "2": 5,
                        "5": 89009,
                        "case": "Random Stuff",
                        "PHP Version": "5.5.18"
                    },
                    valid: true
                },
                {value: "C", valid: false, validResult: undefined},
                {value: "i", valid: false, validResult: undefined},
                {value: "d", valid: false, validResult: undefined},
                {value: "b", valid: false, validResult: undefined},
                {value: "s", valid: false, validResult: undefined},
                {value: "a", valid: false, validResult: undefined},
                {value: "O", valid: false, validResult: undefined},
                {value: "r", valid: false, validResult: undefined},
                {value: "R", valid: false, validResult: undefined},
                {value: "New", valid: false, validResult: undefined},
                {value: "New;", valid: false, validResult: undefined},
                {value: '<pre class="lang-php prettyprint prettyprinted"><code><span class="pln">b</span><span class="pun">:&lt;</span><span class="pln">i</span><span class="pun">&gt;;</span></code></pre>',
                    valid: false, validResult: undefined},
                ]
    }

    function test_php(data) {
        // given
        var phpFormatter = Formatters.phpserialized
        var testValue = data.value
        var validResult = data.validResult
        var isValid = data.valid

        // when
        checkProperties(phpFormatter, false, true)

        // then
        compare(phpFormatter.isValid(testValue), isValid)

        if (isValid)
            compare(JSON.parse(phpFormatter.getFormatted(testValue)), validResult)
    }
}
