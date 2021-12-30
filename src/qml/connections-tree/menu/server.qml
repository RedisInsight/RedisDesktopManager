import QtQuick 2.5
import QtQuick.Layouts 1.1
import QtQuick.Controls 1.4
import "."

InlineMenu {
    id: root

    model: {
        if (styleData.value["locked"] === true) {
            return [
                        {
                            'icon': "qrc:/images/offline.svg", 'event': 'cancel', "help": qsTranslate("RESP","Disconnect"),
                        },
                    ]
        } else {
            return [
                        {
                            'icon': "qrc:/images/log.svg", 'event': 'server_info', "help": qsTranslate("RESP","Server Info"),
                            "shortcut": Qt.platform.os == "osx"? "Meta+I" : "Ctrl+I"
                        },
                        {
                            'icon': "qrc:/images/console.svg", 'event': 'console', "help": qsTranslate("RESP","Open Console"),
                            "shortcut": Qt.platform.os == "osx"? "Meta+T" : "Ctrl+T",
                        },
                        {
                            'icon': "qrc:/images/refresh.svg", 'event': 'reload', "help": qsTranslate("RESP","Reload Server"),
                            "shortcut": Qt.platform.os == "osx"? "Meta+R" : "Ctrl+R",
                        },
                        {
                            'icon': "qrc:/images/offline.svg", 'event': 'unload', "help": qsTranslate("RESP","Unload All Data"),
                            "shortcut": Qt.platform.os == "osx"? "Meta+U" : "Ctrl+U",
                        },
                        {
                            'icon': "qrc:/images/settings.svg", 'event': 'edit', "help": qsTranslate("RESP","Edit Connection Settings"),
                            "shortcut": Qt.platform.os == "osx"? "Meta+E" : "Ctrl+E",
                        },
                        {
                            'icon': "qrc:/images/copy.svg", 'event': 'duplicate', "help": qsTranslate("RESP","Duplicate Connection"),
                            "shortcut": Qt.platform.os == "osx"? "Meta+C" : "Ctrl+C",
                        },
                        {
                            'icon': "qrc:/images/delete.svg", 'event': 'delete', "help": qsTranslate("RESP","Delete Connection"),
                            "shortcut": Qt.platform.os == "osx"? "Meta+Del" : "Ctrl+Del",
                        },
                    ]
        }
    }
}
