import QtQuick 2.0
import QtQuick.Layouts 1.1
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.1
import QtQuick.Dialogs 1.2
import QtQml.Models 2.2
import QtQuick.Window 2.2
import Qt.labs.settings 1.0
import "."
import "./common"
import "./common/platformutils.js" as PlatformUtils
import "./value-editor"
import "./connections-tree"
import "./console"
import "./server-info"
import "./bulk-operations"

ApplicationWindow {
    id: approot
    visible: true
    objectName: "rdm_qml_root"
    title: "Redis Desktop Manager " + Qt.application.version
    width: 1100
    height: 800

    property double wRatio : (width * 1.0) / (Screen.width * 1.0)
    property double hRatio : (height * 1.0) / (Screen.height * 1.0)

    property var currentValueFormatter

    Component.onCompleted: {
        if (hRatio > 1 || wRatio > 1) {
            console.log("Ratio > 1.0. Resize main window.")
            width = Screen.width * 0.9
            height = Screen.height * 0.8
        }

        if (PlatformUtils.isOSXRetina(Screen)) {
            bottomTabView.implicitHeight = 100
        }
    }

    Settings {
        category: "windows_settings"
        property alias x: approot.x
        property alias y: approot.y
        property alias width: approot.width
        property alias height: approot.height
    }

    SystemPalette {
        id: sysPalette
    }

    FontLoader {
        id: monospacedFont
        Component.onCompleted: {
            source = "qrc:/fonts/Inconsolata-Regular.ttf"
        }
    }

    QuickStartDialog {
        id: quickStartDialog
        objectName: "rdm_qml_quick_start_dialog"
    }

    GlobalSettings {
        id: settingsDialog
    }

    ConnectionSettignsDialog {
        id: connectionSettingsDialog

        objectName: "rdm_connection_settings_dialog"

        onTestConnection: {                       
            if (connectionsManager.testConnectionSettings(settings)) {
                hideLoader()
                showMsg(qsTr("Successful connection to redis-server"))
            } else {
                hideLoader()
                showError(qsTr("Can't connect to redis-server"))
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

    BulkOperationsDialog {
        id: bulkOperationDialog
    }

    Connections {
        target: bulkOperations

        onOpenDialog: {
            bulkOperationDialog.operationName = operationName
            bulkOperationDialog.open()
        }
    }

    Connections {
        target: connectionsManager

        onEditConnection: {
            connectionSettingsDialog.settings = config
            connectionSettingsDialog.open()
        }

        onError: {            
            notification.showError(err)
        }

        Component.onCompleted: {
            if (connectionsManager.size() == 0)
                quickStartDialog.open()
        }
    }

    toolBar: AppToolBar {}

    BetterSplitView {
        anchors.fill: parent
        orientation: Qt.Horizontal        

        BetterTreeView {
            id: connectionsTree
            Layout.fillHeight: true
            Layout.minimumWidth: 350
            Layout.minimumHeight: 500
        }

        BetterSplitView {
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

                    if (tabs.getTab(currentIndex).tabType) {
                        if (tabs.getTab(currentIndex).tabType == "value") {

                            var realIndex = currentIndex - serverStatsModel.tabsCount();

                            if (welcomeTab) {
                                realIndex -= 1
                            }

                            viewModel.setCurrentTab(realIndex);
                        } else if (tabs.getTab(currentIndex).tabType == "server_info") {
                            var realIndex = currentIndex;

                            if (welcomeTab) {
                                realIndex -= 1
                            }

                            serverStatsModel.setCurrentTab(index);
                        }
                    }
                }

                WelcomeTab {
                    id: welcomeTab
                    clip: true
                    objectName: "rdm_qml_welcome_tab"

                    property bool not_mapped: true

                    onClose: tabs.removeTab(index)

                    function closeIfOpened() {
                        var welcomeTab = tabs.getTab(0)

                        if (welcomeTab && welcomeTab.not_mapped)
                            tabs.removeTab(0)
                    }
                }

                ServerInfoTabs {
                    model: serverStatsModel
                }

                Connections {
                    target: serverStatsModel

                    onRowsInserted: if (welcomeTab) welcomeTab.closeIfOpened()
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
                            tabs.currentIndex = index

                        notification.showError(error)
                    }

                    onRowsInserted: {
                        if (welcomeTab) welcomeTab.closeIfOpened()
                    }

                    onNewKeyDialog: addNewKeyDialog.open()
                }
            }

            BetterTabView {
                id: bottomTabView
                Layout.fillWidth: true
                Layout.minimumHeight: PlatformUtils.isOSXRetina()? 15 : 30

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
                    icon: "qrc:/images/log.svg"

                    BaseConsole {
                        id: logTab
                        readOnly: true
                        textColor: "#6D6D6E"

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
