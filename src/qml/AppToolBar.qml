import QtQuick 2.0
import QtQuick.Layouts 1.1
import QtQuick.Controls 2.15
import QtQuick.Controls.Styles 1.1
import QtQuick.Dialogs 1.2
import QtQml.Models 2.2
import "."
import "./common"

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
                iconSource: "qrc:/images/plus.svg"
                text: qsTranslate("RESP","Connect to Redis Server")
                objectName: "rdm_connect_to_redis_server_btn"

                onClicked: {
                    connectionSettingsDialog.settings = connectionsManager.createEmptyConfig()
                    connectionSettingsDialog.open()
                }
            }


            ImageButton {
                id: connectionsMenuBtn
                Layout.preferredWidth: 30
                iconSource: "qrc:/images/list.svg"

                onClicked: menu.open()

                FileDialog {
                    id: importConnectionsDialog
                    title: qsTranslate("RESP","Import Connections")
                    nameFilters: ["Connections (*.json)"]
                    selectExisting: true
                    onAccepted: connectionsManager.importConnections(qmlUtils.getPathFromUrl(fileUrl))
                }

                FileDialog {
                    id: exportConnectionsDialog
                    title: qsTranslate("RESP","Export Connections")
                    nameFilters: ["Connections (*.json)"]
                    selectExisting: false
                    onAccepted: connectionsManager.saveConnectionsConfigToFile(qmlUtils.getPathFromUrl(fileUrl))
                }

                Menu {
                    id: menu

                    MenuItem {
                        objectName: "rdm_import_connections_btn"
                        text: qsTranslate("RDM","Import Connections")
                        onTriggered: importConnectionsDialog.open()
                    }
                    MenuItem {
                        objectName: "rdm_export_connections_btn"
                        text: qsTranslate("RDM","Export Connections")
                        onTriggered: exportConnectionsDialog.open()
                    }
                }
            }

            ImageButton {
                id: toggleTreeViewBtn
                Layout.preferredWidth: 30
                iconSource: "qrc:/images/square-half.svg"
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
            iconSource: "qrc:/images/alert.svg"
            tooltip: qsTranslate("RESP","Report issue")
            onClicked: Qt.openUrlExternally("https://github.com/uglide/RedisDesktopManager/issues")
        }

        BetterButton {
            implicitWidth: 40
            iconSource: "qrc:/images/help.svg"
            tooltip: qsTranslate("RESP","Documentation")
            onClicked: Qt.openUrlExternally("http://docs.resp.app/en/latest/")
        }

        BetterButton {
            implicitWidth: 40
            iconSource: "qrc:/images/telegram.svg"
            tooltip: qsTranslate("RESP","Join Telegram Chat")
            onClicked: Qt.openUrlExternally("https://t.me/RedisDesktopManager")
        }

        BetterButton {
            implicitWidth: 40
            iconSource: "qrc:/images/twi.svg"
            tooltip: qsTranslate("RESP","Follow")
            onClicked: Qt.openUrlExternally("https://twitter.com/dev_rdm")
        }

        BetterButton {
            implicitWidth: 40
            iconSource: "qrc:/images/github.svg"
            tooltip: qsTranslate("RESP","Star on GitHub!")
            onClicked: Qt.openUrlExternally("https://github.com/uglide/RedisDesktopManager")
        }

        Item { Layout.fillWidth: true }

        BetterButton {
            iconSource: "qrc:/images/log.svg"
            text: qsTranslate("RESP","Log")

            onClicked: logDrawer.open()
        }

        BetterButton {
            objectName: "rdm_global_settings_btn"
            iconSource: "qrc:/images/settings.svg"
            text: qsTranslate("RESP","Settings")

            onClicked: {
                settingsDialog.item.open()
            }
        }
    }
}

