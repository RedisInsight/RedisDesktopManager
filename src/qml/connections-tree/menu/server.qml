import QtQuick 2.3
import QtQuick.Layouts 1.1
import QtQuick.Controls 1.4
import "."

RowLayout {
    InlineMenu {
        model: [
            {'icon': "qrc:/images/console.png", 'event': 'console', "help": "Open Console"},
            {'icon': "qrc:/images/refresh.png", 'event': 'reload', "help": "Reload Server"},
            {'icon': "qrc:/images/offline.png", 'event': 'unload', "help": "Unload All Data"},
            {'icon': "qrc:/images/editdb.png", 'event': 'edit', "help": "Edit Connection Settings"},
            {'icon': "qrc:/images/delete.png", 'event': 'delete', "help": "Delete Connection"},
        ]
    }
}
