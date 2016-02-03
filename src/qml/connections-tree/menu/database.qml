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

    InlineMenu {
        id: dbMenu

        property string liveUpdateItem: 'live_update'

        callbacks: {
            "filter": function() { root.state = "filter" },
            'live_update': function() {
                if (!connectionsManager)
                    return
                connectionsManager.setMetadata(styleData.index, "live_update", true)

                itemsModel.setProperty(2, "icon", "qrc:/images/live_update_disable.png")
                itemsModel.setProperty(2, "callback", "live_update_disable")
            },
            'live_update_disable': function() {
                if (!connectionsManager)
                    return
                connectionsManager.setMetadata(styleData.index, "live_update", true)

                itemsModel.setProperty(2, "icon", "qrc:/images/live_update.png")
                itemsModel.setProperty(2, "callback", "live_update")
            }
        }

        ListModel {
            id: itemsModel

            ListElement {
                icon: "qrc:/images/filter.png"
                callback: "filter"
                help: qsTr("Open Keys Filter")
            }

            ListElement {
                icon: "qrc:/images/refresh.png"
                event: "reload"
                help: qsTr("Reload Keys in Database")
            }

            ListElement {
                icon: "qrc:/images/live_update.png"
                callback: "live_update"
                help: (callback == "live_update") ? qsTr("Enable Live Update") : qsTr("Disable Live Update")
            }

            ListElement {
                icon: "qrc:/images/add.png"
                event: "add_key"
                help: qsTr("Add New Key")
            }

            Component.onCompleted: {
                if (connectionsManager && connectionsManager.getMetadata(styleData.index, "live_update")) {
                    setProperty(2, "icon", "qrc:/images/live_update_disable.png")
                    setProperty(2, "callback", "live_update_disable")
                }
            }
        }

        model: itemsModel
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
