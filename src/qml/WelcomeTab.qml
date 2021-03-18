import QtQuick 2.0
import QtQuick.Layouts 1.1
import QtQuick.Controls 2.3
import QtQuick.Dialogs 1.3
import "./common"

BetterTab {
    id: root
    ColumnLayout {
        anchors.centerIn: parent
        width: Math.min(parent.width * 0.9, 600)

        RowLayout {
            id: topLayout
            spacing: 15
            Layout.fillWidth: true
            Layout.preferredHeight: 120

            Image {
                id: logo
                source: "qrc:/images/logo.png"
                Layout.preferredWidth: 120
                Layout.preferredHeight: 120
                fillMode: Image.PreserveAspectFit
            }

            ColumnLayout {
                Layout.fillWidth: true
                RichTextWithLinks { Layout.fillWidth: true; html: '<span style="font-size:30px;">RDM.dev - GUI for Redis<sup>®</sup></span>'}
                RichTextWithLinks { Layout.fillWidth: true; html: '<span style="font-size: 13px;"><b>'+ qsTranslate("RDM","Version") + '</b> ' + Qt.application.version +'</span>' }
                RichTextWithLinks { Layout.fillWidth: true;  html:  qsTranslate("RDM", '<span style="font-size: 11px;">Powered by awesome <a href="https://github.com/uglide/RedisDesktopManager/tree/2021/3rdparty">open-source software</a> and '
                                                                    + '<a href="http://icons8.com/">icons8</a>.</span>')}
            }
        }
                
    }
}
