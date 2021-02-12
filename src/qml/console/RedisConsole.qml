import QtQuick 2.3
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.1
import "../common"
import "../common/platformutils.js" as PlatformUtils
import "."
import rdm.models 1.0

Rectangle {
    id: root
    color: "#3A3A3A"

    property bool cursorInEditArea: false
    property string prompt
    property int promptPosition
    property int promptLength: prompt.length
    property alias busy: textArea.readOnly

    property string initText:
          "<span style='color: white; font-size: 13pt;'>RDM Redis Console</span><br/>" +
          qsTranslate("RDM","Connecting...")


    function setPrompt(txt, display) {
        console.log("set prompt: ", txt, display)
        prompt = txt

        if (display)
            displayPrompt();
    }

    function displayPrompt() {
        textArea.insert(textArea.length, prompt)
        promptPosition = textArea.length - promptLength
        //textArea.cursorPosition = textArea.length - 1
    }

    function clear() {
        textArea.clear()
    }

    function addOutput(text, type) {

        if (type == "error") {
            textArea.append("<span style='color: red; font-family: "
                            + appSettings.valueEditorFont + "'>"
                            + qmlUtils.escapeHtmlEntities(text) + '</span>')
        } else {
            textArea.append("<pre style='color: white; font-family: "
                            + appSettings.valueEditorFont + "'>"
                            + qmlUtils.escapeHtmlEntities(text) + '</pre>')
        }

        if (type == "complete" || type == "error") {
            textArea.blockAllInput = false
            textArea.append("<br/>")
            displayPrompt()
        }
    }

    signal execCommand(string command)

    BaseConsole {
        id: textArea
        anchors.fill: parent
        backgroundVisible: false
        textColor: "yellow"
        readOnly: root.promptLength == 0 || blockAllInput
        textFormat: TextEdit.RichText
        menu: null

        property bool blockAllInput: false
        property int commandStartPos: root.promptPosition + root.promptLength

        function getCurrentCommand() {
            return getText(commandStartPos, length)
        }

        Keys.onPressed: {
            if (readOnly) {
                console.log("Console is read-only. Ignore Key presses.")
                return
            }

            var cursorInReadOnlyArea = cursorPosition < commandStartPos

            if (event.key == Qt.Key_Backspace && cursorPosition <= commandStartPos) {
                event.accepted = true
                console.log("Block backspace")
                return
            }

            if (event.key == Qt.Key_Left && cursorPosition <= commandStartPos) {
                event.accepted = true
                console.log("Block left arrow")
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
                    && cursorPosition == commandStartPos) {
                event.accepted = true
                console.log("Block Undo")
                return
            }

            if (selectionStart < commandStartPos
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

            if (event.key == Qt.Key_Up || event.key == Qt.Key_Down) {
                var command;

                if (commandsHistoryModel.historyNavigation) {
                    if (event.key == Qt.Key_Down) {
                        command = commandsHistoryModel.getNextCommand()
                    } else {
                        command = commandsHistoryModel.getPrevCommand()
                    }
                } else {
                    command = commandsHistoryModel.getCurrentCommand()
                }

                remove(commandStartPos, length)
                insert(commandStartPos, command)

                event.accepted = true
                return
            }

            if (event.key == Qt.Key_Return && cursorPosition > commandStartPos) {
                var command = getText(commandStartPos, length)
                blockAllInput = true
                event.accepted = true

                if (command.toLowerCase() === "clear") {
                    root.clear()
                    root.displayPrompt()
                    blockAllInput = false
                } else {
                    root.execCommand(command)
                }

                commandsHistoryModel.appendCommand(command)
            }

            autocompleteModel.filterString = getText(commandStartPos, cursorPosition) + event.text
        }

        Component.onCompleted: {
            textArea.text = root.initText
        }

        MouseArea {
            anchors.fill: parent
            acceptedButtons: Qt.RightButton

            onClicked: {
                menu.popup()
            }
        }

        Menu {
            id: menu

            MenuItem {
                text: qsTranslate("RDM","Clear")
                iconSource: "qrc:/images/cleanup.svg"
                onTriggered: {
                    root.clear()
                    root.displayPrompt()
                }
            }
        }
    }

    ColumnLayout {
        height: 150
        width: root.width - x - 50

        x: textArea.cursorRectangle? textArea.cursorRectangle.x : 0
        y: textArea.cursorRectangle? textArea.cursorRectangle.y + 20 : 0
        z: 255
        visible: {
            return cmdAutocomplete.rowCount > 0
                    && autocompleteModel.filterString.length > 0
                    && textArea.cursorPosition >= textArea.commandStartPos
        }

        TableView {
            id: cmdAutocomplete

            Layout.fillWidth: true
            Layout.fillHeight: true

            model: autocompleteModel

            headerVisible: true

            TableViewColumn {
                title: "Command"
                role: "name"
                width: 120
            }

            TableViewColumn {
                title: qsTranslate("RDM","Arguments")
                role: "arguments"
                width: 250
            }

            TableViewColumn {
                title: qsTranslate("RDM","Description")
                role: "summary"
                width: 350
            }

            TableViewColumn {
                title: qsTranslate("RDM","Available since")
                role: "since"
                width: 60
            }

            itemDelegate: Item {
                Text {
                    anchors.fill: parent
                    color: styleData.textColor
                    elide: styleData.elideMode
                    text: styleData.value
                    wrapMode: Text.WrapAnywhere
                    maximumLineCount: 1
                }

                MouseArea {
                    enabled: styleData.column === 2 || styleData.column === 0
                    anchors.fill: parent
                    cursorShape: Qt.PointingHandCursor
                    onClicked: {
                        var commandName = "#"
                        try {
                            commandName = consoleAutocompleteModel.getRow(
                                        autocompleteModel.getOriginalRowIndex(styleData.row)
                            )["name"]
                        } catch(err) {
                            console.log("Cannot get command name:", err)
                        }

                        if (styleData.column === 2) {
                            Qt.openUrlExternally("https://redis.io/commands/" + commandName)
                        } else {
                            textArea.remove(textArea.commandStartPos, textArea.cursorPosition)
                            textArea.insert(textArea.commandStartPos, commandName)
                            autocompleteModel.filterString = commandName
                        }
                    }
                }
            }
        }

        RowLayout {
            Layout.minimumWidth: 150
            Layout.minimumHeight: closeBtn.implicitHeight

            Item {
                Layout.fillWidth: true
            }

            Button {
                id: closeBtn
                text: qsTranslate("RDM","Close")
                onClicked: {
                    autocompleteModel.filterString = ""
                }
            }
        }
    }

    SortFilterProxyModel {
        id: autocompleteModel
        source: consoleAutocompleteModel
        sortOrder: cmdAutocomplete.sortIndicatorOrder
        sortCaseSensitivity: Qt.CaseInsensitive
        sortRole: "name"

        filterSyntax: SortFilterProxyModel.FixedString
        filterCaseSensitivity: Qt.CaseInsensitive
        filterRole: "name"
    }

    TextArea {
        id: hiddenBuffer
        visible: false
        textFormat: TextEdit.PlainText
    }

    ListModel {
        id: commandsHistoryModel
        property int currentIndex: 0
        property bool historyNavigation: false

        function getCurrentCommand() {
            checkCurrentPos()
            var res = get(currentIndex)
            historyNavigation = true
            return res["cmd"]
        }

        function getNextCommand() {
            currentIndex += 1
            return getCurrentCommand()
        }

        function getPrevCommand() {
            currentIndex -= 1
            return getCurrentCommand()
        }

        function checkCurrentPos() {
            if (currentIndex >= count)
                currentIndex = commandsHistoryModel.count - 1

            if (currentIndex < 0)
                currentIndex = 0
        }

        function appendCommand(cmdStr) {
            append({"cmd": cmdStr})
            currentIndex = count - 1
            historyNavigation = false
        }
    }
}
