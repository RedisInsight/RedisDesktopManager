import QtQuick 2.3
import QtQuick.Layouts 1.1
import QtQuick.Controls 2.3
import QtQuick.Controls.Styles 1.1
import QtQuick.Dialogs 1.2
import QtQuick.Window 2.2
import "./../common"


Repeater {
    id: root

    BetterTab {
        id: tab
        objectName: "rdm_console_tab"

        Component {
            id: consoleTabButton

            BetterTabButton {
                icon.source: "qrc:/images/console.svg"

                text: tabName
                tooltip: tabName

                onCloseClicked: {
                    consoleModel.closeTab(tabIndex)
                }
            }
        }

        Component.onCompleted: {           
            var tabButton = consoleTabButton.createObject(tab);
            tabButton.self = tabButton;
            tabButton.tabRef = tab;
            tabBar.addItem(tabButton)
            tabBar.activateTabButton(tabButton)
            tabs.activateTab(tab)

            tabModel.init()
        }

        RedisConsole {
            id: redisConsole            

            anchors.fill: parent

            Connections {
                target: tabModel

                function onChangePrompt(text, showPrompt) { redisConsole.setPrompt(text, showPrompt) }
                function onAddOutput(text, resultType) { redisConsole.addOutput(text, resultType) }
            }

            onExecCommand: tabModel.executeCommand(command)
        }
    }
}
