import QtQuick 2.0
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.13
import QtQuick.Controls.Styles 1.1
import QtQml.Models 2.2
import QtQuick.Window 2.2
import Qt.labs.settings 1.0
import QtQuick.Dialogs 1.3 as LegacyDialogs
import "."
import "./common"
import "./common/platformutils.js" as PlatformUtils
import "./value-editor/"
import "./value-editor/editors/formatters/"
import "./connections"
import "./connections-tree"
import "./console"
import "./server-actions"
import "./bulk-operations"
import "./settings"

ApplicationWindow {
    id: approot
    visible: true
    objectName: "rdm_qml_root"
    title: "RESP.app - GUI for Redis® " + Qt.application.version
    width: 1180
    height: 800
    minimumWidth: 1000
    minimumHeight: 600

    property bool darkModeEnabled: sysPalette.base.hslLightness < 0.4

    property double wRatio : (width * 1.0) / (Screen.width * 1.0)
    property double hRatio : (height * 1.0) / (Screen.height * 1.0)

    property var currentValueFormatter
    property var embeddedFormatters

    ValueFormatters {
        id: valueFormattersModel        
    }


    Component.onCompleted: {
        if (hRatio > 1 || wRatio > 1) {
            console.log("Ratio > 1.0. Resize main window.")
            width = Screen.width * 0.9
            height = Screen.height * 0.8
        }

        if (Qt.platform.os == "windows") {
            x = Screen.width / 2 - width / 2
            y = Screen.height / 2 - height / 2
        }

        appSplitView.restoreState(windowSettings.splitView)
    }

    Component.onDestruction: windowSettings.splitView = appSplitView.saveState()

    Settings {
        id: windowSettings
        category: "windows_settings"
        property alias width: approot.width
        property alias height: approot.height
        property var splitView
    }

    Settings {
        id: appSettings
        category: "app"
        property string valueEditorFont
        property string valueEditorFontSize
        property int valueSizeLimit: 1500000
    }

    Settings {
        id: defaultFormatterSettings
        category: "formatter_overrides"
    }

    Settings {
        id: defaultCompressionSettings
        category: "compression_overrides"
    }

    SystemPalette {
        id: sysPalette
    }

    SystemPalette {
        id: inactiveSysPalette
        colorGroup: SystemPalette.Inactive
    }

    SystemPalette {
        id: disabledSysPalette
        colorGroup: SystemPalette.Disabled
    }

    QuickStartDialog {
        id: quickStartDialog
        objectName: "rdm_qml_quick_start_dialog"

        width: PlatformUtils.isOSX() ? 600 : approot.width * 0.8
    }

    Loader {
        id: settingsDialog

        asynchronous: true
        source: "settings/GlobalSettings.qml"
    }

    Loader {
        id: extServerSettingsDialog

        asynchronous: true
        source: "extension-server/ExtensionServerSettings.qml"
    }

    ConnectionSettignsDialog {
        id: connectionSettingsDialog

        objectName: "rdm_connection_settings_dialog"

        onTestConnection: {
            connectionsManager.testConnectionSettings(settings, connectionTested)
        }

        function connectionTested(result) {
            if (result) {
                hideLoader()
                showMsg(qsTranslate("RESP","Successful connection to redis-server"))
            } else {
                hideLoader()
                showError(qsTranslate("RESP","Can't connect to redis-server"))
            }
        }
        onSaveConnection: connectionsManager.updateConnection(settings)
    }

    AskSecretDialog {
        id: askSecretDialog
    }

    ConnectionGroupDialog {
        id: connectionGroupDialog

        objectName: "rdm_connection_group_dialog"

        onAddNewGroup: {
            connectionsManager.addNewGroup(name)
        }

        onEditGroup: {
            connectionsManager.updateGroup(group)
        }
    }

    Loader {
        id: notification

        property var icon
        property string text
        property string details

        function showError(msg, details="") {
            icon = LegacyDialogs.StandardIcon.Warning
            text = msg
            notification.details = details
            sourceComponent = notificationTemplate
        }

        function showMsg(msg) {
            icon = LegacyDialogs.StandardIcon.Information
            text = msg
            details = ""
            sourceComponent = notificationTemplate
        }

        onLoaded: {
            item.open()
        }

        Component {
            id: notificationTemplate

            OkDialog {
                objectName: "rdm_qml_error_dialog"
                visible: false

                icon: notification.icon
                text: notification.text
                detailedText: notification.details

                onVisibleChanged: {
                    if (!visible) {
                        notification.sourceComponent = undefined
                    }
                }
            }
        }
    }

    AddKeyDialog {
        id: addNewKeyDialog        
    }

    Connections {
        target: serverStatsModel
        ignoreUnknownSignals: true
        function onError(error) { notification.showError(error) }
    }

    Connections {
        target: keyFactory

        function onNewKeyDialog(r) {
            addNewKeyDialog.request = r
            addNewKeyDialog.open()
        }
    }

    BulkOperationsDialog {
        id: bulkOperationDialog
    }

    Connections {
        target: bulkOperations

        function onOpenDialog(operationName) {
            bulkOperationDialog.operationName = operationName
            bulkOperationDialog.open()
        }
    }

    Connections {
        target: appEvents

        function onError(msg) {
            notification.showError(msg)
        }

        function onPythonLoaded() {
            valueFormattersModel.loadEmbeddedFormatters();            
            valueFormattersModel.updateRWFormatters();
        }


        function onExternalFormattersLoaded() {
            valueFormattersModel.loadExternalFormatters();
            valueFormattersModel.updateRWFormatters();
        }
    }

    Connections {
        target: connectionsManager

        function onEditConnection(config) {
            connectionSettingsDialog.settings = config
            connectionSettingsDialog.open()
        }

        function onEditConnectionGroup(group) {
            connectionGroupDialog.group = group
            connectionGroupDialog.open()
        }

        function onConnectionsLoaded() {
            if (connectionsManager.size() === 0)
                quickStartDialog.open()
        }

        function onAskUserForConnectionSecret(config, id) {
            console.log("Ask user for secret", config.name, id)

            askSecretDialog.secretId = id;
            askSecretDialog.config = config;
            askSecretDialog.open()
            askSecretDialog.forceFocus()
        }
    }

    header: AppToolBar {}

    Rectangle {
        id: appWrapper
        anchors.fill: parent
        color: sysPalette.base
        border.color: sysPalette.mid
        border.width: 1

    BetterSplitView {
        id: appSplitView
        anchors.fill: parent
        anchors.topMargin: 1
        orientation: Qt.Horizontal

        ColumnLayout {
            id: connectionsTreeWrapper
            SplitView.fillHeight: true
            SplitView.minimumWidth: 404
            SplitView.minimumHeight: 500

            BetterTreeView {
                id: connectionsTree

                Layout.fillHeight: true
                Layout.fillWidth: true
            }

            RowLayout {
                Layout.fillWidth: true
                Layout.margins: 10

                BetterButton {
                    id: addConnectionGroupBtn
                    objectName: "rdm_add_group_btn"
                    iconSource: PlatformUtils.getThemeIcon("add.svg")
                    text: qsTranslate("RESP", "Add Group")

                    Layout.fillWidth: true

                    visible: sortButton.visible

                    onClicked: {
                        connectionGroupDialog.group = undefined
                        connectionGroupDialog.open()
                    }
                }

                BetterButton {
                    id: sortButton
                    objectName: "rdm_regroup_connections_btn"
                    text: qsTranslate("RESP", "Regroup connections")

                    iconSource: PlatformUtils.getThemeIcon("sort.svg")

                    Layout.fillWidth: true

                    onClicked: {
                        connectionsTree.sortConnections = true
                        connectionsTree.selection.clear()
                        connectionsTree.backgroundVisible = true

                        connectionsManager.collapseRootItems()

                        sortButton.visible = false
                    }
                }

                BetterButton {
                    id: sortApplyButton
                    objectName: "rdm_exit_regroup_mode_btn"
                    Layout.fillWidth: true

                    text: qsTranslate("RESP", "Exit Regroup Mode")
                    visible: !sortButton.visible

                    iconSource: PlatformUtils.getThemeIcon("ok.svg")

                    onClicked: {
                        connectionsTree.sortConnections = false
                        connectionsTree.backgroundVisible = false
                        connectionsManager.applyGroupChanges()
                        sortButton.visible = true
                    }
                }
            }
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
                Layout.minimumWidth: 550
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

                ServerActionTabs {
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

                function onTabError(index, error) {
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
        dragMargin: 0
        width: 0.66 * approot.width
        height: approot.height
        position: 0.3
        edge: Qt.LeftEdge
        background: Rectangle {
            color: sysPalette.base
            border.color: sysPalette.mid
        }

        LogView {
            anchors.fill: parent
            eventsModel: appEvents
        }
    }

}
