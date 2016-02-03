import QtQuick 2.3
import QtQuick.Layouts 1.1
import QtQuick.Controls 1.4
import "."

RowLayout {
    id: root

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
            tooltip: qsTr("Open Keys Filter")
            iconSource: "qrc:/images/filter.png"

            Layout.preferredWidth: 25
            Layout.preferredHeight: 25

            onClicked: root.state = "filter"
        }

        ToolButton {
            tooltip: qsTr("Reload Keys in Database")
            iconSource: "qrc:/images/refresh.png"

            Layout.preferredWidth: 25
            Layout.preferredHeight: 25

            onClicked: dbMenu.sendEvent("reload")
        }

        ToolButton {
            tooltip: qsTr("Add New Key")
            iconSource: "qrc:/images/add.png"

            Layout.preferredWidth: 25
            Layout.preferredHeight: 25

            onClicked: dbMenu.sendEvent("add_key")
        }

        ToolButton {
            property bool liveUpdateEnabled: false
            tooltip: liveUpdateEnabled? qsTr("Disable Live Update") : qsTr("Enable Live Update")
            iconSource: liveUpdateEnabled? "qrc:/images/live_update_disable.png" : "qrc:/images/live_update.png"

            Layout.preferredWidth: 25
            Layout.preferredHeight: 25

            onClicked: {
                if (liveUpdateEnabled) {
                    connectionsManager.setMetadata(styleData.index, "live_update", false)
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
        placeholderText: "Enter Filter"

        text: {
            if (!connectionsManager)
                return

            return connectionsManager.getMetadata(styleData.index, "filter")
        }

        onAccepted: filterOk.setFilter()
    }

    ToolButton {
        id: filterOk
        iconSource: "qrc:/images/ok.png"

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
        iconSource: "qrc:/images/help.png"
    }

    ToolButton {
        id: filterCancel
        iconSource: "qrc:/images/clear.png"

        onClicked: {
            if (!connectionsManager)
                return

            connectionsManager.setMetadata(styleData.index, "filter", "")
            root.state = "menu"
        }
    }
}
