import QtQuick 2.3
import QtQuick.Layouts 1.13
import QtQuick.Controls 2.13
import QtQuick.Controls 1.4 as LC
import QtQuick.Window 2.2
import QtCharts 2.3
import "./../common"
import "./../common/platformutils.js" as PlatformUtils
import "./../settings"


Repeater {
    id: root

    BetterTab {
        id: serverTab

        Component {
            id: serverTabButton

            BetterTabButton {
                icon.source: PlatformUtils.getThemeIcon("database.svg")

                text: tabName

                onCloseClicked: {
                    serverStatsModel.closeTab(tabIndex)
                }
            }
        }

        Component.onCompleted: {
            var tabButton = serverTabButton.createObject(serverTab);
            tabButton.self = tabButton;
            tabButton.tabRef = serverTab;
            tabBar.addItem(tabButton)
            tabBar.activateTabButton(tabButton)
            tabs.activateTab(serverTab)
        }

        property var model: tabModel

        onModelChanged: {
            if (!model)
                return;

            serverTab.model.init()
        }


        function getValue(cat, prop) {
            try {
                return model.serverInfo[cat][prop]
            } catch(e) {
                console.error("Cannot get server info '" + prop + "' from " + cat)
                return ""
            }
        }

        function getIntValue(cat, prop) {
            var val = getValue(cat, prop)
            if (val !== "") return parseInt(val)
            return 0
        }

        function getHitRatio() {
            var hits = getIntValue("stats", "keyspace_hits")
            var misses = getIntValue("stats", "keyspace_misses")
            var total = hits + misses
            if (total === 0) {
                return 0
            }
            return Math.round(hits / total * 100 * 100) / 100
        }

        Rectangle {
            id: wrappingBackground
            anchors.fill: parent

            color: sysPalette.base

            ColumnLayout {                
                clip: true
                anchors.fill: parent
                anchors.margins: 15

                Component {
                    id: actionsMenu

                    ColumnLayout {

                        GridLayout {
                            id: tileGrid
                            columns: 4

                            Layout.fillWidth: true

                            property int tileSize: PlatformUtils.isScalingDisabled()? 150 : 110
                            property int tileIconSize: PlatformUtils.isScalingDisabled()? 90 : 75

                            ImageButton {
                                objectName: "rdm_server_action_info"

                                Layout.fillWidth: true
                                Layout.rowSpan: 2
                                implicitHeight: tileGrid.tileSize * 2

                                tooltip: qsTranslate("RESP", "View Server Info")

                                showBorder: true
                                imgStickTop: true

                                imgWidth: tileGrid.tileIconSize
                                imgHeight: tileGrid.tileIconSize
                                iconSource: PlatformUtils.getThemeIcon("server-config.svg")
                                onClicked: {
                                    currentAction.text = tooltip
                                    serverStackView.push(serverConfig)
                                }

                                GridLayout {
                                     anchors.bottom: parent.bottom
                                     anchors.horizontalCenter: parent.horizontalCenter
                                     anchors.margins: 15

                                     columns: 2
                                     flow: GridLayout.LeftToRight

                                     Text {
                                         text: qsTranslate("RESP","Redis Version")
                                         font.pointSize: 12
                                         color: sysPalette.windowText
                                     }

                                     BetterLabel {
                                         id: redisVersionLabel
                                         text: "N/A"
                                         font.pointSize: 12
                                         objectName: "rdm_server_info_redis_version"
                                     }

                                     Text {
                                         text: qsTranslate("RESP","Uptime")
                                         font.pointSize: 12
                                         color: sysPalette.windowText
                                     }

                                     BetterLabel {
                                         id: uptimeLabel;
                                         text: "N/A";
                                         font.pointSize: 12
                                         objectName: "rdm_server_info_uptime"
                                     }

                                     Text {
                                         text: qsTranslate("RESP","Hit Ratio")
                                         font.pointSize: 12
                                         color: sysPalette.windowText
                                     }

                                     BetterLabel {
                                         id: hitRatioLabel;
                                         text: "N/A";
                                         font.pointSize: 12
                                         objectName: "rdm_server_info_hit_ratio"
                                     }

                                     Text {
                                         text: qsTranslate("RESP","Used memory")
                                         font.pointSize: 12
                                         color: sysPalette.windowText
                                     }

                                     BetterLabel {
                                         id: usedMemoryLabel;
                                         text: "N/A";
                                         font.pointSize: 12
                                         objectName: "rdm_server_info_used_memory"
                                     }

                                     Text {
                                         text: qsTranslate("RESP","Cmd Processed")
                                         font.pointSize: 12
                                         color: sysPalette.windowText
                                         wrapMode: Text.WordWrap
                                     }

                                     BetterLabel {
                                         id: totalCommandsProcessedLabel;
                                         text: "N/A";
                                         font.pointSize: 12
                                         objectName: "rdm_server_info_cmd_processed"
                                     }
                                }
                            }

                            ImageButton {
                                objectName: "rdm_server_action_monitor"

                                Layout.fillWidth: true
                                implicitHeight: tileGrid.tileSize

                                text: qsTranslate("RESP", "Monitor Commands")

                                showBorder: true

                                imgWidth: tileGrid.tileIconSize
                                imgHeight: tileGrid.tileIconSize
                                iconSource: PlatformUtils.getThemeIcon("console.svg")
                                onClicked: {
                                    serverTab.model.monitorCommands()
                                }
                            }

                            ImageButton {
                                objectName: "rdm_server_action_slowlog"

                                Layout.fillWidth: true
                                implicitHeight: tileGrid.tileSize

                                text: qsTranslate("RESP", "Slowlog")

                                showBorder: true

                                imgWidth: tileGrid.tileIconSize
                                imgHeight: tileGrid.tileIconSize
                                iconSource: PlatformUtils.getThemeIcon("slowlog.svg")
                                onClicked: {
                                    currentAction.text = text
                                    serverTab.model.refreshSlowLog = true
                                    serverStackView.push(serverSlowlog)
                                }
                            }

                            ImageButton {
                                id: connectedClientsBtn
                                objectName: "rdm_server_action_clients"

                                Layout.fillWidth: true
                                implicitHeight: tileGrid.tileSize

                                text: qsTranslate("RESP", "Clients")

                                showBorder: true

                                imgWidth: tileGrid.tileIconSize
                                imgHeight: tileGrid.tileIconSize
                                iconSource: PlatformUtils.getThemeIcon("clients.svg")
                                onClicked: {
                                    currentAction.text = text
                                    serverTab.model.refreshClients = true
                                    serverStackView.push(serverClients)
                                }
                            }

                            ImageButton {
                                objectName: "rdm_server_action_charts"

                                Layout.fillWidth: true
                                implicitHeight: tileGrid.tileSize

                                text: qsTranslate("RESP", "Server Stats")

                                showBorder: true

                                imgWidth: tileGrid.tileIconSize
                                imgHeight: tileGrid.tileIconSize
                                iconSource: PlatformUtils.getThemeIcon("server-stats.svg")
                                onClicked: {
                                    currentAction.text = text
                                    serverStackView.push(serverCharts)
                                }
                            }

                            ImageButton {
                                objectName: "rdm_server_action_console"

                                Layout.fillWidth: true
                                implicitHeight: tileGrid.tileSize

                                text: qsTranslate("RESP", "Console")

                                showBorder: true

                                imgWidth: tileGrid.tileIconSize
                                imgHeight: tileGrid.tileIconSize
                                iconSource: PlatformUtils.getThemeIcon("console.svg")
                                onClicked: {
                                    serverTab.model.openTerminal()
                                }
                            }

                            ImageButton {
                                objectName: "rdm_server_action_pubsub"

                                Layout.fillWidth: true
                                implicitHeight: tileGrid.tileSize

                                text: qsTranslate("RESP", "Pub/Sub Channels")

                                showBorder: true

                                imgWidth: tileGrid.tileIconSize
                                imgHeight: tileGrid.tileIconSize
                                iconSource: PlatformUtils.getThemeIcon("pub-sub-channels.svg")
                                onClicked: {
                                    currentAction.text = text
                                    serverTab.model.refreshPubSubMonitor = true
                                    serverStackView.push(serverPubSub)
                                }
                            }

                            Connections {
                                target: model? model : null

                                function onServerInfoChanged() {
                                    usedMemoryLabel.text = serverTab.getValue("memory", "used_memory_human")
                                    redisVersionLabel.text = serverTab.getValue("server", "redis_version")
                                    connectedClientsBtn.text = qsTranslate("RESP", "Clients") + " " + serverTab.getValue("clients", "connected_clients")
                                    totalCommandsProcessedLabel.text = serverTab.getValue("stats", "total_commands_processed")
                                    uptimeLabel.text = serverTab.getValue("server", "uptime_in_days") + qsTranslate("RESP"," day(s)")
                                    hitRatioLabel.text = serverTab.getHitRatio() + "%"
                                }
                            }
                        }                        

                        Item {
                            Layout.fillHeight: true
                        }
                    }
                }

                Component {
                    id: serverCharts

                    ServerCharts {
                        model: serverTab.model
                    }
                }

                Component {
                    id: serverClients

                    ServerClients {
                        model: serverTab.model
                    }
                }

                Component {
                    id: serverConfig

                    ServerConfig {
                        model: serverTab.model
                    }
                }

                Component {
                    id: serverPubSub

                    ServerPubSub {
                        model: serverTab.model
                    }
                }

                Component {
                    id: serverSlowlog

                    ServerSlowlog {
                        model: serverTab.model
                    }
                }                

                RowLayout {
                    visible: serverStackView.depth > 1

                    BetterButton {
                        text: qsTr("Server Actions")

                        onClicked: {
                            serverStackView.currentItem.stopTimer()
                            serverStackView.pop()
                        }
                    }

                    BetterLabel { text: "❯" }

                    BetterLabel {id: currentAction}

                }

                StackView {
                    id: serverStackView
                    Layout.fillHeight: true
                    Layout.fillWidth: true

                    initialItem: actionsMenu
                }
            }
        }
    }
}
