import QtQuick 2.0
import QtQuick.Controls 1.4

TextArea {
    id: root

    function clear() {
        text = ""
    }

    wrapMode: TextEdit.WrapAnywhere
    textFormat: TextEdit.PlainText
}
