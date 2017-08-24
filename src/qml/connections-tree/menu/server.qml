import QtQuick 2.5
import QtQuick.Layouts 1.1
import QtQuick.Controls 1.4
import "."

RowLayout {
    id: root

    property var shortcuts: {
        'console': Qt.platform.os == "osx"? "Meta+T" : "Ctrl+T",
        'reload': Qt.platform.os == "osx"? "Meta+R" : "Ctrl+R",
        'unload': Qt.platform.os == "osx"? "Meta+U" : "Ctrl+U",
        'edit': Qt.platform.os == "osx"? "Meta+E" : "Ctrl+E",
        'delete': Qt.platform.os == "osx"? "Meta+Del" : "Ctrl+Del",
    }

    function sc(t, a) {
        return t + " (" + root.shortcuts[a] + ")"
    }

    InlineMenu {
        id: serverMenu
        model: [
            {'icon': "qrc:/images/console.svg", 'event': 'console', "help": sc(qsTr("Open Console"), 'console')},
            {'icon': "qrc:/images/refresh.svg", 'event': 'reload', "help": sc(qsTr("Reload Server"), 'reload')},
            {'icon': "qrc:/images/offline.svg", 'event': 'unload', "help": sc(qsTr("Unload All Data"), 'unload')},
            {'icon': "qrc:/images/editdb.svg", 'event': 'edit', "help": sc(qsTr("Edit Connection Settings"), 'edit')},
            {'icon': "qrc:/images/delete.svg", 'event': 'delete', "help": sc(qsTr("Delete Connection"), 'delete')},
        ]
    }

    Shortcut {
        sequence: root.shortcuts['console']
        onActivated: {
            serverMenu.sendEvent('console')
        }
    }

    Shortcut {
        sequence: root.shortcuts['reload']
        onActivated: {
            serverMenu.sendEvent('reload')
        }
    }

    Shortcut {
        sequence: root.shortcuts['unload']
        onActivated: {
            serverMenu.sendEvent('unload')
        }
    }

    Shortcut {
        sequence: root.shortcuts['edit']
        onActivated: {
            serverMenu.sendEvent('edit')
        }
    }

    Shortcut {
        sequence: root.shortcuts['delete']
        onActivated: {
            serverMenu.sendEvent('delete')
        }
    }
}
