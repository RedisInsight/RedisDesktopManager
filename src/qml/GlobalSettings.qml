import QtQuick 2.3
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
        implicitWidth: 800
        implicitHeight: PlatformUtils.isOSX()? 500 : 620

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

                    SettingsGroupTitle {
                        text: qsTranslate("RDM","General")
                    }

                    ComboboxOption {
                        id: appLang

                        Layout.fillWidth: true
                        Layout.preferredHeight: 30

                        model: ["system", "en_US", "zh_CN", "zh_TW", "ru_RU", "es_ES", "ja_JP"]
                        value: "system"
                        label: qsTranslate("RDM","Language")
                        description: qsTranslate("RDM","Application will be restarted to apply this setting.")

                        onValueChanged: root.restartRequired = true
                    }

                    ComboboxOption {
                        id: appFont

                        Layout.fillWidth: true
                        Layout.preferredHeight: 30

                        value: Qt.platform.os == "osx"? "Helvetica Neue" : "Open Sans"
                        model: Qt.fontFamilies()
                        label: qsTranslate("RDM","Font")
                        description: qsTranslate("RDM","Application will be restarted to apply this setting.")

                        onValueChanged: root.restartRequired = true
                    }

                    ComboboxOption {
                        id: appFontSize

                        Layout.fillWidth: true
                        Layout.preferredHeight: 30

                        model: ["8", "9", "10", "11", "12", "13", "14", "15", "16"]
                        value: Qt.platform.os == "osx"? "12" : "11"
                        label: qsTranslate("RDM","Font Size")
                        description: qsTranslate("RDM","Application will be restarted to apply this setting.")

                        onValueChanged: root.restartRequired = true
                    }

                    ComboboxOption {
                        id: valueEditorFontSize

                        Layout.fillWidth: true
                        Layout.preferredHeight: 30

                        model: ["8", "9", "10", "11", "12", "13", "14", "15", "16"]
                        value: Qt.platform.os == "osx"? "12" : "11"
                        label: qsTranslate("RDM","Value Editor Font Size")
                        description: qsTranslate("RDM","Application will be restarted to apply this setting.")

                        onValueChanged: root.restartRequired = true
                    }

                    BoolOption {
                        id: systemProxy

                        Layout.fillWidth: true
                        Layout.preferredHeight: 30

                        value: false
                        label: qsTranslate("RDM","Use system proxy settings")
                        description: qsTranslate("RDM","Application will be restarted to apply this setting.")

                        onValueChanged: root.restartRequired = true
                    }

                    SettingsGroupTitle {
                        text: qsTranslate("RDM","Connections Tree")
                        Layout.topMargin: 20
                    }

                    BoolOption {
                        id: nsReload

                        Layout.fillWidth: true
                        Layout.preferredHeight: 30

                        value: true
                        label: qsTranslate("RDM","Reopen namespaces on reload")
                        description: qsTranslate("RDM","(Disable to improve treeview performance)")
                    }

                    BoolOption {
                        id: keySorting

                        Layout.fillWidth: true
                        Layout.preferredHeight: 30

                        value: true
                        label: qsTranslate("RDM","Enable key sorting in tree")
                        description: qsTranslate("RDM","(Disable to improve treeview performance)")
                    }

                    IntOption {
                        id: liveKeyLimit

                        Layout.fillWidth: true
                        Layout.preferredHeight: 30

                        min: 100
                        max: 100000
                        value: 1000
                        label: qsTranslate("RDM","Live update maximum allowed keys")
                        description: ""
                    }

                    IntOption {
                        id: liveUpdateInterval

                        Layout.fillWidth: true
                        Layout.preferredHeight: 30

                        min: 3
                        max: 100000
                        value: 10
                        label: qsTranslate("RDM","Live update interval (in seconds)")
                        description: ""
                    }

                    SettingsGroupTitle {
                        visible: !PlatformUtils.isOSX()
                        text: qsTranslate("RDM","External Value View Formatters")
                    }

                    Text {
                        visible: !PlatformUtils.isOSX()
                        text: formattersManager? qsTranslate("RDM","Formatters path: %0").arg(formattersManager.formattersPath()) : ""
                        font.pixelSize: 12
                        color: "grey"
                    }

                    LC.TableView {
                        visible: !PlatformUtils.isOSX()

                        Layout.fillWidth: true
                        Layout.fillHeight: true
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
        property alias enableKeySortingInTree: keySorting.value
        property alias liveUpdateKeysLimit: liveKeyLimit.value
        property alias liveUpdateInterval: liveUpdateInterval.value
        property alias appFont: appFont.value
        property alias appFontSize: appFontSize.value
        property alias valueEditorFontSize: valueEditorFontSize.value
        property alias locale: appLang.value
        property alias useSystemProxy: systemProxy.value
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
