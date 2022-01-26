import QtQuick 2.5
import QtQuick.Layouts 1.1
import QtQuick.Controls 1.4
import "."
import "./../../common/platformutils.js" as PlatformUtils

InlineMenu {
    id: root

    model: {
        if (styleData.value["locked"] === true) {
            return [
                        {
                            'icon': PlatformUtils.getThemeIcon("offline.svg"), 'event': 'cancel', "help": qsTranslate("RESP","Disconnect"),
                        },
                    ]
        } else {
            return [
                        {
                            'icon': PlatformUtils.getThemeIcon("log.svg"), 'event': 'server_info', "help": qsTranslate("RESP","Server Info"),
                            "shortcut": PlatformUtils.isOSX()? "Meta+I" : "Ctrl+I"
                        },
                        {
                            'icon': PlatformUtils.getThemeIcon("console.svg"), 'event': 'console', "help": qsTranslate("RESP","Open Console"),
                            "shortcut": PlatformUtils.isOSX()? "Meta+T" : "Ctrl+T",
                        },
                        {
                            'icon': PlatformUtils.getThemeIcon("refresh.svg"), 'event': 'reload', "help": qsTranslate("RESP","Reload Server"),
                            "shortcut": PlatformUtils.isOSX()? "Meta+R" : "Ctrl+R",
                        },
                        {
                            'icon': PlatformUtils.getThemeIcon("offline.svg"), 'event': 'unload', "help": qsTranslate("RESP","Unload All Data"),
                            "shortcut": PlatformUtils.isOSX()? "Meta+U" : "Ctrl+U",
                        },
                        {
                            'icon': PlatformUtils.getThemeIcon("settings.svg"), 'event': 'edit', "help": qsTranslate("RESP","Edit Connection Settings"),
                            "shortcut": PlatformUtils.isOSX()? "Meta+E" : "Ctrl+E",
                        },
                        {
                            'icon': PlatformUtils.getThemeIcon("copy.svg"), 'event': 'duplicate', "help": qsTranslate("RESP","Duplicate Connection"),
                            "shortcut": PlatformUtils.isOSX()? "Meta+C" : "Ctrl+C",
                        },
                        {
                            'icon': PlatformUtils.getThemeIcon("delete.svg"), 'event': 'delete', "help": qsTranslate("RESP","Delete Connection"),
                            "shortcut": PlatformUtils.isOSX()? "Meta+Del" : "Ctrl+Del",
                        },
                    ]
        }
    }
}
