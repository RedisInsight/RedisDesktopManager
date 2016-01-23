import QtQuick 2.3
import QtQuick.Layouts 1.1
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.1
import QtQuick.Dialogs 1.2
import QtQuick.Window 2.2
import MeasurementProtocol 1.0
import "./../common"


Repeater {
    id: root

    BetterTab {
        id: tab
        title: consoleName
        icon: "qrc:/images/console.png"

        onClose: {
            consoleModel.closeTab(consoleIndex)
        }

        QConsole {
            id: redisConsole

            Connections {
                target: consoleModel ? consoleModel.getValue(consoleIndex) : null

                onChangePrompt: {                    
                    redisConsole.setPrompt(text, showPrompt)
                }

                onAddOutput: {                    
                    redisConsole.addOutput(text, resultType)
                }
            }

            onExecCommand: {
                consoleModel.getValue(consoleIndex).executeCommand(command)
            }

            Timer {
                id: initTimer

                onTriggered: {
                    consoleModel.getValue(consoleIndex).init()
                }
            }

            Component.onCompleted: {
                tab.icon = Qt.binding(function() {
                    return redisConsole.busy ? "qrc:/images/loader.gif" : "qrc:/images/console.png"
                })
                initTimer.start()
            }
        }
    }
}
