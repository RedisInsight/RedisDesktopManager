import QtQuick 2.0
import QtQuick.Layouts 1.1
import QtQuick.Controls 2.3
import QtQuick.Controls.Styles 1.1
import QtQuick.Dialogs 1.2
import QtQml.Models 2.2
import "."
import "./common"

ToolBar {
    RowLayout {
        anchors.fill: parent

        BetterButton {
            Layout.preferredWidth: 190
            iconSource: "qrc:/images/add.svg"
            text: qsTranslate("RDM","Connect to Redis Server")
            objectName: "rdm_connect_to_redis_server_btn"

            onClicked: {
                connectionSettingsDialog.settings = connectionsManager.createEmptyConfig()
                connectionSettingsDialog.open()
            }
        }

        BetterButton {
            Layout.preferredWidth: 90
            iconSource: "qrc:/images/import.svg"
            text: qsTranslate("RDM","Import")
            tooltip: qsTranslate("RDM","Import Connections")
            objectName: "rdm_import_connections_btn"

            onClicked: importConnectionsDialog.open()

            FileDialog {
                id: importConnectionsDialog
                title: qsTranslate("RDM","Import Connections")
                nameFilters: ["RDM Connections (*.xml *.json)"]
                selectExisting: true
                onAccepted: connectionsManager.importConnections(qmlUtils.getPathFromUrl(fileUrl))
            }
        }

        BetterButton {
            Layout.preferredWidth: 90
            iconSource: "qrc:/images/export.svg"
            text: qsTranslate("RDM","Export")
            tooltip: qsTranslate("RDM","Export Connections")
            objectName: "rdm_export_connections_btn"

            onClicked: exportConnectionsDialog.open()

            FileDialog {
                id: exportConnectionsDialog
                title: qsTranslate("RDM","Import Connections")
                nameFilters: ["RDM Connections (*.json)"]
                selectExisting: false
                onAccepted: connectionsManager.saveConnectionsConfigToFile(qmlUtils.getPathFromUrl(fileUrl))
            }
        }

        Rectangle { width: 1; color: "lightgrey"; Layout.fillHeight: true;}

        Item { Layout.fillWidth: true }

        BetterButton {
            implicitWidth: 40
            iconSource: "qrc:/images/alert.svg"            
            tooltip: qsTranslate("RDM","Report issue")
            onClicked: Qt.openUrlExternally("https://github.com/uglide/RedisDesktopManager/issues")
        }

        BetterButton {
            implicitWidth: 40
            iconSource: "qrc:/images/help.svg"            
            tooltip: qsTranslate("RDM","Documentation")
            onClicked: Qt.openUrlExternally("http://docs.redisdesktop.com/en/latest/")
        }

        BetterButton {
            implicitWidth: 40
            iconSource: "qrc:/images/telegram.svg"            
            tooltip: qsTranslate("RDM","Join Telegram Chat")
            onClicked: Qt.openUrlExternally("https://t.me/RedisDesktopManager")
        }

        BetterButton {
            implicitWidth: 40
            iconSource: "qrc:/images/twi.svg"            
            tooltip: qsTranslate("RDM","Follow")
            onClicked: Qt.openUrlExternally("https://twitter.com/RedisDesktop")
        }

        BetterButton {
            implicitWidth: 40
            iconSource: "qrc:/images/github.svg"            
            tooltip: qsTranslate("RDM","Star on GitHub!")
            onClicked: Qt.openUrlExternally("https://github.com/uglide/RedisDesktopManager")
        }

        Item { Layout.fillWidth: true }

        BetterButton {
            iconSource: "qrc:/images/settings.svg"
            text: qsTranslate("RDM","Settings")

            onClicked: {
                settingsDialog.open()
            }
        }

        BetterButton {
            iconSource: "qrc:/images/log.svg"
            text: qsTranslate("RDM","Log")

            onClicked: logDrawer.open()
        }
    }
}

