import QtQuick 2.15
import QtQuick.Layouts 1.1
import QtQuick.Controls 2.13
import QtQuick.Controls 1.4 as LC
import QtQuick.Dialogs 1.2
import Qt.labs.settings 1.0
import QtQuick.Window 2.3
import "./common"
import "./settings"
import "./common/platformutils.js" as PlatformUtils

Dialog {
    id: root
    title: qsTranslate("RDM","Settings")

    property bool restartRequired: false

    contentItem: Rectangle {
        id: dialogRoot
        implicitWidth: 950
        implicitHeight: PlatformUtils.isOSX() && qmlUtils.isAppStoreBuild()? 550 : 680

        color: sysPalette.base

        Control {
            palette: approot.palette
            anchors.fill: parent
            anchors.margins: 20

            ScrollView {
                id: globalSettingsScrollView
                width: parent.width
                height: parent.height
                ScrollBar.horizontal.policy: ScrollBar.AlwaysOff

                ColumnLayout {
                    id: innerLayout
                    width: PlatformUtils.isOSX()? globalSettingsScrollView.width - 25 : globalSettingsScrollView.width
                    height: (dialogRoot.height - 50 > implicitHeight) ? dialogRoot.height - 50 : implicitHeight
                    spacing: 10

                    RowLayout {
                        Layout.fillWidth: true

                        SettingsGroupTitle {
                            Layout.fillWidth: true
                            text: qsTranslate("RDM","General")
                        }

                        BetterLabel {
                            color: disabledSysPalette.text
                            text: qsTranslate("RDM","Application will be restarted to apply this settings.")
                        }
                    }

                    GridLayout {
                        columns: 2
                        Layout.fillWidth: true
                        rowSpacing: 10
                        columnSpacing: 15

                        ComboboxOption {
                            id: appLang

                            Layout.fillWidth: true
                            Layout.preferredHeight: 30

                            model: ["system", "en_US", "zh_CN", "zh_TW", "ru_RU", "es_ES", "ja_JP"]
                            value: "system"
                            label: qsTranslate("RDM","Language")
                            onValueChanged: root.restartRequired = true
                        }

                        BoolOption {
                            id: systemProxy

                            Layout.fillWidth: true
                            Layout.preferredHeight: 30

                            value: false
                            label: qsTranslate("RDM","Use system proxy settings")

                            onValueChanged: root.restartRequired = true
                        }

                        ComboboxOption {
                            id: appFont

                            Layout.fillWidth: true
                            Layout.preferredHeight: 30

                            model: Qt.fontFamilies()
                            label: qsTranslate("RDM","Font")

                            onValueChanged: root.restartRequired = true
                        }

                        BoolOption {
                            id: disableProxyForRedisConnections

                            Layout.fillWidth: true
                            Layout.preferredHeight: 30

                            value: false
                            label: qsTranslate("RDM","Use system proxy only for HTTP(S) requests")
                        }

                        ComboboxOption {
                            id: appFontSize

                            Layout.fillWidth: true
                            Layout.preferredHeight: 30

                            model: ["8", "9", "10", "11", "12", "13", "14", "15", "16"]
                            label: qsTranslate("RDM","Font Size")

                            onValueChanged: root.restartRequired = true
                        }
                    }

                    SettingsGroupTitle {
                        Layout.topMargin: 10
                        text: qsTranslate("RDM","Value Editor")
                    }

                    GridLayout {
                        columns: 2
                        rowSpacing: 10
                        columnSpacing: 15

                        ComboboxOption {
                            id: valueEditorFont

                            Layout.fillWidth: true
                            Layout.preferredHeight: 30

                            model: Qt.fontFamilies()
                            label: qsTranslate("RDM","Font")

                            onValueChanged: root.restartRequired = true
                        }

                        ComboboxOption {
                            id: valueEditorFontSize

                            Layout.fillWidth: true
                            Layout.preferredHeight: 30

                            model: ["8", "9", "10", "11", "12", "13", "14", "15", "16"]
                            value: Qt.platform.os == "osx"? "12" : "11"
                            label: qsTranslate("RDM","Font Size")

                            onValueChanged: root.restartRequired = true
                        }

                        IntOption {
                            id: valueSizeLimit

                            Layout.fillWidth: true
                            Layout.preferredHeight: 30

                            min: 1
                            max: 2000000
                            value: 150000
                            label: qsTranslate("RDM","Maximum Formatted Value Size")
                            description: qsTranslate("RDM", "Size in bytes")
                        }

                    }

                    SettingsGroupTitle {
                        text: qsTranslate("RDM","Connections Tree")
                        Layout.topMargin: 20
                    }

                    GridLayout {
                        columns: 2
                        rowSpacing: 30
                        columnSpacing: 20

                        BoolOption {
                            id: nsReload

                            Layout.fillWidth: true
                            Layout.preferredHeight: 30

                            value: true
                            label: qsTranslate("RDM","Reopen namespaces on reload")
                            description: qsTranslate("RDM","(Disable to improve treeview performance)")
                        }

                        BoolOption {
                            id: namespacedKeysShortName

                            Layout.fillWidth: true
                            Layout.preferredHeight: 30

                            value: true
                            label: qsTranslate("RDM","Show only last part for namespaced keys")
                        }

                        IntOption {
                            id: liveKeyLimit

                            Layout.fillWidth: true
                            Layout.preferredHeight: 30

                            min: 100
                            max: 100000
                            value: 1000
                            label: qsTranslate("RDM","Live update maximum allowed keys")                            
                        }

                        IntOption {
                            id: liveUpdateInterval

                            Layout.fillWidth: true
                            Layout.preferredHeight: 30

                            min: 3
                            max: 100000
                            value: 10
                            label: qsTranslate("RDM","Live update interval (in seconds)")                            
                        }

                    }

                    SettingsGroupTitle {
                        Layout.topMargin: 10
                        visible: !(PlatformUtils.isOSX() && qmlUtils.isAppStoreBuild())
                        text: qsTranslate("RDM","External Value View Formatters")
                    }

                    Text {
                        visible: !(PlatformUtils.isOSX() && qmlUtils.isAppStoreBuild())
                        text: formattersManager? qsTranslate("RDM","Formatters path: %0").arg(formattersManager.formattersPath()) : ""
                        font.pixelSize: 12
                        color: "grey"
                    }

                    LC.TableView {
                        id: formattersTable
                        visible: !(PlatformUtils.isOSX() && qmlUtils.isAppStoreBuild())

                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        Layout.preferredHeight: 100
                        verticalScrollBarPolicy: Qt.ScrollBarAlwaysOn

                        LC.TableViewColumn {
                            role: "name"
                            title: qsTranslate("RDM","Name")
                        }
                        LC.TableViewColumn {
                            role: "version"
                            width: 75
                            title: qsTranslate("RDM","Version")
                        }
                        LC.TableViewColumn {
                            role: "cmd"
                            title: qsTranslate("RDM","Command")
                        }

                        LC.TableViewColumn {
                            width: 250
                            role: "description"
                            title: qsTranslate("RDM","Description")
                        }

                        model: formattersManager
                    }

                    Item {
                        visible: !formattersTable.visible
                        Layout.fillHeight: true
                    }

                    RowLayout {
                        Layout.fillWidth: true

                        Item { Layout.fillWidth: true; }
                        BetterButton {
                            text: qsTranslate("RDM","OK")
                            onClicked: {
                                if (root.restartRequired === true) {
                                    // restart app
                                    Qt.exit(1001)
                                }

                                restartRequired = false
                                root.close()
                            }
                        }
                        BetterButton {
                            text: qsTr("Cancel")
                            onClicked: root.close()
                        }
                    }
                }
            }
        }
    }

    Settings {
        id: globalSettings
        category: "app"

        property alias reopenNamespacesOnReload: nsReload.value        
        property alias namespacedKeysShortName: namespacedKeysShortName.value
        property alias liveUpdateKeysLimit: liveKeyLimit.value
        property alias liveUpdateInterval: liveUpdateInterval.value
        property alias appFont: appFont.value
        property alias appFontSize: appFontSize.value
        property alias valueEditorFont: valueEditorFont.value
        property alias valueEditorFontSize: valueEditorFontSize.value
        property alias valueSizeLimit: valueSizeLimit.value
        property alias locale: appLang.value
        property alias useSystemProxy: systemProxy.value
        property alias disableProxyForRedisConnections: disableProxyForRedisConnections.value
    }

    Settings {
        id: customFormatters
        category: "formatters"

        property var formatters
    }

    Component.onCompleted: {
        restartRequired = false
    }
}
