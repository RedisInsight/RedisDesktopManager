import QtQuick 2.3
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.13
import QtQuick.Dialogs 1.2
import QtQuick.Window 2.3
import "../common"
import "../common/platformutils.js" as PlatformUtils

BetterDialog {
    id: root
    title: group? qsTranslate("RDM","Edit Connections Group") + group.name : qsTranslate("RDM","Add New Connections Group")
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
                text: qsTranslate("RDM","Group Name:")
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
                    text: qsTranslate("RDM","Save")

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
                    text: qsTranslate("RDM","Cancel")
                    onClicked: root.close()
                }
            }
            Item { Layout.fillWidth: true }
        }

        OkDialog {
            id: dialogError
            title: qsTranslate("RDM","Error")
            text: ""
            visible: false
        }
    }
}
