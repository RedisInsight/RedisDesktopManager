import QtQuick 2.15
import QtQuick.Layouts 1.1
import QtQuick.Controls 2.13
import QtQuick.Controls 1.4 as LC
import Qt.labs.settings 1.0
import QtQuick.Window 2.3
import "../common"
import "../settings"
import "../common/platformutils.js" as PlatformUtils

BetterDialog {
    id: root
    title: qsTranslate("RESP","Extension Server")

    footer: null

    property bool restartRequired: false

    contentItem: Rectangle {
        id: dialogRoot
        implicitWidth: 950
        implicitHeight: 550

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
                    width: globalSettingsScrollView.width - 25
                    height: (dialogRoot.height - 50 > implicitHeight) ? dialogRoot.height - 50 : implicitHeight
                    spacing: 10

                    RowLayout {
                        Layout.fillWidth: true

                        SettingsGroupTitle {
                            Layout.fillWidth: true
                            text: qsTranslate("RESP","Connection Settings")
                        }
                    }

                    ColumnLayout {
                        Layout.fillWidth: true                        
                        spacing: 10


                        RowLayout {

                            BetterLabel {
                                Layout.preferredWidth: 200
                                text: qsTranslate("RESP","Server Url:")
                            }

                            BetterTextField {
                                id: serverUrl

                                Layout.fillWidth: true
                            }
                        }

                        RowLayout {

                            BetterLabel {
                                Layout.preferredWidth: 200
                                text: qsTranslate("RESP","Basic Auth:")
                            }

                            BetterTextField {
                                id: serverAuthTokenName
                                Layout.fillWidth: true
                                placeholderText: qsTranslate("RESP","User")
                            }

                            PasswordInput {
                                id: serverAuthTokenValue
                                Layout.fillWidth: true
                                placeholderText: qsTranslate("RESP","Password")
                            }
                        }
                        IntOption {
                            id: responseTimeout

                            Layout.preferredHeight: 30
                            Layout.fillWidth: true

                            min: 1
                            max: 60
                            value: 10
                            label: qsTranslate("RESP","Response timeout  (in seconds)")
                        }


                    }                                       

                    RowLayout {
                        Layout.topMargin: 10

                        SettingsGroupTitle {                            
                            text: qsTranslate("RESP", "Available Data Formatters")
                        }

                        Item {
                            Layout.fillWidth: true
                        }

                        BetterButton {
                            text: qsTranslate("RESP", "Reload")
                            onClicked: {
                                formattersManager.loadFormatters();
                            }
                        }
                    }

                    LC.TableView {
                        id: formattersTable

                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        Layout.preferredHeight: 100
                        verticalScrollBarPolicy: Qt.ScrollBarAlwaysOn

                        LC.TableViewColumn {
                            role: "id"
                            width: 75
                            title: qsTranslate("RESP","Id")
                        }

                        LC.TableViewColumn {
                            width: 250
                            role: "name"
                            title: qsTranslate("RESP","Name")
                        }

                        LC.TableViewColumn {
                            width: 75
                            role: "readOnly"
                            title: qsTranslate("RESP","Read Only")
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
                            text: qsTranslate("RESP","OK")
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
                            text: qsTranslate("RESP","Cancel")
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

        property alias extensionServerUrl: serverUrl.text
        property alias extensionServerUser: serverAuthTokenName.text
        property alias extensionServerPassword: serverAuthTokenValue.text
        property alias extensionServerRequestTimeout: responseTimeout.value

    }

    Component.onCompleted: {
        restartRequired = false
    }
}
