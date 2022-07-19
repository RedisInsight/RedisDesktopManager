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
    id: tab

    function stopTimer() {
        model.refreshPubSubMonitor = false
    }

    Connections {
        target: tab.model? tab.model : null

        function onPubSubChannelsChanged() {
            if (uiBlocked) {
                uiBlocked = false
            }
        }
    }

    ColumnLayout {

        anchors.fill: parent
        anchors.margins: 10

        BoolOption {
            Layout.preferredWidth: 200
            Layout.preferredHeight: 40

            value: true
            label: qsTranslate("RESP","Enable")

            onValueChanged: {
                tab.model.refreshPubSubMonitor = value
            }
        }

        LegacyTableView {
            Layout.fillHeight: true
            Layout.fillWidth: true

            model: tab.model.pubSubChannels ? tab.model.pubSubChannels : []

            rowDelegate: Item {
                height: 50
            }

            LC.TableViewColumn {
                role: "addr"
                title: qsTranslate("RESP","Channel Name")
                width: 200
            }

            LC.TableViewColumn {
                role: "addr"
                width: 200
                delegate: Item {
                    BetterButton {
                        objectName: "rdm_server_info_pub_sub_subscribe_to_channel_btn"
                        anchors.centerIn: parent
                        text: qsTranslate("RESP","Subscribe in Console")
                        onClicked: {
                            console.log(styleData.value)
                            tab.model.subscribeToChannel(styleData.value)
                        }
                    }
                }
            }

        }
    }
}
