import QtQuick 2.3
import QtQuick.Layouts 1.1
import QtQuick.Controls 1.2
import QtQuick.Dialogs 1.2
import "./common"

Dialog {
    id: root
    objectName: "rdm_quick_start_dialog"
    title: qsTr("Explore Redis Desktop Manager")

    contentItem: Item {
        implicitWidth: msgLayout.implicitWidth + 50
        implicitHeight: 150

        ColumnLayout {
            anchors.centerIn: parent

            RowLayout {
                id: msgLayout

                Layout.fillHeight: true

                Image { source: "qrc:/images/help.svg" }

                RichTextWithLinks {

                    html: "<p style='font-size: 13pt;'>" + qsTr("Before using Redis Desktop Manager (RDM) take a look on the %1").arg(
                              "<a href='http://docs.redisdesktop.com/en/latest/quick-start/'>" + qsTr("Quick Start Guide")+ "</a>") + "</p>"
                }
            }

            RowLayout {
                Layout.fillWidth: true

                Item { Layout.fillWidth: true; }
                Button {
                    objectName: "rdm_quick_start_dialog_ok_btn"
                    text: qsTr("OK")
                    onClicked: root.close()
                }
            }
        }
    }
}
