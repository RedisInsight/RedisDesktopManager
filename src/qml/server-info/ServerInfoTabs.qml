import QtQuick 2.3
import QtQuick.Layouts 1.1
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.1
import QtQuick.Dialogs 1.2
import QtQuick.Window 2.2
import QtCharts 2.0
import "./../common"


Repeater {
    id: root

    BetterTab {
        id: tab
        title: tabName
        icon: "qrc:/images/console.svg"
        tabType: "server_info"

        property var model

        onClose: {
            serverStatsModel.closeTab(tabIndex)
        }

        Rectangle {
            anchors.fill: parent

            color: "white"

            ColumnLayout {

                anchors.fill: parent
                anchors.margins: 20

                GridLayout {
                    Layout.fillWidth: true

                    rows: 2
                    flow: GridLayout.TopToBottom

                    Text {
                        Layout.preferredWidth: tab.width / 5

                        text: qsTr("Redis Version")
                        font.pointSize: 15
                        color: "grey"
                    }

                    Label { id: redisVersionLabel; text: "N/A"; font.pointSize: 18  }

                    Text {
                        Layout.preferredWidth: tab.width / 5

                        text: qsTr("Used memory")
                        font.pointSize: 15
                        color: "grey"
                    }

                    Label { id: usedMemoryLabel; text: "N/A"; font.pointSize: 18 }

                    Text {
                        Layout.preferredWidth: tab.width / 5

                        text: qsTr("Clients")
                        font.pointSize: 15
                        color: "grey"
                    }

                    Label { id: connectedClientsLabel; text: "N/A"; font.pointSize: 18 }

                    Text {
                        Layout.preferredWidth: tab.width / 5

                        text: qsTr("Commands Processed")
                        font.pointSize: 15
                        color: "grey"
                        wrapMode: Text.WordWrap
                    }

                    Label { id: totalCommandsProcessedLabel; text: "N/A"; font.pointSize: 18 }


                    Text {
                        Layout.preferredWidth: tab.width / 5

                        text: qsTr("Uptime")
                        font.pointSize: 15
                        color: "grey"
                    }

                    Label { id: uptimeLabel; text: "N/A"; font.pointSize: 18 }


                    Connections {
                        target: tab.model? tab.model : null

                        onServerInfoChanged: {
                            usedMemoryLabel.text = tab.model.serverInfo["memory"]["used_memory_human"]
                            redisVersionLabel.text = tab.model.serverInfo["server"]["redis_version"]
                            connectedClientsLabel.text = tab.model.serverInfo["clients"]["connected_clients"]
                            totalCommandsProcessedLabel.text = tab.model.serverInfo["stats"]["total_commands_processed"]
                            uptimeLabel.text = tab.model.serverInfo["server"]["uptime_in_days"] + " days"
                        }
                    }
                }

                RowLayout {
                    Layout.fillHeight: true
                    Layout.fillWidth: true

                    ChartView {
                        id: view

                        Layout.fillHeight: true
                        Layout.preferredWidth: tab.width * 0.5

                        title: qsTr("Memory Usage")
                        antialiasing: true

                        DateTimeAxis {
                            id: axisX
                            min: new Date()
                            format: "HH:mm:ss"
                        }

                        ValueAxis {
                            id: axisY
                            min: 0
                            titleText: qsTr("Mb")
                        }

                        function toMsecsSinceEpoch(date) {
                            var msecs = date.getTime();
                            return msecs;
                        }

                        SplineSeries {
                            id: used_memory_series
                            name: "used_memory"
                            axisX: axisX
                            axisY: axisY
                        }

                        SplineSeries {
                            id: used_memory_rss_series
                            name: "used_memory_rss"
                            axisX: axisX
                            axisY: axisY
                        }

                        SplineSeries {
                            id: used_memory_lua_series
                            name: "used_memory_lua"
                            axisX: axisX
                            axisY: axisY
                        }

                        SplineSeries {
                            id: used_memory_peak_series
                            name: "used_memory_peak"
                            axisX: axisX
                            axisY: axisY
                        }
                    }

                    TabView {
                        Layout.fillHeight: true
                        Layout.minimumWidth: tab.width * 0.4

                        Tab {
                            title: qsTr("Server Info")

                            TableView {
                                id: serverInfoListView

                                Layout.fillHeight: true
                                Layout.minimumWidth: 400

                                model: tab.model && tab.model.serverInfo ? tab.model.serverInfo["server"] : null

                                TableViewColumn {
                                    role: "name"
                                    title: qsTr("Property")
                                    width: 200
                                }

                                TableViewColumn {
                                    role: "value"
                                    title: qsTr("Value")
                                    width: 200
                                }

                                Connections {
                                    target: tab.model? tab.model : null

                                    onServerInfoChanged: {
                                        // fill tab with info
                                        var serverInfo = []

                                        for (var key in tab.model.serverInfo["server"])
                                        {
                                            var property = {"name": key, "value": tab.model.serverInfo["server"][key]}
                                            serverInfo.push(property)
                                        }

                                        serverInfoListView.model = serverInfo
                                    }
                                }
                            }
                        }
// TODO: Add UI for slowlog (view/reset) and clients (view/kill)
//                        Tab {
//                            title: "Slowlog"
//                        }

//                        Tab {
//                            title: "Clients"
//                        }
                    }

                    Connections {
                        target: tab.model? tab.model : null

                        onServerInfoChanged: {
                            var getValue = function (name) {
                                return parseFloat(tab.model.serverInfo["memory"][name] ) / (1024 * 1024)
                            }

                            qmlUtils.addNewValueToDynamicChart(used_memory_series, getValue("used_memory"))
                            qmlUtils.addNewValueToDynamicChart(used_memory_rss_series, getValue("used_memory_rss"))
                            qmlUtils.addNewValueToDynamicChart(used_memory_lua_series, getValue("used_memory_lua"))
                            qmlUtils.addNewValueToDynamicChart(used_memory_peak_series, getValue("used_memory_peak"))

                            axisY.max = getValue("used_memory_peak") + 100
                        }
                    }
                }
            }


            Timer {
                id: initTimer

                onTriggered: {
                    tab.model = serverStatsModel.getValue(tabIndex)
                    tab.model.init()
                }
            }

            Component.onCompleted: {                
                initTimer.start()
                uiBlocker.visible = true
            }

            Connections {
                target: tab.model ? tab.model : null

                onInitialized: {
                    uiBlocker.visible = false
                    tab.icon = "qrc:/images/console.svg"
                }
            }

            Rectangle {
                id: uiBlocker
                visible: false
                anchors.fill: parent
                color: Qt.rgba(0, 0, 0, 0.1)

                Item {
                    anchors.fill: parent
                    BusyIndicator { anchors.centerIn: parent; running: true }
                }

                MouseArea {
                    anchors.fill: parent
                }
            }
        }
    }
}
