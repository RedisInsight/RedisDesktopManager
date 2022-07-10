import QtQuick 2.3
import QtQuick.Layouts 1.13
import QtQuick.Controls 2.13
import QtQuick.Controls 1.4 as LC
import QtQuick.Window 2.2
import QtCharts 2.3
import "./../common"
import "./../common/platformutils.js" as PlatformUtils
import "./../settings"

ServerAction {
    id: infoCharts

    GridLayout {
        id: infoChartsGrid
        columns: 3
        columnSpacing: 0
        rowSpacing: 0

        Layout.fillWidth: true
        Layout.fillHeight: true

        property int chartWidth: infoCharts.width / 3
        property int chartHeight: (infoCharts.height - 80) / 2

        function chartTheme() {
            if (sysPalette.base.hslLightness < 0.4) {
                return ChartView.ChartThemeDark
            } else {
                return ChartView.ChartThemeLight
            }
        }

        ChartView {
            id: chartCommandsPerSec

            Layout.preferredWidth: parent.chartWidth
            Layout.preferredHeight: parent.chartHeight

            legend.visible: false
            backgroundRoundness: 0

            theme: parent.chartTheme()

            backgroundColor: sysPalette.base

            title: qsTranslate("RESP","Commands Per Second")
            antialiasing: true

            DateTimeAxis {
                id: axisXCommandsPerSec
                min: new Date()
                format: "HH:mm:ss"
            }

            ValueAxis {
                id: axisYCommandsPerSec
                min: 0
                max: 100
                labelFormat: "%d"
                titleText: qsTranslate("RESP","Ops/s")
            }

            LineSeries {
                id: commands_per_sec_series
                name: "commands_per_sec"
                axisX: axisXCommandsPerSec
                axisY: axisYCommandsPerSec
            }
        }

        ChartView {
            id: chartConnectedClients

            Layout.preferredWidth: parent.chartWidth
            Layout.preferredHeight: parent.chartHeight

            legend.visible: false
            backgroundRoundness: 0

            theme: parent.chartTheme()

            backgroundColor: sysPalette.base

            title: qsTranslate("RESP","Connected Clients")
            antialiasing: true

            DateTimeAxis {
                id: axisXConnectedClients
                min: new Date()
                format: "HH:mm:ss"
            }

            ValueAxis {
                id: axisYConnectedClients
                min: 0
                max: 100
                labelFormat: "%d"
                titleText: qsTranslate("RESP","Clients")
            }

            SplineSeries {
                id: connected_clients_series
                name: "connected_clients"
                axisX: axisXConnectedClients
                axisY: axisYConnectedClients
            }
        }

        ChartView {
            id: chartMemoryUsage
            objectName: "rdm_server_info_tab_memory_usage"

            Layout.preferredWidth: parent.chartWidth
            Layout.preferredHeight: parent.chartHeight

            legend.visible: false
            backgroundRoundness: 0

            theme: parent.chartTheme()
            backgroundColor: sysPalette.base

            title: qsTranslate("RESP","Memory Usage")
            antialiasing: true

            DateTimeAxis {
                id: axisXMemoryUsage
                min: new Date()
                format: "HH:mm:ss"
            }

            ValueAxis {
                id: axisYMemoryUsage
                min: 0
                titleText: qsTranslate("RESP","Mb")
            }

            function toMsecsSinceEpoch(date) {
                var msecs = date.getTime();
                return msecs;
            }

            SplineSeries {
                id: used_memory_series
                name: "used_memory"
                axisX: axisXMemoryUsage
                axisY: axisYMemoryUsage
            }
        }

        ChartView {
            id: chartNetworkInput

            Layout.preferredWidth: parent.chartWidth
            Layout.preferredHeight: parent.chartHeight

            legend.visible: false
            backgroundRoundness: 0

            theme: parent.chartTheme()

            backgroundColor: sysPalette.base

            title: qsTranslate("RESP","Network Input")
            antialiasing: true

            DateTimeAxis {
                id: axisXNetworkInput
                min: new Date()
                format: "HH:mm:ss"
            }

            ValueAxis {
                id: axisYNetworkInput
                min: 0
                titleText: qsTranslate("RESP","Kb/s")
            }

            LineSeries {
                id: network_input_series
                name: "network_input"
                axisX: axisXNetworkInput
                axisY: axisYNetworkInput
            }
        }

        ChartView {
            id: chartNetworkOutput

            Layout.preferredWidth: parent.chartWidth
            Layout.preferredHeight: parent.chartHeight

            legend.visible: false
            backgroundRoundness: 0

            theme: parent.chartTheme()

            backgroundColor: sysPalette.base

            title: qsTranslate("RESP","Network Output")
            antialiasing: true

            DateTimeAxis {
                id: axisXNetworkOutput
                min: new Date()
                format: "HH:mm:ss"
            }

            ValueAxis {
                id: axisYNetworkOutput
                min: 0
                titleText: qsTranslate("RESP","Kb/s")
            }

            LineSeries {
                id: network_output_series
                name: "network_output"
                axisX: axisXNetworkOutput
                axisY: axisYNetworkOutput
            }
        }

        ChartView {
            id: chartTotalKeys

            Layout.preferredWidth: parent.chartWidth
            Layout.preferredHeight: parent.chartHeight

            legend.visible: false
            backgroundRoundness: 0

            theme: parent.chartTheme()

            backgroundColor: sysPalette.base

            title: qsTranslate("RESP","Total Keys")
            antialiasing: true

            DateTimeAxis {
                id: axisXTotalKeys
                min: new Date()
                format: "HH:mm:ss"
            }

            ValueAxis {
                id: axisYTotalKeys
                min: 0
                max: 100
                labelFormat: "%d"
                titleText: qsTranslate("RESP","Keys")
            }

            LineSeries {
                id: total_keys_series
                name: "total_keys"
                axisX: axisXTotalKeys
                axisY: axisYTotalKeys
            }
        }
    }

    Connections {
        target: infoCharts.model? infoCharts.model : null

        function onServerInfoChanged() {
            if (uiBlocked) {
                uiBlocked = false
            }

            var getUsedMemory = function (name) {
                return Math.round(parseFloat(infoCharts.model.serverInfo["memory"][name] ) / (1024 * 1024)  * 100) / 100;
            }

            // Commands per second
            var commandsPerSec = parseInt(serverTab.getValue("stats", "instantaneous_ops_per_sec"))
            var commandsPerSecMax = commandsPerSec + (10 - commandsPerSec % 10)
            if (commandsPerSecMax > axisYCommandsPerSec.max)
                axisYCommandsPerSec.max = commandsPerSecMax
            qmlUtils.addNewValueToDynamicChart(commands_per_sec_series, commandsPerSec)

            // Connected clients
            var connectedClients = parseInt(serverTab.getValue("clients", "connected_clients"))
            var connectedClientsMax = connectedClients + (10 - connectedClients % 10)
            if (connectedClientsMax > axisYConnectedClients.max)
                axisYConnectedClients.max = connectedClientsMax
            qmlUtils.addNewValueToDynamicChart(connected_clients_series, connectedClients)

            // Memory usage
            var usedMemory = getUsedMemory("used_memory")
            var memoryUsageMax = getUsedMemory("used_memory") + (10 - usedMemory % 10)
            if (memoryUsageMax > axisYMemoryUsage.max)
                axisYMemoryUsage.max = memoryUsageMax
            qmlUtils.addNewValueToDynamicChart(used_memory_series, usedMemory)

            // Network input
            var networkInput = parseFloat(serverTab.getValue("stats", "instantaneous_input_kbps"))
            var networkInputMax = networkInput + (10 - networkInput % 10)
            if (networkInputMax > axisYNetworkInput.max)
                axisYNetworkInput.max = networkInputMax
            qmlUtils.addNewValueToDynamicChart(network_input_series, networkInput)

            // Network output
            var networkOutput = parseFloat(serverTab.getValue("stats", "instantaneous_output_kbps"))
            var networkOutputMax = networkOutput + (10 - networkOutput % 10)
            if (networkOutputMax > axisYNetworkOutput.max)
                axisYNetworkOutput.max = networkOutputMax
            qmlUtils.addNewValueToDynamicChart(network_output_series, networkOutput)

            // Total keys
            var totalKeys = parseInt(serverTab.getTotalKeysValue())
            var totalKeysMax = totalKeys + (10 - totalKeys % 10)
            if (totalKeysMax > axisYTotalKeys.max)
                axisYTotalKeys.max = totalKeysMax
            qmlUtils.addNewValueToDynamicChart(total_keys_series, totalKeys)
        }
    }
}
