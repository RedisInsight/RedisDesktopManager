import QtQuick 2.3
import QtQuick.Layouts 1.1
import QtQuick.Controls 1.2
import QtQuick.Dialogs 1.2
import Qt.labs.settings 1.0
import QtQuick.Window 2.3
import "./common"
import "./settings"
import "./common/platformutils.js" as PlatformUtils

Dialog {
    id: root
    title: qsTr("Settings")

    contentItem: Item {
        implicitWidth: 800
        implicitHeight: PlatformUtils.isOSX()? 680 : Math.min(750, Screen.desktopAvailableHeight - 100)

        ColumnLayout {
            anchors.fill: parent
            anchors.margins: 20

            SettingsGroupTitle {
                text: qsTr("General")                
            }

            ComboboxOption {
                id: appLang

                Layout.fillWidth: true
                Layout.preferredHeight: 40

                model: ["system", "en_US", "zh_CN", "zh_TW", "ru_RU"]
                value: "system"
                label: qsTr("Language")
                description: qsTr("Application restart is needed to apply this setting.")
            }

            ComboboxOption {
                id: appFont

                Layout.fillWidth: true
                Layout.preferredHeight: 40

                value: Qt.platform.os == "osx"? "Helvetica Neue" : "Open Sans"
                model: Qt.fontFamilies()
                label: qsTr("Font")
                description: qsTr("Application restart is needed to apply this setting.")
            }

            ComboboxOption {
                id: appFontSize

                Layout.fillWidth: true
                Layout.preferredHeight: 40

                model: ["8", "9", "10", "11", "12"]
                value: Qt.platform.os == "osx"? "12" : "11"
                label: qsTr("Font Size")
                description: qsTr("Application restart is needed to apply this setting.")
            }

            BoolOption {
                id: systemProxy

                Layout.fillWidth: true
                Layout.preferredHeight: 40

                value: false
                label: qsTr("Use system proxy settings")
                description: qsTr("Application restart is needed to apply this setting.")
            }

            SettingsGroupTitle {
                text: qsTr("Connections Tree")
                Layout.topMargin: 20
            }

            BoolOption {
                id: nsReload

                Layout.fillWidth: true
                Layout.preferredHeight: 40

                value: true
                label: qsTr("Reopen namespaces on reload")
                description: qsTr("(Disable to improve treeview performance)")
            }

            BoolOption {
                id: keySorting

                Layout.fillWidth: true
                Layout.preferredHeight: 40

                value: true
                label: qsTr("Enable key sorting in tree")
                description: qsTr("(Disable to improve treeview performance)")
            }

            IntOption {
                id: liveKeyLimit

                Layout.fillWidth: true
                Layout.preferredHeight: 40

                min: 100
                max: 100000
                value: 1000
                label: qsTr("Live update maximum allowed keys")
                description: ""
            }

            IntOption {
                id: liveUpdateInterval

                Layout.fillWidth: true
                Layout.preferredHeight: 40

                min: 3
                max: 100000
                value: 10
                label: qsTr("Live update interval (in seconds)")
                description: ""
            }               

            SettingsGroupTitle {
                text: qsTr("Custom Value View Formatters")                
            }

            Text {
                text: qsTr("Formatters path: %0").arg(formattersManager.formattersPath())
                font.pixelSize: 12
                color: "grey"
            }

            TableView {
                Layout.fillWidth: true                
                verticalScrollBarPolicy: Qt.ScrollBarAlwaysOn

                TableViewColumn {
                    role: "name"
                    title: "Name"
                }
                TableViewColumn {
                    role: "version"
                    title: "Version"
                }
                TableViewColumn {
                    role: "cmd"
                    title: "Command"
                }

                model: formattersManager
            }

            Item {
                Layout.fillHeight: true
            }

            RowLayout {
                Layout.fillWidth: true

                Item { Layout.fillWidth: true; }
                Button {
                    text: qsTr("OK")
                    onClicked: root.close()
                }
            }
        }
    }

    Settings {
        id: globalSettings
        category: "app"

        property alias reopenNamespacesOnReload: nsReload.value
        property alias enableKeySortingInTree: keySorting.value
        property alias liveUpdateKeysLimit: liveKeyLimit.value
        property alias liveUpdateInterval: liveUpdateInterval.value
        property alias appFont: appFont.value
        property alias appFontSize: appFontSize.value
        property alias locale: appLang.value
        property alias useSystemProxy: systemProxy.value
    }

    Settings {
        id: customFormatters
        category: "formatters"

        property var formatters
    }
}
