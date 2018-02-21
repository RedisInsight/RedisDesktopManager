import QtQuick 2.5
import QtQuick.Layouts 1.1
import QtQuick.Controls 1.4
import QtQuick.Window 2.2
import "./../../common/platformutils.js" as PlatformUtils
import "."

RowLayout {
    id: root

    focus: true

    state: "menu"

    states: [
        State {
            name: "menu"
            PropertyChanges { target: dbMenu; visible: true;}
            PropertyChanges { target: filterMenu; visible: false;}
        },
        State {
            name: "filter"
            PropertyChanges { target: dbMenu; visible: false;}
            PropertyChanges { target: filterMenu; visible: true;}
        }
    ]

    Keys.onPressed: {
        if (state == "filter" && event.key == Qt.Key_Escape) {
            state = "menu"
        }
    }

    InlineMenu {
        id: dbMenu

        Layout.fillWidth: true

        callbacks: {
            "filter": function() {
                root.state = "filter"
            },
            "live_update": function () {
                if (styleData.value["live_update"]) {
                    connectionsManager.setMetadata(styleData.index, "live_update", '')
                } else {
                    connectionsManager.setMetadata(styleData.index, "live_update", true)
                }
            }
        }

        model: [
            {
                'icon': "qrc:/images/filter.svg", 'callback': 'filter', "help": qsTr("Open Keys Filter"),
                "shortcut": PlatformUtils.isOSX()? "Meta+F" : "Ctrl+F",
            },
            {
                'icon': "qrc:/images/refresh.svg", 'event': 'reload', "help": qsTr("Reload Keys in Database"),
                "shortcut": PlatformUtils.isOSX()? "Meta+R" : "Ctrl+R",
            },
            {
                'icon': "qrc:/images/add.svg", 'event': 'add_key', "help": qsTr("Add New Key"),
                "shortcut": PlatformUtils.isOSX()? "Meta+N" : "Ctrl+N",
            },
            {
                'icon': styleData.value["live_update"]? "qrc:/images/live_update_disable.svg" : "qrc:/images/live_update.svg",
                'callback': 'live_update',
                "help": styleData.value["live_update"]? qsTr("Disable Live Update") : qsTr("Enable Live Update"),
                "shortcut": PlatformUtils.isOSX()? "Meta+L" : "Ctrl+L",
            },
            {
                'icon': "qrc:/images/cleanup.svg", 'event': 'flush', "help": qsTr("Flush DB"),
                "shortcut": PlatformUtils.isOSX()? "Meta+Del" : "Ctrl+Del",
            },
        ]
    }

    RowLayout {
        id: filterMenu

        spacing: 0

        Layout.fillWidth: true

        TextField {
            id: filterText

            Layout.fillWidth: true

            placeholderText: qsTr("Enter Filter")
            objectName: "rdm_inline_menu_filter_field"

            text: styleData.value["filter"]

            onAccepted: {
                filterOk.setFilter()
                focus = false
            }
        }

        ToolButton {
            id: filterOk
            iconSource: "qrc:/images/ok.svg"
            objectName: "rdm_inline_menu_button_apply_filter"

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
            onClicked: Qt.openUrlExternally("http://docs.redisdesktop.com/en/latest/features/#search-in-connection-tree")
        }

        ToolButton {
            id: filterCancel
            iconSource: "qrc:/images/clear.svg"
            objectName: "rdm_inline_menu_button_reset_filter"

            onClicked: {
                if (!connectionsManager)
                    return

                connectionsManager.setMetadata(styleData.index, "filter", "")
                root.state = "menu"
            }
        }
    }
}
