import QtQuick 2.3
import QtQuick.Layouts 1.1
import QtQuick.Controls 1.2
import QtQuick.Dialogs 1.2
import "./common"

Dialog {
    id: root
    title: "Explore Redis Desktop Manager"

    contentItem: Item {
        implicitWidth: msgLayout.implicitWidth + 50
        implicitHeight: 120

        ColumnLayout {
            anchors.centerIn: parent

            RowLayout {
                id: msgLayout

                Layout.fillHeight: true

                Image { source: "qrc:/images/help.png" }

                RichTextWithLinks {

                    html: "<p style='font-size: 13pt;'>Before using Redis Desktop Manager (RDM) take a look on the "
                          + "<a href='https://github.com/uglide/RedisDesktopManager/wiki/Quick-Start'>Quick Start Guide</a>"
                          + "</p>"
                }
            }

            RowLayout {
                Layout.fillWidth: true

                Item { Layout.fillWidth: true; }
                Button {
                    text: "Ok"
                    onClicked: root.close()
                }
            }
        }
    }
}
