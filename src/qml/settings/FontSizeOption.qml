import QtQuick 2.3
import QtQuick.Layouts 1.1
import QtQuick.Controls 2.2

import "../common"
import "."

ComboboxOption {
    id: root

    property int minFontSize: 4
    property int maxFontSize: 16

    Component.onCompleted: {
        var m = []
        for (var c=minFontSize; c <= maxFontSize; c++) {
            m.push(c)
        }
        model = m;
    }
}

