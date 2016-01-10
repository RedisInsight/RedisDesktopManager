import QtQuick 2.0
import QtQuick.Layouts 1.1
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.1
import QtQuick.Dialogs 1.2
import QtQml.Models 2.2
import "."
import "./common"
import "./value-editor"
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

    toolBar: ToolBar {
        RowLayout {
            anchors.fill: parent
            Button {
                iconSource: "qrc:/images/add.png"
                text: "Connect to Redis Server"
                Layout.preferredWidth: 230

                onClicked: {
                    connectionSettingsDialog.settings = connectionsManager.createEmptyConfig()
                    connectionSettingsDialog.open()
                }
            }

            ToolButton {
                iconSource: "qrc:/images/import.png"
                text: "Import Connections"
                tooltip: text

                onClicked: importConnectionsDialog.open()

                FileDialog {
                    id: importConnectionsDialog
                    title: "Import Connections"
                    nameFilters: ["RDM Connections (*.xml *.json)"]
                    selectExisting: true
                    onAccepted: connectionsManager.importConnections(qmlUtils.getPathFromUrl(fileUrl))
                }
            }

            ToolButton {
                iconSource: "qrc:/images/export.png"
                text: "Export Connections"
                tooltip: text

                onClicked: exportConnectionsDialog.open()

                FileDialog {
                    id: exportConnectionsDialog
                    title: "Import Connections"
                    nameFilters: ["RDM Connections (*.json)"]
                    selectExisting: false
                    onAccepted: connectionsManager.saveConnectionsConfigToFile(qmlUtils.getPathFromUrl(fileUrl))
                }
            }

            Rectangle { width: 1; color: "lightgrey"; Layout.fillHeight: true;}

            Item { Layout.fillWidth: true }

            Button {
                iconSource: "qrc:/images/settings.png"
                text: "Settings"
            }
        }
    }

    SplitView {
        anchors.fill: parent
        orientation: Qt.Horizontal

        TreeView {
            id: connectionsTree
            Layout.fillHeight: true
            Layout.minimumWidth: 300
            alternatingRowColors: false
            headerVisible: false

            TableViewColumn {
                title: "item"
                role: "name"
                width: 300
            }

            selectionMode: SelectionMode.SingleSelection

            selection: ItemSelectionModel {
                id: connectionTreeSelectionModel
                model: connectionsManager
            }

            model: connectionsManager

            itemDelegate: Item {

                RowLayout {
                    anchors.fill: parent
                    spacing: 5

                    Item {
                        Layout.fillHeight: true
                        Layout.preferredWidth: 5
                    }

                    Item {
                        Layout.fillHeight: true
                        Image {
                            id: itemIcon
                            anchors.centerIn: parent
                            width: 21
                            height: 21
                            source: {
                                if (!connectionsManager)
                                    return

                                return connectionsManager.getItemIcon(styleData.index)
                            }
                        }
                    }

                    Item {
                        Layout.fillHeight: true
                        Layout.preferredWidth: itemIcon.width / 3
                    }

                    Item {
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        Layout.preferredHeight: 35

                        Text {
                            anchors.verticalCenter: parent.verticalCenter
                            color: styleData.textColor
                            elide: styleData.elideMode
                            text: styleData.value
                        }
                    }

                    Loader {
                        Layout.fillHeight: true
                        visible: styleData.selected

                        source: {
                            if (!connectionsManager)
                                return

                            var type = connectionsManager.getItemType(styleData.index)
                            return "./connections-tree/menu/" + type + ".qml"
                        }
                    }
                }

                MouseArea {
                    anchors.fill: parent
                    acceptedButtons: Qt.RightButton | Qt.MiddleButton

                    onClicked: {
                        console.log("Catch event to item")

                        if(mouse.button == Qt.RightButton) {
                            mouse.accepted = true
                            connectionTreeSelectionModel.setCurrentIndex(styleData.index, 1)
                            connectionsManager.sendEvent(styleData.index, "right-click")
                            return
                        }

                        if (mouse.button == Qt.MiddleButton) {
                            mouse.accepted = true
                            connectionsManager.sendEvent(styleData.index, "mid-click")
                            return
                        }
                    }
                }
            }

            onClicked: {
                if (!connectionsManager)
                    return

                connectionsManager.sendEvent(index, "click")
            }
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

                BetterTab {
                    CodeEditor {
                        anchors.fill: parent
                        anchors.margins: 5
                    }
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
                    target: viewModel
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
                Layout.fillWidth: true
                Layout.minimumHeight: 30

                tabPosition: Qt.BottomEdge

                BetterTab {
                    title: "Test Console"
                    icon: "qrc:/images/console.png"

                    QConsole {
                        id: redisConsole

                        Component.onCompleted: {
                            testTimer.start()
                        }

                        Timer {
                            id: testTimer
                            repeat: false
                            interval: 1000

                            onTriggered: {
                                redisConsole.setPrompt("test_server>", true)
                            }
                        }
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
