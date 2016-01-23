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
                                              + 'Developed by - <a href="http://careers.stackoverflow.com/IgorMalinovskiy">Igor Malinovskiy</a> in '
                                              + '<a href="http://en.wikipedia.org/wiki/Ukraine">&nbsp;<img src="qrc:/images/ua.png" width="20" height="20" />&nbsp;Ukraine</a></span>'}
                }
            }

            Rectangle { color: "#cccccc"; Layout.preferredHeight: 1; Layout.fillWidth: true }

            RowLayout {
                spacing: 5
                Layout.fillWidth: true

                RichTextWithLinks {
                    Layout.fillWidth: true
                    html: '<a style="color: red; font-size: 16px;" href="https://github.com/uglide/RedisDesktopManager/issues"><img src="qrc:/images/alert.png" />&nbsp;Report issue</a> '
                }
                RichTextWithLinks {
                    Layout.fillWidth: true
                    html:  '<a style="color: green; font-size: 16px;" href="https://github.com/uglide/RedisDesktopManager/wiki"><img  src="qrc:/images/help.png" />&nbsp;Wiki</a>'
                }
                RichTextWithLinks {
                    Layout.fillWidth: true
                    html: '<a style="font-size: 16px; " href="https://gitter.im/uglide/RedisDesktopManager"><img src="qrc:/images/chat.png" />&nbsp;Join Gitter Chat</a> '
                }
                RichTextWithLinks {
                    Layout.fillWidth: true
                    html: '<a style="font-size: 16px; " href="https://twitter.com/RedisDesktop"><img  src="qrc:/images/twi.png" />&nbsp;Follow</a> '
                }
                RichTextWithLinks {
                    Layout.fillWidth: true
                    html: '<a style="font-size: 16px; " href="https://github.com/uglide/RedisDesktopManager/stargazers"><img  src="qrc:/images/github.png" />&nbsp;Star!</a>'
                }
            }

            Rectangle { color: "#cccccc"; Layout.preferredHeight: 1; Layout.fillWidth: true }

            RichTextWithLinks { html: '<span style="font-size: 13px;">Many thanks to <a href="http://redisdesktop.com/#contributors">our amazing Contributors</a>, '
                                      + '<a href="https://www.bountysource.com/teams/redisdesktopmanager/backers">Backers</a> and '
                                      + '<a href="http://redisdesktop.com/community">Community</a></span>'}

            RichTextWithLinks {
                Layout.maximumWidth: topLayout.implicitWidth
                html: '<span style="font-size: 12px;"><b>Special thanks to:</b> '
                                      + 'andrewjknox, Rob T., chasm, mjirby, linux_china, GuRui, '
                                      + 'cristianobaptista, stgogm, ryanski44, Itamar Haber, elliots, caywood, '
                                      + 'chrisgo, pmercier, henkvos, sun.ming.77, trelsco, Sai P.S.,'
                                      +' Anonymous, Wrhector, richard.hoogenboom, cblage, WillPerone, rodogu, peters'
                                      + '</span>'
                                      }

            RichTextWithLinks { html: '<span style="font-size: 11px;">Used third party software and images: <a href="http://qt.io/">Qt</a>, '
                                      + '<a href="https://github.com/uglide/qredisclient">QRedisClient</a>, '
                                      + '<a href="http://code.google.com/p/google-breakpad/">Google Breakpad</a>, '
                                      + '<a href="http://icons8.com/">Icons from icons8.com</a>, '
                                      + '<a href="http://redis.io/">Redis Logo</a>.</span>'}

            Rectangle { color: "#cccccc"; Layout.preferredHeight: 1; Layout.fillWidth: true }

            RichTextWithLinks {
                Layout.fillWidth: true
                html: '<img align="left" src="qrc:/images/ga.png" height="50" width="50" /><span style="font-size: 12px; display: block; ">'
                                      + ' Redis Desktop Manager uses Google Analytics to track which features you are using. '
                                      + ' <br />&nbsp;This data helps <a href="https://github.com/uglide">me</a> to develop features that you actually need :)'
                                      + ' <br />&nbsp;RDM <b>doesn\'t</b> send <a href="https://github.com/uglide/RedisDesktopManager/search?q=GoogleMP&utf8=%E2%9C%93">any sensitive information or data from your databases.</a>'
                                      + ' <a href="https://github.com/uglide/RedisDesktopManager/wiki/Google-Analytics">More ></a>'
                                      + ' </span>'}

            Rectangle { color: "#cccccc"; Layout.preferredHeight: 1; Layout.fillWidth: true }

            Item {
                Layout.fillWidth: true
                Layout.preferredHeight: 10
            }

            RichTextWithLinks {
                id: footerMsg
                html: '<span style="font-size: 20px; font-weight: 700;"><a style="color: dark-grey;" href="https://www.bountysource.com/teams/redisdesktopmanager">JUST <span style="color:red">DO</span><span style="font-size: 10px; color: grey;">NATE</span> IT!</a></span>'
            }
        }
    }
}
