import QtQuick 2.0
import QtQuick.Layouts 1.1
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.1


Tab {
    title: "RDM"

    Rectangle {
        color: "transparent"
        anchors.fill: parent

        ColumnLayout {

            anchors.centerIn: parent

            RowLayout {

                id: topLayout
                property string styleString: '<style>a {color: darkblue; text-decoration: none}</style>'

                Image {
                    id: logo
                    source: "qrc:/images/logo.png"
                }

                ColumnLayout {

                    Text {
                        textFormat: Qt.RichText
                        text: topLayout.styleString + '<span style="font-size:24pt;">Redis Desktop Manager</span>'
                    }

                    Text {
                        textFormat: Qt.RichText
                        text: topLayout.styleString  + '<span style="padding-left:53px; font-size: 11px; line-height: 21px"><b>Version</b> ' + appVersion +' &nbsp;&nbsp;&nbsp; Developed by - <a href="http://careers.stackoverflow.com/IgorMalinovskiy">Igor Malinovskiy</a> in <a href="http://en.wikipedia.org/wiki/Ukraine">&nbsp;<img src="qrc:/images/ua.png" />&nbsp;Ukraine</a></span>'
                        onLinkActivated: Qt.openUrlExternally(link)
                    }

                    Text {
                        textFormat: Qt.RichText
                        text: topLayout.styleString  + '<span style="font-size: 11px;">Many thanks to <a href="http://redisdesktop.com/#contributors">our amazing Contributors</a> and <a href="http://redisdesktop.com/community">Community</a></span>'
                        onLinkActivated: Qt.openUrlExternally(link)
                    }
                }
            }

            Text {
                textFormat: Qt.RichText
                text: topLayout.styleString  + '<span style="font-size: 8px;">Used third party software and images:<a href="http://qt-project.org/"> Qt5</a>; <a href="http://libqxt.org/">Qxt</a> , <a href="http://www.libssh2.org">libssh2</a>, <a href="http://code.google.com/p/google-breakpad/">google breakpad</a>, <a href="https://github.com/uglide/QtConsole">QtConsole</a>, <a href="http://www.carlosprioglio.com/">Redis Logo</a> and other great OSS</span>'
                onLinkActivated: Qt.openUrlExternally(link)
            }

            Rectangle {
                color: "#cccccc"
                Layout.preferredHeight: 1
                Layout.fillWidth: true
            }

            Text {
                textFormat: Qt.RichText
                text: topLayout.styleString  + '<span style="font-size: 13px;">
<a style=" color: red;" href="https://github.com/uglide/RedisDesktopManager/issues"><img  src="qrc:/images/alert.png" />&nbsp;Report issue</a>
<a style="color: green; " href="https://github.com/uglide/RedisDesktopManager/wiki"><img  src="qrc:/images/help.png" />&nbsp;Documentation</a>
<a style="font-size: 13px; " href="https://twitter.com/RedisDesktop"><img  src="qrc:/images/twi.png" />&nbsp;Follow @RedisDesktop</a>
<a style="font-size: 13px; " href="https://github.com/uglide/RedisDesktopManager/stargazers"><img  src="qrc:/images/github.png" />&nbsp;Star!</a>
</span>'
                onLinkActivated: Qt.openUrlExternally(link)
            }
        }
    }
}
