import QtQuick 2.0
import QtQuick.Layouts 1.1
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.1
import QtQuick.Dialogs 1.2

ColumnLayout {

    state: "edit"

    states: [
        State { name: "new"}, // Creating new key
        State { name: "add"}, // Adding new value to existing key
        State { name: "edit"} // Editing existing key
    ]

    function initEmpty() {
        console.exception("Not implemented")
    }

    function validateValue(callback) {
        console.exception("Not implemented")
    }

    function isEdited() {
        console.exception("Not implemented")
    }

    function setValue(value) {
        console.exception("Not implemented")
    }

    function getValue() {
        console.exception("Not implemented")
    }

    function reset() {
        console.exception("Not implemented")
    }
}
