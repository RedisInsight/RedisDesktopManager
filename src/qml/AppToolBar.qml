import QtQuick 2.0
import QtQuick.Layouts 1.1
import QtQuick.Controls 2.15
import QtQuick.Controls.Styles 1.1
import Qt.labs.platform 1.1
import QtQml.Models 2.2
import "."
import "./common"
import "./common/platformutils.js" as PlatformUtils

ToolBar {

    background: Rectangle {
        implicitHeight: 40
        color: sysPalette.button
    }

    RowLayout {
        anchors.fill: parent
        spacing: 0

        RowLayout {
            Layout.maximumWidth: connectionsTree.width + 1

            BetterButton {
                Layout.fillWidth: true
                Layout.minimumWidth: 190
                iconSource: PlatformUtils.getThemeIcon("add.svg")
                text: qsTranslate("RESP","Connect to Redis Server")
                objectName: "rdm_connect_to_redis_server_btn"

                onClicked: {
                    connectionSettingsDialog.settings = connectionsManager.createEmptyConfig()
                    connectionSettingsDialog.open()
                }
            }


            ImageButton {
                id: connectionsMenuBtn
                objectName: "rdm_connections_menu_btn"

                Layout.preferredWidth: 30
                iconSource: PlatformUtils.getThemeIcon("list.svg")

                onClicked: menu.open()

                FileDialog {
                    id: importConnectionsDialog
                    title: qsTranslate("RESP","Import Connections")
                    nameFilters: ["Connections (*.json)"]
                    fileMode: FileDialog.OpenFile
                    onAccepted: connectionsManager.importConnections(qmlUtils.getPathFromUrl(file))
                }

                FileDialog {
                    id: exportConnectionsDialog
                    title: qsTranslate("RESP","Export Connections")
                    nameFilters: ["Connections (*.json)"]
                    fileMode: FileDialog.SaveFile
                    onAccepted: connectionsManager.saveConnectionsConfigToFile(qmlUtils.getPathFromUrl(file))
                }

                BetterMenu {
                    id: menu

                    BetterMenuItem {
                        objectName: "rdm_import_connections_btn"
                        text: qsTranslate("RESP","Import Connections")
                        onTriggered: importConnectionsDialog.open()
                    }
                    BetterMenuItem {
                        objectName: "rdm_export_connections_btn"
                        text: qsTranslate("RESP","Export Connections")
                        onTriggered: exportConnectionsDialog.open()
                    }
                }
            }

            ImageButton {
                id: toggleTreeViewBtn
                Layout.preferredWidth: 30
                iconSource: PlatformUtils.getThemeIcon("square-half.svg")
                imgWidth: 15
                imgHeight: 15

                onClicked: {
                    connectionsTreeWrapper.visible = !connectionsTreeWrapper.visible
                }
            }
        }

        Rectangle { width: 1; color: sysPalette.mid; Layout.fillHeight: true;}

        Item { Layout.fillWidth: true }

        BetterButton {
            implicitWidth: 40
            iconSource: PlatformUtils.getThemeIcon("alert.svg")
            tooltip: qsTranslate("RESP","Report issue")
            onClicked: Qt.openUrlExternally("https://github.com/uglide/RedisDesktopManager/issues")
        }

        BetterButton {
            implicitWidth: 40
            iconSource: PlatformUtils.getThemeIcon("help.svg")
            tooltip: qsTranslate("RESP","Documentation")
            onClicked: Qt.openUrlExternally("http://docs.resp.app/en/latest/")
        }

        BetterButton {
            implicitWidth: 40
            iconSource: PlatformUtils.getThemeIcon("telegram.svg")
            tooltip: qsTranslate("RESP","Join Telegram Chat")
            onClicked: Qt.openUrlExternally("https://t.me/RedisDesktopManager")
        }

        BetterButton {
            implicitWidth: 40
            iconSource: PlatformUtils.getThemeIcon("twi.svg")
            tooltip: qsTranslate("RESP","Follow")
            onClicked: Qt.openUrlExternally("https://twitter.com/dev_rdm")
        }

        BetterButton {
            implicitWidth: 40
            iconSource: PlatformUtils.getThemeIcon("github.svg")
            tooltip: qsTranslate("RESP","Star on GitHub!")
            onClicked: Qt.openUrlExternally("https://github.com/uglide/RedisDesktopManager")
        }

        Item { Layout.fillWidth: true }

        BetterButton {
            iconSource: PlatformUtils.getThemeIcon("log.svg")
            text: qsTranslate("RESP","Log")

            onClicked: logDrawer.open()
        }

        BetterButton {
            objectName: "rdm_extension_server_settings_btn"
            iconSource: PlatformUtils.getThemeIcon("server_2.svg")
            text: qsTranslate("RESP","Extension Server")

            onClicked: {
                extServerSettingsDialog.item.open()
            }
        }

        BetterButton {
            objectName: "rdm_global_settings_btn"
            iconSource: PlatformUtils.getThemeIcon("settings.svg")
            text: qsTranslate("RESP","Settings")

            onClicked: {
                settingsDialog.item.open()
            }
        }
    }
}

