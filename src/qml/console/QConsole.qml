import QtQuick 2.3
import QtQuick.Controls 1.4
import "."

Rectangle {
    id: root
    color: "#3A3A3A"

    property bool cursorInEditArea: false
    property string prompt
    property int promptPosition
    property int promptLength: prompt.length

    property string initText:
          "<span style='color: white; font-size: 14px;'>RDM Redis Console</span> | <span style='color: orange'>Unsupported commands: DUMP, RESTORE, AUTH </span><br/>" +
          "Connecting ..."


    function setPrompt(txt, display) {
        console.log("set prompt: ", txt, display)
        prompt = txt

        if (display)
            displayPrompt();
    }

    function displayPrompt() {
        textArea.insert(textArea.length, "<br/>" + prompt)
        promptPosition = textArea.length - promptLength
        //textArea.cursorPosition = textArea.length - 1
    }

    function clear() {
        textArea.clear()
    }

    BaseConsole {
        id: textArea
        anchors.fill: parent
        backgroundVisible: false
        textColor: "yellow"
        readOnly: root.prompt.length == 0
        textFormat: TextEdit.RichText
        menu: null

        Keys.onPressed: {
            if (readOnly)
                return

            var cursorInReadOnlyArea = cursorPosition < root.promptPosition + root.promptLength

            if (event.key == Qt.Key_Backspace && cursorPosition <= root.promptPosition + root.promptLength) {
                event.accepted = true
                console.log("Block backspace")
                return
            }

            if (((event.modifiers == Qt.NoModifier) || (event.modifiers & Qt.ShiftModifier))
                    && cursorInReadOnlyArea) {
                cursorPosition = length
                event.accepted = true
                console.log("Block Input in Read-Only area")
                return
            }

            if (event.matches(StandardKey.Undo)
                    && cursorPosition == root.promptPosition + root.promptLength) {
                event.accepted = true
                console.log("Block Undo")
                return
            }

            if (selectionStart < root.promptPosition + root.promptLength
                    && (event.matches(StandardKey.Cut)
                        || event.key == Qt.Key_Delete
                        || event.key == Qt.Key_Backspace)) {
                event.accepted = true
                console.log("Block Cut/Delete")
                return
            }

            if (event.matches(StandardKey.Paste)) {
                event.accepted = true
                console.log("Block Reach Text Input")
                hiddenBuffer.text = ""
                hiddenBuffer.paste()

                if (cursorInReadOnlyArea)
                    cursorPosition = length

                insert(cursorPosition, hiddenBuffer.text.trim())

                return
            }
        }

        Component.onCompleted: {
            textArea.text = root.initText
        }
    }

    TextArea {
        id: hiddenBuffer
        visible: false
        textFormat: TextEdit.PlainText
    }
}
