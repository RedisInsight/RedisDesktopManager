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
                            'icon': "qrc:/images/offline.svg", 'event': 'cancel', "help": qsTranslate("RDM","Disconnect"),
                        },
                    ]
        } else {
            return [
                        {
                            'icon': "qrc:/images/log.svg", 'event': 'server_info', "help": qsTranslate("RDM","Server Info"),
                            "shortcut": Qt.platform.os == "osx"? "Meta+I" : "Ctrl+I"
                        },
                        {
                            'icon': "qrc:/images/console.svg", 'event': 'console', "help": qsTranslate("RDM","Open Console"),
                            "shortcut": Qt.platform.os == "osx"? "Meta+T" : "Ctrl+T",
                        },
                        {
                            'icon': "qrc:/images/refresh.svg", 'event': 'reload', "help": qsTranslate("RDM","Reload Server"),
                            "shortcut": Qt.platform.os == "osx"? "Meta+R" : "Ctrl+R",
                        },
                        {
                            'icon': "qrc:/images/offline.svg", 'event': 'unload', "help": qsTranslate("RDM","Unload All Data"),
                            "shortcut": Qt.platform.os == "osx"? "Meta+U" : "Ctrl+U",
                        },
                        {
                            'icon': "qrc:/images/settings.svg", 'event': 'edit', "help": qsTranslate("RDM","Edit Connection Settings"),
                            "shortcut": Qt.platform.os == "osx"? "Meta+E" : "Ctrl+E",
                        },
                        {
                            'icon': "qrc:/images/copy.svg", 'event': 'duplicate', "help": qsTranslate("RDM","Duplicate Connection"),
                            "shortcut": Qt.platform.os == "osx"? "Meta+C" : "Ctrl+C",
                        },
                        {
                            'icon': "qrc:/images/delete.svg", 'event': 'delete', "help": qsTranslate("RDM","Delete Connection"),
                            "shortcut": Qt.platform.os == "osx"? "Meta+Del" : "Ctrl+Del",
                        },
                    ]
        }
    }
}
