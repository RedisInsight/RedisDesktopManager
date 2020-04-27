import QtQuick 2.0
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.13
import QtQuick.Controls.Styles 1.1
import QtQuick.Dialogs 1.2
import QtQml.Models 2.2
import QtQuick.Window 2.2
import Qt.labs.settings 1.0
import "."
import "./common"
import "./common/platformutils.js" as PlatformUtils
import "./value-editor/"
import "./value-editor/editors/formatters/"
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
    minimumWidth: 1100
    minimumHeight: 700

    property double wRatio : (width * 1.0) / (Screen.width * 1.0)
    property double hRatio : (height * 1.0) / (Screen.height * 1.0)

    property var currentValueFormatter
    property var embeddedFormatters

    ValueFormatters {
        id: valueFormattersModel

        Component.onCompleted: {
            loadEmbeddedFormatters();
            loadExternalFormatters();
        }
    }


    Component.onCompleted: {
        if (hRatio > 1 || wRatio > 1) {
            console.log("Ratio > 1.0. Resize main window.")
            width = Screen.width * 0.9
            height = Screen.height * 0.8
        }        
    }

    Settings {
        category: "windows_settings"
        property alias width: approot.width
        property alias height: approot.height
    }

    Settings {
        id: appSettings
        category: "app"
        property string valueEditorFontSize
    }

    SystemPalette {
        id: sysPalette
    }

    SystemPalette {
        id: inactiveSysPalette
        colorGroup: SystemPalette.Inactive

    }

    QuickStartDialog {
        id: quickStartDialog
        objectName: "rdm_qml_quick_start_dialog"

        width: PlatformUtils.isOSX() ? 600 : approot.width * 0.8
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
                showMsg(qsTranslate("RDM","Successful connection to redis-server"))
            } else {
                hideLoader()
                showError(qsTranslate("RDM","Can't connect to redis-server"))
            }
        }

        onSaveConnection: connectionsManager.updateConnection(settings)
    }

    OkDialog {
        id: notification
        objectName: "rdm_qml_error_dialog"
        visible: false

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

    AddKeyDialog {
        id: addNewKeyDialog        
    }

    Connections {
        target: serverStatsModel
        ignoreUnknownSignals: true
        onError: notification.showError(error)
    }

    Connections {
        target: keyFactory

        onNewKeyDialog: {
            addNewKeyDialog.request = r
            addNewKeyDialog.open()
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
        target: appEvents

        onError: {
            notification.showError(msg)
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

    header: AppToolBar {}

    Rectangle {
        anchors.fill: parent
        color: sysPalette.base
        border.color: sysPalette.mid
        border.width: 1

    BetterSplitView {
        anchors.fill: parent
        anchors.topMargin: 1
        orientation: Qt.Horizontal

        BetterTreeView {
            id: connectionsTree
            SplitView.fillHeight: true
            SplitView.minimumWidth: 383
            SplitView.minimumHeight: 500
        }      

        ColumnLayout {
            SplitView.fillWidth: true
            SplitView.fillHeight: true
            TabBar {
                id: tabBar
                objectName: "rdm_main_tab_bar"
                Layout.fillWidth: true
                Layout.preferredHeight: 30

                background: Rectangle {
                    color: sysPalette.base
                }

                onCountChanged: {
                    updateTimer.start()
                }

                function activateTabButton(item) {
                    for (var btnIndex in contentChildren) {
                        if (contentChildren[btnIndex] == item) {
                            currentIndex = btnIndex;
                            break;
                        }
                    }
                }

                Timer {
                    id: updateTimer
                    interval: 50;
                    running: false;
                    repeat: false
                    onTriggered: {
                        if (tabBar.count > 0) {
                            tabs.activateTab(tabBar.itemAt(tabBar.currentIndex).tabRef)

                            if (tabBar.currentIndex == 0 ) {
                                tabBar.currentIndex = -1
                                tabBar.currentIndex = 0
                            }
                        }
                    }
                }
            }

            StackLayout {
                id: tabs
                objectName: "rdm_qml_tabs"

                Layout.fillHeight: true
                Layout.fillWidth: true
                Layout.minimumWidth: 650
                Layout.minimumHeight: 30

                onCountChanged: {
                    if (count === 1) {
                        currentIndex = 0;
                    }
                }

                function activateTab(item) {
                    var realIndex = 0;
                    for (var tIndex in tabs.children) {
                        if (!tabs.children[tIndex].__isTab) {
                            continue;
                        }

                        if (tabs.children[tIndex] === item) {
                            tabs.currentIndex = realIndex;
                            item.activate();
                            break;
                        }

                        realIndex++;
                    }
                }

                WelcomeTab {
                    id: welcomeTab
                    clip: true
                    objectName: "rdm_qml_welcome_tab"
                    visible: tabs.count == 1
                }

                ServerInfoTabs {
                    objectName: "rdm_qml_server_info_tabs"
                    model: serverStatsModel
                }

                ValueTabs {
                    objectName: "rdm_qml_value_tabs"
                    model: valuesModel
                }

                Consoles {
                    objectName: "rdm_qml_console_tabs"
                    model: consoleModel
                }
            }

            Connections {
                target: valuesModel
                ignoreUnknownSignals: true
                onKeyError: {
                    if (index != -1)
                        tabs.currentIndex = index

                    notification.showError(error)
                }
            }
        }
        }
    }

    Drawer {
        id: logDrawer

        width: 0.66 * approot.width
        height: approot.height
        position: 0.3
        edge: Qt.LeftEdge

        LogView {
            anchors.fill: parent
            eventsModel: appEvents
        }
    }

}
