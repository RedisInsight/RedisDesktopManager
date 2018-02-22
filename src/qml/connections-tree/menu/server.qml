import QtQuick 2.5
import QtQuick.Layouts 1.1
import QtQuick.Controls 1.4
import "."

InlineMenu {
    id: root

    model: [
        {
            'icon': "qrc:/images/log.svg", 'event': 'server_info', "help": qsTr("Server Info"),
            "shortcut": Qt.platform.os == "osx"? "Meta+I" : "Ctrl+I"
        },
        {
            'icon': "qrc:/images/console.svg", 'event': 'console', "help": qsTr("Open Console"),
            "shortcut": Qt.platform.os == "osx"? "Meta+T" : "Ctrl+T",
        },
        {
            'icon': "qrc:/images/refresh.svg", 'event': 'reload', "help": qsTr("Reload Server"),
            "shortcut": Qt.platform.os == "osx"? "Meta+R" : "Ctrl+R",
        },
        {
            'icon': "qrc:/images/offline.svg", 'event': 'unload', "help": qsTr("Unload All Data"),
            "shortcut": Qt.platform.os == "osx"? "Meta+U" : "Ctrl+U",
        },
        {
            'icon': "qrc:/images/editdb.svg", 'event': 'edit', "help": qsTr("Edit Connection Settings"),
            "shortcut": Qt.platform.os == "osx"? "Meta+E" : "Ctrl+E",
        },
        {
            'icon': "qrc:/images/delete.svg", 'event': 'delete', "help": qsTr("Delete Connection"),
            "shortcut": Qt.platform.os == "osx"? "Meta+Del" : "Ctrl+Del",
        },
    ]
}
