import QtQuick 2.5
import QtQuick.Layouts 1.1
import QtQuick.Controls 1.4
import "."

RowLayout {
    id: root

    property var shortcuts: {
        'add': Qt.platform.os == "osx"? "Meta+N" : "Ctrl+N",
        'reload': Qt.platform.os == "osx"? "Meta+R" : "Ctrl+R",
        'filter': Qt.platform.os == "osx"? "Meta+F" : "Ctrl+F",
        'live': Qt.platform.os == "osx"? "Meta+L" : "Ctrl+L",
    }

    function sc(t, a) {
        return t + " (" + root.shortcuts[a] + ")"
    }

    Shortcut {
        sequence: root.shortcuts['add']
        onActivated: {
            dbMenu.sendEvent('add_key')
        }
    }

    Shortcut {
        sequence: root.shortcuts['reload']
        onActivated: {
            dbMenu.sendEvent('reload')
        }
    }

    Shortcut {
        sequence: root.shortcuts['filter']
        onActivated: root.state = "filter"
    }

    Shortcut {
        sequence: root.shortcuts['live']
        onActivated: {
            liveButton.processClick()
        }
    }

    state: "menu"

    states: [
        State {
            name: "menu"
            PropertyChanges { target: root; spacing: 5;}
            PropertyChanges { target: dbMenu; visible: true;}
            PropertyChanges { target: filterText; visible: false;}
            PropertyChanges { target: filterOk; visible: false;}
            PropertyChanges { target: filterHelp; visible: false;}
            PropertyChanges { target: filterCancel; visible: false;}
        },
        State {
            name: "filter"
            PropertyChanges { target: root; spacing: 0;}
            PropertyChanges { target: dbMenu; visible: false;}
            PropertyChanges { target: filterText; visible: true;}
            PropertyChanges { target: filterOk; visible: true;}
            PropertyChanges { target: filterHelp; visible: true;}
            PropertyChanges { target: filterCancel; visible: true;}
        }
    ]

    RowLayout {
        id: dbMenu

        function sendEvent(e) {
            if (!connectionsManager)
                return

            connectionsManager.sendEvent(styleData.index, e)
        }

        ToolButton {
            tooltip: sc(qsTr("Open Keys Filter"), 'filter')
            iconSource: "qrc:/images/filter.svg"

            Layout.preferredWidth: 25
            Layout.preferredHeight: 25

            onClicked: root.state = "filter"
        }

        ToolButton {
            tooltip: sc(qsTr("Reload Keys in Database"), 'reload')
            iconSource: "qrc:/images/refresh.svg"

            Layout.preferredWidth: 25
            Layout.preferredHeight: 25

            onClicked: dbMenu.sendEvent("reload")
        }

        ToolButton {
            tooltip: sc(qsTr("Add New Key"), 'add')
            iconSource: "qrc:/images/add.svg"

            Layout.preferredWidth: 25
            Layout.preferredHeight: 25

            onClicked: dbMenu.sendEvent("add_key")
        }

        ToolButton {
            id: liveButton

            property bool liveUpdateEnabled: false
            tooltip: liveUpdateEnabled? qsTr("Disable Live Update") : sc(qsTr("Enable Live Update"), 'live')
            iconSource: liveUpdateEnabled? "qrc:/images/live_update_disable.svg" : "qrc:/images/live_update.svg"

            Layout.preferredWidth: 25
            Layout.preferredHeight: 25

            onClicked: processClick()

            function processClick() {
                if (liveUpdateEnabled) {
                    connectionsManager.setMetadata(styleData.index, "live_update", '')
                } else {
                    connectionsManager.setMetadata(styleData.index, "live_update", true)
                }

                liveUpdateEnabled = connectionsManager.getMetadata(styleData.index, "live_update")
            }

            Component.onCompleted: {
                liveUpdateEnabled = connectionsManager.getMetadata(styleData.index, "live_update")
            }
        }
    }

    TextField {
        id: filterText
        placeholderText: qsTr("Enter Filter")

        text: {
            if (!connectionsManager)
                return

            return connectionsManager.getMetadata(styleData.index, "filter")
        }

        onAccepted: filterOk.setFilter()
    }

    ToolButton {
        id: filterOk
        iconSource: "qrc:/images/ok.svg"

        onClicked: setFilter()

        function setFilter() {
            if (!connectionsManager)
                return

            connectionsManager.setMetadata(styleData.index, "filter", filterText.text)
            root.state = "menu"
        }
    }

    ToolButton {
        id: filterHelp
        iconSource: "qrc:/images/help.svg"
        onClicked: Qt.openUrlExternally("https://github.com/uglide/RedisDesktopManager/wiki/Features#Filter")
    }

    ToolButton {
        id: filterCancel
        iconSource: "qrc:/images/clear.svg"

        onClicked: {
            if (!connectionsManager)
                return

            connectionsManager.setMetadata(styleData.index, "filter", "")
            root.state = "menu"
        }
    }
}
