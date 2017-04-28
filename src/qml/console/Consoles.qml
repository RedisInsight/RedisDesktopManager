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

        onClose: {
            consoleModel.closeTab(tabIndex)
        }

        QConsole {
            id: redisConsole

            property var model: consoleModel.getValue(tabIndex)

            Connections {
                target: consoleModel ? consoleModel.getValue(tabIndex) : null

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
            }
        }
    }
}
