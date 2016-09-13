import QtQuick 2.3
import QtQuick.Layouts 1.1
import QtQuick.Controls 1.2
import QtQuick.Dialogs 1.2
import Qt.labs.settings 1.0
import "./common"
import "./settings"

Dialog {
    id: root
    title: "Settings"

    contentItem: Item {
        implicitWidth: 800
        implicitHeight: 600

        ColumnLayout {
            anchors.fill: parent
            anchors.margins: 20

            Text {
                text: "Appearance"
                font.pixelSize: 20
            }

            ComboboxOption {
                id: appFont

                Layout.fillWidth: true
                Layout.preferredHeight: 40

                value: "Open Sans"
                model: Qt.fontFamilies()
                label: "Font"
                description: ""
            }

            ComboboxOption {
                id: appFontSize

                Layout.fillWidth: true
                Layout.preferredHeight: 40

                model: ["8", "9", "10", "11", "12"]
                value: "10"
                label: "Font Size"
                description: "in pixels"
            }

            Text {
                text: "Connections Tree"
                font.pixelSize: 20
            }

            BoolOption {
                id: nsReload

                Layout.fillWidth: true
                Layout.preferredHeight: 40

                value: true
                label: "Reopen namespaces on reload"
                description: "(Disable to improve treeview performance)"
            }

            BoolOption {
                id: keySorting

                Layout.fillWidth: true
                Layout.preferredHeight: 40

                value: true
                label: "Enable key sorting in tree"
                description: "(Disable to improve treeview performance)"
            }

            IntOption {
                id: liveKeyLimit

                Layout.fillWidth: true
                Layout.preferredHeight: 40

                min: 100
                max: 100000
                value: 1000
                label: "Live update maximum allowed keys"
                description: ""
            }

            IntOption {
                id: liveUpdateInterval

                Layout.fillWidth: true
                Layout.preferredHeight: 40

                min: 3
                max: 100000
                value: 10
                label: "Live update interval (in seconds)"
                description: ""
            }

            Item {
                Layout.fillHeight: true
            }

            RowLayout {
                Layout.fillWidth: true

                Item { Layout.fillWidth: true; }
                Button {
                    text: "OK"
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
    }
}
