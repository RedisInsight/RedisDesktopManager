import QtQuick 2.3
import QtQuick.Layouts 1.1
import QtQuick.Controls 2.13
import QtQuick.Window 2.3
import "./common"
import "./common/platformutils.js" as PlatformUtils

BetterDialog {
    id: root
    objectName: "rdm_quick_start_dialog"
    title: qsTranslate("RDM","Explore RDM")

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
            anchors.margins: 15

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
                        Layout.fillWidth: true
                        wrapMode: Text.WrapAnywhere
                        html: "<p style='font-size: 13pt;'>" + qsTranslate("RDM","Before using RDM take a look on the %1").arg(
                                  "<a href='http://docs.rdm.dev/en/latest/quick-start/'>" + qsTranslate("RDM","Quick Start Guide")+ "</a>") + "</p>"

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
                    BetterButton {
                        objectName: "rdm_quick_start_dialog_ok_btn"
                        text: qsTranslate("RDM","OK")
                        onClicked: root.close()
                    }
                }
            }
        }
    }
}
