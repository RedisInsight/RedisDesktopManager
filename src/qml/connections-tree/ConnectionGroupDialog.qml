import QtQuick 2.3
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.13
import QtQuick.Window 2.3
import "../common"
import "../common/platformutils.js" as PlatformUtils

BetterDialog {
    id: root
    title: group? qsTranslate("RESP","Edit Connections Group") + group.name : qsTranslate("RESP","Add New Connections Group")
    visible: false
    property var group

    footer: null

    signal addNewGroup(string name)
    signal editGroup(var group)

    Item {
        anchors.fill: parent
        implicitHeight: 150
        implicitWidth: 600

        ColumnLayout {
            anchors.fill: parent
            anchors.margins: 5

            BetterLabel {
                text: qsTranslate("RESP","Group Name:")
            }
            BetterTextField {
                id: groupName
                Layout.fillWidth: true
                objectName: "rdm_connections_group_field"
                text: group? group.name : ''
            }

            RowLayout {
                Layout.fillWidth: true
                Layout.minimumHeight: 40
                Item {
                    Layout.fillWidth: true
                }
                BetterButton {
                    objectName: "rdm_connections_group_save_btn"
                    text: qsTranslate("RESP","Save")

                    onClicked: {                        
                        if (group) {
                            group.name = groupName.text
                            root.editGroup(group)
                        } else {
                            root.addNewGroup(groupName.text)
                        }

                        root.close()
                    }
                }

                BetterButton {
                    text: qsTranslate("RESP","Cancel")
                    onClicked: root.close()
                }
            }
            Item { Layout.fillWidth: true }
        }
    }
}
