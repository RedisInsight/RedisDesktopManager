import QtQuick 2.0
import QtQuick.Layouts 1.1
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.1
import QtQuick.Dialogs 1.2
import QtQml.Models 2.2
import "."
import "./common"
import "./value-editor"
import "./connections-tree"
import "./console"
import "./code-editor"
//import "./editors/formatters/formatters.js" as Formatters

ApplicationWindow {
    id: approot
    visible: true
    objectName: "rdm_qml_root"
    title: "Redis Desktop Manager " + Qt.application.version
    width: 1000
    height: 800

    property var currentValueFormatter

    SystemPalette {
        id: sysPalette
    }

    QuickStartDialog {
       id: quickStartDialog
       objectName: "rdm_qml_quick_start_dialog"
    }

    ConnectionSettignsDialog {
        id: connectionSettingsDialog

        onTestConnection: {
            if (connectionsManager.testConnectionSettings(settings)) {
                notification.showMsg("Successful connection to redis-server")
            } else {
                notification.showError("Can't connect to redis-server")
            }
        }

        onSaveConnection: connectionsManager.updateConnection(settings)
    }

    MessageDialog {
        id: notification
        objectName: "rdm_qml_error_dialog"
        visible: false
        modality: Qt.WindowModal
        icon: StandardIcon.Warning
        standardButtons: StandardButton.Ok

        function showError(msg) {
            icon = StandardIcon.Warning
            text = msg
            open()
        }

        function showMsg(msg) {
            icon = StandardIcon.Information
            text = msg
            open()
        }
    }

    Connections {
        target: connectionsManager

        onEditConnection: {
            connectionSettingsDialog.settings = config
            connectionSettingsDialog.open()           
        }

        Component.onCompleted: {
            if (connectionsManager.size() == 0)
                quickStartDialog.open()
        }
    }

    toolBar: AppToolBar {}

    SplitView {
        anchors.fill: parent
        orientation: Qt.Horizontal

        BetterTreeView {
            id: connectionsTree
            Layout.fillHeight: true
            Layout.minimumWidth: 300
        }

        SplitView {
            orientation: Qt.Vertical

            BetterTabView {
                id: tabs
                objectName: "rdm_qml_tabs"
                currentIndex: 0

                Layout.fillHeight: true
                Layout.fillWidth: true
                Layout.minimumWidth: 650
                Layout.minimumHeight: 30

                onCurrentIndexChanged: {
                    var index = currentIndex
                    if (tabs.getTab(0).not_mapped) index -= 1
                    viewModel.setCurrentTab(index)
                }

                WelcomeTab {
                    clip: true
                    objectName: "rdm_qml_welcome_tab"

                    property bool not_mapped: true

                    onClose: tabs.removeTab(index)
                }                

                ValueTabs {
                    objectName: "rdm_qml_value_tabs"
                    model: valuesModel
                }

                AddKeyDialog {
                   id: addNewKeyDialog
                   objectName: "rdm_qml_new_key_dialog"
                }

                Connections {
                    target: valuesModel
                    onKeyError: {
                        if (index != -1)
                            tabs.currentIndex = index + 1

                        notification.showError(error)
                    }

                    onCloseWelcomeTab: {
                        var welcomeTab = tabs.getTab(0)

                        if (welcomeTab && welcomeTab.not_mapped)
                           tabs.removeTab(0)
                    }

                    onNewKeyDialog: addNewKeyDialog.open()
                }
            }

            BetterTabView {
                id: bottomTabView
                Layout.fillWidth: true
                Layout.minimumHeight: 30

                tabPosition: Qt.BottomEdge

                Consoles {
                    objectName: "rdm_qml_console_tabs"
                    model: consoleModel
                }

                Connections {
                    target: consoleModel

                    onChangeCurrentTab: {
                        bottomTabView.currentIndex = i + 1
                    }
                }

                BetterTab {                    
                    closable: false
                    title: "Log"
                    icon: "qrc:/images/log.png"

                    BaseConsole {
                        id: logTab
                        readOnly: true
                        textColor: "darkgrey"

                        Connections {
                            target: appLogger
                            onEvent: logTab.append(msg)
                            Component.onCompleted: appLogger.getMessages()
                        }
                    }
                }                                
            }
        }
    }
}
