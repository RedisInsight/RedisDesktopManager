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

                Image { id: logo; source: "qrc:/images/logo.png"}

                ColumnLayout {
                    RichTextWithLinks { html: '<span style="font-size:32pt;">Redis Desktop Manager</span>'}
                    RichTextWithLinks { html: '<span style="padding-left:85px; font-size: 14px; line-height: 21px; display: inline-block;">&nbsp;<b>Version</b> ' + Qt.application.version +' &nbsp;&nbsp;&nbsp; '
                                              + 'Developed by - <a href="http://github.com/uglide">Igor Malinovskiy</a> in '
                                              + '<a href="http://en.wikipedia.org/wiki/Ukraine">&nbsp;<img src="qrc:/images/ua.svg" width="20" height="20" />&nbsp;Ukraine</a></span>'}
                }
            }

            Rectangle { color: "#cccccc"; Layout.preferredHeight: 1; Layout.fillWidth: true }

            RowLayout {
                spacing: 5
                Layout.fillWidth: true

                RichTextWithLinks {
                    Layout.fillWidth: true
                    html: '<a style="color: red; font-size: 16px;" href="https://github.com/uglide/RedisDesktopManager/issues"><img width="25" src="qrc:/images/alert.svg" />&nbsp;Report issue</a> '
                }
                RichTextWithLinks {
                    Layout.fillWidth: true
                    html:  '<a style="color: green; font-size: 16px;" href="http://docs.redisdesktop.com/en/latest/"><img width="25" src="qrc:/images/help.svg" />&nbsp;Documentation</a>'
                }
                RichTextWithLinks {
                    Layout.fillWidth: true
                    html: '<a style="font-size: 16px; " href="https://gitter.im/uglide/RedisDesktopManager"><img width="25" src="qrc:/images/chat.svg" />&nbsp;Join Gitter Chat</a> '
                }
                RichTextWithLinks {
                    Layout.fillWidth: true
                    html: '<a style="font-size: 16px; " href="https://twitter.com/RedisDesktop"><img width="25" src="qrc:/images/twi.svg" />&nbsp;Follow</a> '
                }
                RichTextWithLinks {
                    Layout.fillWidth: true
                    html: '<a style="font-size: 16px; " href="https://github.com/uglide/RedisDesktopManager/stargazers"><img width="25" src="qrc:/images/github.svg" />&nbsp;Star!</a>'
                }
            }

            Rectangle { color: "#cccccc"; Layout.preferredHeight: 1; Layout.fillWidth: true }


            RichTextWithLinks { html: '<span style="font-size: 11px;">Used third party software and images: <a href="http://qt.io/">Qt</a>, '
                                      + '<a href="https://github.com/uglide/qredisclient">QRedisClient</a>, '
                                      + '<a href="http://code.google.com/p/google-breakpad/">Google Breakpad</a>, '
                                      + '<a href="http://icons8.com/">Icons from icons8.com</a>, '
                                      + '<a href="http://redis.io/">Redis Logo</a>.</span>'}

            Rectangle { color: "#cccccc"; Layout.preferredHeight: 1; Layout.fillWidth: true }

            Item {
                Layout.fillWidth: true
                Layout.preferredHeight: 10
            }


            RichTextWithLinks { html: '<span style="font-size: 13pt;">Many thanks to <a href="http://redisdesktop.com/#contributors">our amazing Contributors</a>, '
                                      + '<a href="https://redisdesktop.com/subscribe">Supporters</a> and '
                                      + '<a href="http://redisdesktop.com/community">Community</a></span>'}
        }
    }
}
