import QtQuick 2.15
import QtQuick.Layouts 1.13
import QtQuick.Controls 2.13
import QtQuick.Controls 1.4 as LC
import Qt.labs.qmlmodels 1.0
import QtQuick.Window 2.2
import QtCharts 2.3
import "./../common"
import "./../common/platformutils.js" as PlatformUtils
import "./../settings"
import "./../value-editor"

ServerAction {
    id: tab

    Connections {
        target: tab.model? tab.model : null

        function onClientsChanged() {
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
            label: qsTranslate("RESP","Auto Refresh")

            onValueChanged: {
                tab.model.refreshClients = value
            }
        }        

        LegacyTableView {
            Layout.fillHeight: true
            Layout.fillWidth: true

            model: tab.model.clients ? tab.model.clients : []

            LC.TableViewColumn {
                role: "addr"
                title: qsTranslate("RESP","Client Address")
                width: 200
            }

            LC.TableViewColumn {
                role: "age"
                title: qsTranslate("RESP","Age (sec)")
                width: 75
            }

            LC.TableViewColumn {
                role: "idle"
                title: qsTranslate("RESP","Idle")
                width: 75
            }

            LC.TableViewColumn {
                role: "flags"
                title: qsTranslate("RESP","Flags")
                width: 75
            }

            LC.TableViewColumn {
                role: "db"
                title: qsTranslate("RESP","Current Database")
                width: 120
            }
        }
    }
}
