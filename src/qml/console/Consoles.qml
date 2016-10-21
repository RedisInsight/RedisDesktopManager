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
        title: tabName
        icon: "qrc:/images/console.png"

        onClose: {
            consoleModel.closeTab(tabIndex)
        }

        QConsole {
            id: redisConsole

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
                consoleModel.getValue(tabIndex).executeCommand(command)
            }

            Timer {
                id: initTimer

                onTriggered: {
                    consoleModel.getValue(tabIndex).init()
                }
            }

            Component.onCompleted: {
                tab.icon = Qt.binding(function() {
                    return redisConsole.busy ? "qrc:/images/loader.gif" : "qrc:/images/console.svg"
                })
                initTimer.start()
            }
        }
    }
}
