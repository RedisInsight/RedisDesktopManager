import QtQuick 2.0
import QtQuick.Layouts 1.1
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.1
import "./common"

BetterTab {
    title: "Redis Desktop Manager"
    icon: "qrc:/images/logo.png"

    Rectangle {
        id: parentWrapper
        anchors.fill: parent
        color: "transparent"

        ColumnLayout {
            anchors.centerIn: parent

            RowLayout {
                id: topLayout
                spacing: 15
                Layout.fillWidth: true

                Image {
                    id: logo
                    source: "qrc:/images/logo.png"
                    Layout.preferredWidth: 120
                    fillMode: Image.PreserveAspectFit
                }

                ColumnLayout {
                    RichTextWithLinks { html: '<span style="font-size:32pt;">Redis Desktop Manager</span>'}
                    RichTextWithLinks { html: '<span style="font-size: 13px;"><b>Version</b> ' + Qt.application.version +' &nbsp;&nbsp;&nbsp; '
                                              + '<span style="font-size: 13px;">Many thanks to <a href="http://redisdesktop.com/#contributors">our amazing Contributors</a>, '
                                              + '<a href="https://redisdesktop.com/subscribe">Supporters</a></span>'
                    }

                    RichTextWithLinks { html:  '<span style="font-size: 11px;">Powered by awesome <a href="https://github.com/uglide/RedisDesktopManager/tree/2019/3rdparty">open-source software</a>, '
                                               + '<a href="http://icons8.com/">icons from icons8.com</a> and '
                                               + '<a href="http://redis.io/">Redis Logo</a>.</span>'}
                }
            }
        }
    }
}
