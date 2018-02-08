import QtQuick 2.5
import QtQuick.Layouts 1.1
import QtQuick.Controls 1.4
import QtQuick.Window 2.2
import "./../../common/platformutils.js" as PlatformUtils
import "."

RowLayout {
    id: root

    focus: true
    implicitHeight: itemHeight
    property int itemHeight: PlatformUtils.isOSXRetina(Screen)? 20 : 25

    Keys.onPressed: {
        if (state == "filter" && event.key == Qt.Key_Escape) {
            state = "menu"
        }
    }

    property var shortcuts: {
        'add': PlatformUtils.isOSX()? "Meta+N" : "Ctrl+N",
        'reload': PlatformUtils.isOSX()? "Meta+R" : "Ctrl+R",
        'filter': PlatformUtils.isOSX()? "Meta+F" : "Ctrl+F",
        'live': PlatformUtils.isOSX()? "Meta+L" : "Ctrl+L",
        'flush': PlatformUtils.isOSX()? "Meta+Del" : "Ctrl+Del",
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

    Shortcut {
        sequence: root.shortcuts['flush']
        onActivated: {
            dbMenu.sendEvent('flush')
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
            objectName: "rdm_inline_menu_button_filter"

            Layout.preferredWidth: root.itemHeight
            Layout.preferredHeight: root.itemHeight

            onClicked: root.state = "filter"
        }

        ToolButton {
            tooltip: sc(qsTr("Reload Keys in Database"), 'reload')
            iconSource: "qrc:/images/refresh.svg"
            objectName: "rdm_inline_menu_button_reload_db"

            Layout.preferredWidth: root.itemHeight
            Layout.preferredHeight: root.itemHeight

            onClicked: dbMenu.sendEvent("reload")
        }

        ToolButton {
            tooltip: sc(qsTr("Add New Key"), 'add')
            iconSource: "qrc:/images/add.svg"
            objectName: "rdm_inline_menu_button_add_key"

            Layout.preferredWidth: root.itemHeight
            Layout.preferredHeight: root.itemHeight

            onClicked: dbMenu.sendEvent("add_key")
        }

        ToolButton {
            id: liveButton

            property bool liveUpdateEnabled: false
            tooltip: liveUpdateEnabled? qsTr("Disable Live Update") : sc(qsTr("Enable Live Update"), 'live')
            iconSource: liveUpdateEnabled? "qrc:/images/live_update_disable.svg" : "qrc:/images/live_update.svg"
            objectName: "rdm_inline_menu_button_live_update"

            Layout.preferredWidth: root.itemHeight
            Layout.preferredHeight: root.itemHeight

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

        ToolButton {
            tooltip: sc(qsTr("Flush DB"), 'flush')
            iconSource: "qrc:/images/cleanup.svg"
            objectName: "rdm_inline_menu_button_flush_db"

            Layout.preferredWidth: root.itemHeight
            Layout.preferredHeight: root.itemHeight

            onClicked: dbMenu.sendEvent("flush")
        }
    }

    TextField {
        id: filterText
        placeholderText: qsTr("Enter Filter")
        objectName: "rdm_inline_menu_filter_field"

        text: {
            if (!connectionsManager)
                return

            return connectionsManager.getMetadata(styleData.index, "filter")
        }

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
        onClicked: Qt.openUrlExternally("https://github.com/uglide/RedisDesktopManager/wiki/Features#Filter")
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
