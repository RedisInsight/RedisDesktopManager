import QtQuick 2.3
import QtQuick.Layouts 1.1
import QtQuick.Controls 1.4
import "."
import "./../../common/platformutils.js" as PlatformUtils

InlineMenu {
    id: root

    callbacks: {
        "copy": function() {
            var result = styleData.value["full_path"]

            if (result) {
                qmlUtils.copyToClipboard(result + ":*")
            }
        },
    }

    model: {
        if (styleData.value["locked"] === true) {
            return [
                        {
                            'icon': PlatformUtils.getThemeIcon("offline.svg"), 'event': 'cancel', "help": qsTranslate("RESP","Disconnect"),
                        },
                    ]
        } else {
            [
                {'icon': PlatformUtils.getThemeIcon("refresh.svg"), "event": "reload", "help": qsTranslate("RESP","Reload Namespace"), "shortcut": "Ctrl+R"},
                {'icon': PlatformUtils.getThemeIcon("add.svg"), 'event': 'add_key', "help": qsTranslate("RESP","Add New Key")},
                {'icon': PlatformUtils.getThemeIcon("copy.svg"), "callback": "copy", "help": qsTranslate("RESP","Copy Namespace Pattern"), "shortcut": "Ctrl+C"},
                {'icon': PlatformUtils.getThemeIcon("memory_usage.svg"), "event": "analyze_memory_usage", "help": qsTranslate("RESP","Analyze Used Memory")},
                {'icon': PlatformUtils.getThemeIcon("delete.svg"), "event": "delete", "help": qsTranslate("RESP","Delete Namespace"), "shortcut": "D"},
            ]
        }
    }
}

