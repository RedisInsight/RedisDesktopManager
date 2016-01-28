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

        callbacks: {
            "filter": function() { root.state = "filter" }
        }

        model: [
            {'icon': "qrc:/images/filter.png", "callback": "filter", "help": "Open Filter"},
            {'icon': "qrc:/images/refresh.png", 'event': 'reload', "help": "Reload Keys in Database"},
            {'icon': "qrc:/images/add.png", 'event': 'add_key', "help": "Add New Key"},
        ]
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
