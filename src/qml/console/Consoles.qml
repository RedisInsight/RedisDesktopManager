import QtQuick 2.3
import QtQuick.Layouts 1.1
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.1
import QtQuick.Dialogs 1.2
import QtQuick.Window 2.2
import "./../common"


Repeater {
    id: root

    BetterTab {
        id: tab
        title: tabName
        icon: "qrc:/images/console.svg"

        property var redisConsoleVar

        RedisConsole {
            id: redisConsole

            property var model: consoleModel.getValue(tabIndex)

            function close() {
                redisConsole.model = null;
                consoleModel.closeTab(tabIndex);
            }

            Connections {
                target: redisConsole.model ? redisConsole.model : null

                onChangePrompt: {                    
                    redisConsole.setPrompt(text, showPrompt)
                }

                onAddOutput: {                    
                    redisConsole.addOutput(text, resultType)
                }
            }

            onExecCommand: {
                if (model)
                    model.executeCommand(command)
            }

            Timer {
                id: initTimer

                onTriggered: {
                    if (model)
                        model.init()
                }
            }

            Component.onCompleted: {
                tab.icon = Qt.binding(function() {
                    return redisConsole.busy ? "qrc:/images/wait.svg" : "qrc:/images/console.svg"
                })
                initTimer.start()
                tab.redisConsoleVar = redisConsole
            }

            Dialog {
                id: closeConfirmation
                title: qsTranslate("RDM","Confirm Action")

                width: 520

                RowLayout {
                    implicitWidth: 500
                    implicitHeight: 100
                    width: 500

                    Text { text: qsTranslate("RDM","Do you really want to close console with running command?") }
                }

                onYes: redisConsole.close()

                visible: false
                modality: Qt.ApplicationModal
                standardButtons: StandardButton.Yes | StandardButton.Cancel
            }

            function closeConfirmation() {
                closeConfirmation.open()
            }
        }      

        onClose: {
            if (redisConsoleVar && redisConsoleVar.busy) {
                redisConsoleVar.closeConfirmation()
                return
            }

            redisConsoleVar.close()
        }
    }
}
