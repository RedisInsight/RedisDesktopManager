import QtQuick 2.0
import QtQuick.Layouts 1.1
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.1
import "./parts"

Tab {
    title: "RDM"

    Rectangle {
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
                    RichTextWithLinks { html: '<span style="padding-left:85px; font-size: 14px; line-height: 21px; display: inline-block;">&nbsp;<b>Version</b> ' + appVersion +' &nbsp;&nbsp;&nbsp; '
                                              + 'Developed by - <a href="http://careers.stackoverflow.com/IgorMalinovskiy">Igor Malinovskiy</a> in '
                                              + '<a href="http://en.wikipedia.org/wiki/Ukraine">&nbsp;<img src="qrc:/images/ua.png" />&nbsp;Ukraine</a></span>'}
                }
            }

            Rectangle { color: "#cccccc"; Layout.preferredHeight: 1; Layout.fillWidth: true }

            RowLayout {
                spacing: 5
                Layout.fillWidth: true

                RichTextWithLinks {
                    Layout.fillWidth: true
                    html: '<a style=" color: red; font-size: 14px;" href="https://github.com/uglide/RedisDesktopManager/issues"><img  src="qrc:/images/alert.png" />&nbsp;Report issue</a> '
                }
                RichTextWithLinks {
                    Layout.fillWidth: true
                    html:  '<a style="color: green; font-size: 14px;" href="https://github.com/uglide/RedisDesktopManager/wiki"><img  src="qrc:/images/help.png" />&nbsp;Wiki</a>'
                }
                RichTextWithLinks {
                    Layout.fillWidth: true
                    html: '<a style="font-size: 14px; " href="https://gitter.im/uglide/RedisDesktopManager"><img height="16" src="qrc:/images/gitter.png" />&nbsp;Join Gitter Chat</a> '
                }
                RichTextWithLinks {
                    Layout.fillWidth: true
                    html: '<a style="font-size: 14px; " href="https://twitter.com/RedisDesktop"><img  src="qrc:/images/twi.png" />&nbsp;Follow @RedisDesktop</a> '
                }
                RichTextWithLinks {
                    Layout.fillWidth: true
                    html: '<a style="font-size: 14px; " href="https://github.com/uglide/RedisDesktopManager/stargazers"><img  src="qrc:/images/github.png" />&nbsp;Star!</a>'
                }
            }

            Rectangle { color: "#cccccc"; Layout.preferredHeight: 1; Layout.fillWidth: true }

            RichTextWithLinks { html: '<span style="font-size: 13px;">Many thanks to <a href="http://redisdesktop.com/#contributors">our amazing Contributors</a>, '
                                      + '<a href="https://www.bountysource.com/teams/redisdesktopmanager/backers">Backers</a> and '
                                      + '<a href="http://redisdesktop.com/community">Community</a></span>'}

            RichTextWithLinks {
                Layout.maximumWidth: footerMsg.contentWidth
                html: '<span style="font-size: 12px;"><b>Special thanks to:</b> '
                                      + 'andrewjknox, Rob T., chasm, mjirby, linux_china, GuRui, '
                                      + 'cristianobaptista, stgogm, ryanski44, Itamar Haber, elliots, caywood, '
                                      + 'chrisgo, pmercier, henkvos, sun.ming.77, trelsco, Sai P.S.,'
                                      +' Anonymous, Wrhector, richard.hoogenboom, cblage, WillPerone, rodogu, peters'
                                      + '</span>'
                                      }

            RichTextWithLinks { html: '<span style="font-size: 9px;">Used third party software and images:<a href="http://qt-project.org/"> Qt5</a>; '
                                      + '<a href="http://libqxt.org/">Qxt</a> , <a href="http://www.libssh2.org">libssh2</a>, '
                                      + '<a href="http://code.google.com/p/google-breakpad/">google breakpad</a>, '
                                      + '<a href="https://github.com/uglide/QtConsole">QtConsole</a>, '
                                      + '<a href="http://www.carlosprioglio.com/">Redis Logo</a> and other great OSS</span>'}

            Rectangle { color: "#cccccc"; Layout.preferredHeight: 1; Layout.fillWidth: true }

            Item {
                Layout.fillWidth: true
                Layout.preferredHeight: 10
            }

            RichTextWithLinks {
                id: footerMsg
                html: '<span style="font-size: 15pt; font-weight: 700;"><a style="color: dark-grey;" href="https://www.bountysource.com/teams/redisdesktopmanager">IF YOU FEEL THAT IS USEFUL, <u style="color:green">DONATE</u> TO HELP KEEP DEVELOPMENT GOING.</a></span>'
            }
        }
    }
}
