import QtQuick 2.3
import QtTest 1.0

import "./../../src/resources/qml/editors/formatters/formatters.js" as Formatters

TestCase {
    name: "FormatterTests"

    function checkProperties(f, validBin, validRead) {
        verify(plain.title.length != 0, "title")
        compare(f.binary, validBin)
        compare(f.readOnly, validRead)
    }

    function test_plain() {
        var plain = Formatters.plain




    }
}
