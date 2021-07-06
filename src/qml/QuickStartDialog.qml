import QtQuick 2.3
import QtQuick.Layouts 1.1
import QtQuick.Controls 2.13
import QtQuick.Window 2.3
import "./common"
import "./common/platformutils.js" as PlatformUtils

BetterDialog {
    id: root
    objectName: "rdm_quick_start_dialog"
    title: qsTranslate("RDM","Getting Started")

    footer: null

    contentItem: Rectangle {
        id: rootItem
        color: sysPalette.base
        anchors.fill: parent
        implicitWidth:  750
        implicitHeight: 150

        Control {
            palette: approot.palette
            anchors.fill: parent
            anchors.margins: 30

            ColumnLayout {
                anchors.fill: parent

                Item { Layout.fillHeight: true }

                RowLayout {
                    id: msgLayout

                    Layout.fillHeight: true
                    Layout.alignment: Qt.AlignHCenter

                    BetterLabel {
                        Layout.fillWidth: true
                        wrapMode: Text.WrapAnywhere
                        text: qsTranslate("RDM","Thank you for choosing RDM. Let's make your Redis experience better.")
                        font.pixelSize: 16

                        Component.onCompleted: {
                            if (!PlatformUtils.isOSX()) {
                                root.width = contentWidth
                            }
                        }
                    }
                }

                Item { Layout.fillHeight: true }

                RowLayout {

                    Item { Layout.fillWidth: true }

                    BetterButton {
                        text: qsTranslate("RDM","Connect to Redis-Server")
                        palette.button: "#c6302b"
                        palette.buttonText: "#ffffff"
                        onClicked: {
                            root.close()
                            connectionSettingsDialog.settings = connectionsManager.createEmptyConfig()
                            connectionSettingsDialog.open()
                        }
                    }

                    BetterButton {
                        property string url: "http://docs.rdm.dev/en/latest/quick-start/"

                        text: qsTranslate("RDM","Read the Docs")
                        tooltip: url

                        onClicked: Qt.openUrlExternally(url)
                    }

                    Item { Layout.fillWidth: true }
                }

                Item { Layout.fillHeight: true }
            }
        }
    }
}
