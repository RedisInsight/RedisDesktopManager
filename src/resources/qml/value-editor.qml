import QtQuick 2.0
import QtQuick.Layouts 1.1
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.1
import QtQuick.Dialogs 1.2
import QtWebEngine 1.1
import "."
import "./editors/formatters/formatters.js" as Formatters

Rectangle {
    id: approot
    objectName: "rdm_qml_root"
    color: "transparent"
    property var currentValueFormatter

    TabView {
        id: tabs
        objectName: "rdm_qml_tabs" 
        anchors.fill: parent
        anchors.margins: 2
        currentIndex: 0

        onCurrentIndexChanged: {
            var index = currentIndex
            if (tabs.getTab(0).not_mapped) index -= 1
            viewModel.setCurrentTab(index)
        }

        Tab {
            title: "web_view!"

            WebEngineView {
                id: webview

                anchors.fill: parent

                Component.onCompleted: {
                    var json_editor = '<html><head><link rel="stylesheet" href="qrc:/highlight.css"></head><body>'
                    + '<pre><code>{"a": true}</code></pre>'
                    + '<script src="qrc:/highlight.js"></script>'
                    + '<script>hljs.initHighlightingOnLoad();</script></body></html>'

                    webview.loadHtml(json_editor)
                }
            }
        }

        WelcomeTab {
            objectName: "rdm_qml_welcome_tab"
            property bool not_mapped: true

            function close(index) {
                tabs.removeTab(index)
            }
        }

        ValueTabs {
            objectName: "rdm_qml_value_tabs"
            model: viewModel
        }

        style: TabViewStyle {
            tab: Rectangle {
                color: "#cccccc"
                implicitWidth: layout.implicitWidth + 3
                implicitHeight: 28
                radius: 2

                Rectangle {
                    id: content
                    color: styleData.selected ? "white" :"#cccccc"
                    anchors.fill: parent
                    anchors.topMargin: styleData.selected
                    anchors.rightMargin: styleData.selected
                    anchors.leftMargin: styleData.selected

                    RowLayout {
                        id: layout
                        anchors.fill: parent
                        anchors.rightMargin: 8

                        Item { Layout.preferredWidth: 3 }

                        Text {
                            Layout.fillWidth: true
                            Layout.minimumWidth: implicitWidth
                            color: "#D11313"
                            text: styleData.title
                        }
                        Item {
                            Layout.preferredWidth: 18
                            Layout.preferredHeight: 18

                            Image {
                                anchors.fill: parent
                                anchors.margins: 2
                                source: "qrc:/images/clear.png"
                                MouseArea {
                                    anchors.fill: parent
                                    onClicked: {
                                        tabs.getTab(styleData.index).close(styleData.index)
                                    }
                                }
                            }
                        }
                    }
                }
            }

            frame: Rectangle {
                color: "#cccccc"

                Rectangle {
                    color: "white"
                    anchors.fill: parent
                    anchors.bottomMargin: 1
                    anchors.leftMargin: 1
                    anchors.rightMargin: 1
                    anchors.topMargin: 1
                }
            }
        }

    }


    MessageDialog {
        id: errorNotification
        objectName: "rdm_qml_error_dialog"
        visible: false
        modality: Qt.WindowModal
        icon: StandardIcon.Warning
        standardButtons: StandardButton.Ok
    }

    AddKeyDialog {
       id: addNewKeyDialog
       objectName: "rdm_qml_new_key_dialog"
    }

    Connections {
        target: viewModel
        onKeyError: {
            if (index != -1)
                tabs.currentIndex = index + 1
            errorNotification.text = error
            errorNotification.open()
        }

        onCloseWelcomeTab: {
            var welcomeTab = tabs.getTab(0)

            if (welcomeTab && welcomeTab.not_mapped)
               tabs.removeTab(0)
        }

        onNewKeyDialog: {
            console.log(dbIdentificationString, keyPrefix)
            addNewKeyDialog.open()
        }
    }
}
