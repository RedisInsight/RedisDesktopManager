import QtQuick 2.0
import QtQuick.Layouts 1.1
import QtQuick.Controls 2.3
import "./common"
import "./common/platformutils.js" as PlatformUtils

BetterTab {
    id: root
    ColumnLayout {
        anchors.centerIn: parent
        width: Math.min(parent.width * 0.9, 600)

        RowLayout {
            id: topLayout
            spacing: 15
            Layout.fillWidth: true
            Layout.preferredHeight: 350

            Image {
                id: logo
                source: "qrc:/images/redisinsight.svg"
                Layout.preferredWidth: 50
                Layout.preferredHeight: 50
                Layout.alignment: Qt.AlignTop
                fillMode: Image.PreserveAspectFit
            }

            ColumnLayout {
                Layout.fillWidth: true
                RichTextWithLinks { Layout.fillWidth: true; html: '<span style="font-size:26px;"><b>Redis</b>Insight is the successor to <span style="color: grey;">RESP.app</span></span>'}
                RichTextWithLinks { Layout.fillWidth: true; html: '<div style="font-size:14px; line-height: 120%">In 2022, <a href="https://redis.com/blog/respapp-joining-redis/">Redis joined forces with the creator of RESP.app, Igor Malinovskyi</a>, '
                                                                  + 'bringing RESP.app’s popular features into RedisInsight. RedisInsight now provides improved '
                                                                  + 'performance and these additional features:</div>'
                                                                  + '<ul style="font-size:14px;">'
                                                                  + '<li style="line-height: 150%">SSH tunneling support</li>'
                                                                  + '<li style="line-height: 150%">Support for <a href="https://redis.io/docs/stack/about/">RedisStack</a></li>'
                                                                  + '<li style="line-height: 150%">Database analysis and performance improvement recommendations</li>'
                                                                  + '<li style="line-height: 150%">Advanced CLI with syntax highlighting and autocomplete</li>'
                                                                  + '<li style="line-height: 150%">… and much, much more</li>'
                                                                  + '</ul>' }
                RowLayout {
                    Layout.fillWidth: true
                    Layout.alignment: Qt.AlignHCenter
                    Layout.margins: 20;

                    BetterButton {
                        text: qsTranslate("RESP","Download from Snapcraft")
                        onClicked: Qt.openUrlExternally("https://snapcraft.io/redisinsight")
                        visible: PlatformUtils.isLinux()
                    }

                    BetterButton {
                        text: qsTranslate("RESP","Download from Flathub")
                        onClicked: Qt.openUrlExternally("https://flathub.org/apps/details/com.redis.RedisInsight")
                        visible: PlatformUtils.isLinux()
                    }

                    BetterButton {
                        text: qsTranslate("RESP","Download from Microsoft Store")
                        onClicked: Qt.openUrlExternally("https://apps.microsoft.com/store/detail/redisinsight/XP8K1GHCB0F1R2")
                        visible: PlatformUtils.isWindows()
                    }

                    BetterButton {
                        text: qsTranslate("RESP","Download from AppStore")
                        onClicked: Qt.openUrlExternally("https://apps.apple.com/us/app/redisinsight/id6446987963")
                        visible: PlatformUtils.isOSX()
                    }

                    BetterButton {
                        text: PlatformUtils.isWindows() ? qsTranslate("RESP","Download Installer") : qsTranslate("RESP","Download DMG")
                        onClicked: Qt.openUrlExternally("https://redis.com/redis-enterprise/redis-insight/")
                        visible: !PlatformUtils.isLinux()
                    }
                }


               RichTextWithLinks { Layout.fillWidth: true; html: '<div style="font-size:15px; line-height: 120%">Thank you for being a RESP.app user! '
                                                                 + ' To transfer your connections to RedisInsight seamlessly, please follow the <a href="https://resp.app/migration-guide/">migration guide</a>. If you face any migration issues, please contact <a href="mailto:igor@resp.app">igor@resp.app</a>.</div>'}

            }
        }
                
    }
}
