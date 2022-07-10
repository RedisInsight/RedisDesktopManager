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
                            'icon': PlatformUtils.getThemeIcon("refresh.svg"), 'event': 'reload', "help": qsTranslate("RESP","Reload Server"),
                            "shortcut": "Ctrl+R",
                        },
                        {
                            'icon': PlatformUtils.getThemeIcon("offline.svg"), 'event': 'unload', "help": qsTranslate("RESP","Unload All Data"),
                            "shortcut": "Ctrl+U",
                        },
                        {
                            'icon': PlatformUtils.getThemeIcon("settings.svg"), 'event': 'edit', "help": qsTranslate("RESP","Edit Connection Settings"),
                            "shortcut": "Ctrl+E",
                        },
                        {
                            'icon': PlatformUtils.getThemeIcon("copy.svg"), 'event': 'duplicate', "help": qsTranslate("RESP","Duplicate Connection"),
                            "shortcut": "Ctrl+C",
                        },
                        {
                            'icon': PlatformUtils.getThemeIcon("delete.svg"), 'event': 'delete', "help": qsTranslate("RESP","Delete Connection"),
                            "shortcut": "D",
                        },
                    ]
        }
    }
}
