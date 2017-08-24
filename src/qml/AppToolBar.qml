import QtQuick 2.0
import QtQuick.Layouts 1.1
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.1
import QtQuick.Dialogs 1.2
import QtQml.Models 2.2
import "."

ToolBar {
    RowLayout {
        anchors.fill: parent
        Button {
            iconSource: "qrc:/images/add.svg"
            text: qsTr("Connect to Redis Server")
            objectName: "rdm_connect_to_redis_server_btn"
            Layout.preferredWidth: 230

            onClicked: {
                connectionSettingsDialog.settings = connectionsManager.createEmptyConfig()
                connectionSettingsDialog.open()
            }
        }

        ToolButton {
            iconSource: "qrc:/images/import.svg"
            text: qsTr("Import Connections")
            tooltip: text

            onClicked: importConnectionsDialog.open()

            FileDialog {
                id: importConnectionsDialog
                title: qsTr("Import Connections")
                nameFilters: ["RDM Connections (*.xml *.json)"]
                selectExisting: true
                onAccepted: connectionsManager.importConnections(qmlUtils.getPathFromUrl(fileUrl))
            }
        }

        ToolButton {
            iconSource: "qrc:/images/export.svg"
            text: qsTr("Export Connections")
            tooltip: text

            onClicked: exportConnectionsDialog.open()

            FileDialog {
                id: exportConnectionsDialog
                title: qsTr("Import Connections")
                nameFilters: ["RDM Connections (*.json)"]
                selectExisting: false
                onAccepted: connectionsManager.saveConnectionsConfigToFile(qmlUtils.getPathFromUrl(fileUrl))
            }
        }

        Rectangle { width: 1; color: "lightgrey"; Layout.fillHeight: true;}

        Item { Layout.fillWidth: true }

        Button {
            iconSource: "qrc:/images/settings.svg"
            text: qsTr("Settings")

            onClicked: {
                settingsDialog.open()
            }
        }
    }
}

