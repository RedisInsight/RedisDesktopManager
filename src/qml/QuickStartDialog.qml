import QtQuick 2.3
import QtQuick.Layouts 1.1
import QtQuick.Controls 1.2
import QtQuick.Dialogs 1.2
import "./common"
import "./common/platformutils.js" as PlatformUtils

Dialog {
    id: root
    objectName: "rdm_quick_start_dialog"
    title: qsTranslate("RDM","Explore Redis Desktop Manager")

    contentItem: Item {
        id: rootItem
        anchors.fill: parent
        anchors.margins: 15
        implicitWidth:  750
        implicitHeight: 150

        ColumnLayout {
            anchors.fill: parent

            RowLayout {
                id: msgLayout

                Layout.fillHeight: true
                Layout.alignment: Qt.AlignHCenter

                Image {
                    source: "qrc:/images/help.svg"
                    sourceSize.width: 50
                    sourceSize.height: 50
                }

                RichTextWithLinks {
                    wrapMode: Text.WrapAnywhere
                    html: "<p style='font-size: 13pt;'>" + qsTranslate("RDM","Before using Redis Desktop Manager (RDM) take a look on the %1").arg(
                              "<a href='http://docs.redisdesktop.com/en/latest/quick-start/'>" + qsTranslate("RDM","Quick Start Guide")+ "</a>") + "</p>"

                    Component.onCompleted: {
                        if (!PlatformUtils.isOSX()) {
                            root.width = contentWidth
                        }
                    }
                }
            }

            RowLayout {
                Layout.fillWidth: true

                Item { Layout.fillWidth: true; }
                Button {
                    objectName: "rdm_quick_start_dialog_ok_btn"
                    text: qsTranslate("RDM","OK")
                    onClicked: root.close()
                }
            }
        }
    }
}
