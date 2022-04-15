import QtQuick 2.0
import QtQuick.Layouts 1.1
import QtQuick.Controls 2.3
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
                RichTextWithLinks { Layout.fillWidth: true; html: '<span style="font-size:30px;">RESP</span><sup style="font-size:18px; line-height:30px;">.app</sup><span style="font-size:30px;line-height: 30px;"> - GUI for Redis</span><sup style="font-size:18px;">®</sup>'}
                RichTextWithLinks { Layout.fillWidth: true; html: '<span style="font-size: 13px;"><b>'+ qsTranslate("RESP","Version") + '</b> ' + Qt.application.version +'</span>' }
                RichTextWithLinks { Layout.fillWidth: true;  html:  qsTranslate("RESP", '<span style="font-size: 11px;">Powered by awesome <a href="https://github.com/uglide/RedisDesktopManager/tree/2021/3rdparty">open-source software</a> and '
                                                                    + '<a href="http://icons8.com/">icons8</a>.</span>')}
            }
        }
                
    }
}
